// internal
#include "ai_system.hpp"
#include "world_init.hpp"
#include <limits>
#include "physics_system.hpp"
#include <iostream>

const size_t FISH_DELAY_MS = 5000 * 3;
float current_speed = 1.f;

float minDis = std::numeric_limits<float>::infinity();

float game_width;
float game_height;
int base_heuristic[21][21];
int heuristic_matrix[21][21];
bool heuristic_initialized = false;
bool update = false;

void AISystem::init(RenderSystem* renderer_arg) {
    this->renderer = renderer_arg;
}

void AISystem::SetDimensions(float width, float height) {
    game_width = width*2;
    game_height = height*2;
}

void AISystem::flagUpdate() {
    update = true;
}

void AISystem::updateNextPos(Entity e) {
//    Motion m = motion_container.get(e);
    auto& m = registry.motions.get(e);
    SkullType st = registry.hardShells.get(e).type;
    
    if (m.position.x < -game_width/4 || m.position.x > (game_width/2)*1.5 || m.position.y < -game_height/4 || m.position.y > (game_height/2)*1.5 ) {
        return;
    }

    vec2 heuristicIdx = getHeuristicIdxFromPos(m.position);
    vec2 dest_pos;
    float i = heuristicIdx.x;
    float j = heuristicIdx.y;

//    printf("cur pos is %f %f \n", m.position.x, m.position.y);
    vec2 curIdx = getHeuristicIdxFromPos(m.position);
    vec2 goalIdx = m.destNode;

    //USED FOR DEBUGGINB
//    printf("cur idx is %f %f, goal is %f %f , velocity is %f %f \n", curIdx.x, curIdx.y, goalIdx.x, goalIdx.y, m.velocity.x, m.velocity.y);

    if ((curIdx.x == goalIdx.x && curIdx.y == goalIdx.y) || (goalIdx.x == -1 && goalIdx.y == -1)) {
        // if youre close, just go to the middle
        if (9 <= i && i <= 11 && 10 <= j && j <= 12) {
            dest_pos = vec2(game_width/4, game_height/4);
            m.destNode = getHeuristicIdxFromPos(dest_pos);
        } else {
            // value, x index, y index
            vec3 lowest = vec3(-1, -1, INFINITY);
            // update the lowest index (check up down left right)
            if (i > 1) {
                int val = heuristic_matrix[(int)i-1][(int)j];
                if (val < lowest.z && (m.pastNodes[(int)i-1][(int)j] != 1)) {
                    lowest.z = val;
                    lowest.x = i-1;
                    lowest.y = j;
                }
            }
            if (i < 21) {

                int val = heuristic_matrix[(int)i+1][(int)j];
                if (val < lowest.z && (m.pastNodes[(int)i+1][(int)j] != 1)) {
                    lowest.z = val;
                    lowest.x = i+1;
                    lowest.y = j;
                }
            }
            if (j > 1) {
                int val = heuristic_matrix[(int)i][(int)j-1];
                if (val < lowest.z && (m.pastNodes[(int)i][(int)j-1] != 1)) {
                    lowest.z = val;
                    lowest.x = i;
                    lowest.y = j-1;
                }
            }
            if (j < 21) {
                int val = heuristic_matrix[(int)i][(int)j+1];
                if (val < lowest.z && (m.pastNodes[(int)i][(int)j+1] != 1)) {
                    lowest.z = val;
                    lowest.x = i;
                    lowest.y = j+1;
                }
            }

            vec2 destNode = vec2(lowest.x, lowest.y);
            dest_pos = getPosFromHeuristicIdx(destNode);
            m.destNode = destNode;
        }


        //starting pos is cur pos, dest is dest, calculate velocity
        // use game coordinates
        vec2 start_pos = {m.position.x + game_width/4, m.position.y + game_height/4};
        dest_pos = {dest_pos.x + game_width/4, dest_pos.y + game_height/4};

        float speedFact = (st == SkullType::TWO) ? 1 : 1.5;
        float max_speed = 50*speedFact;
        float x_unit = start_pos.x > dest_pos.x ? -1 : 1;
        float y_unit = start_pos.y > dest_pos.y ? -1 : 1;

        float x_component = abs(dest_pos.x - start_pos.x);
        float y_component = abs(dest_pos.y - start_pos.y);
        float x_factor = x_component/(x_component + y_component);
        float y_factor = y_component/(x_component + y_component);

        float final_y_speed = y_factor * max_speed * y_unit;
        float final_x_speed = x_factor * max_speed * x_unit;

        vec2 t = getHeuristicIdxFromPos(m.position);

        //debug statements

//        auto temp1 = heuristic_matrix[(int)t.x][(int)t.y];
//        auto temp2 = heuristic_matrix[(int)m.destNode.x][(int)m.destNode.y];
//        printf("game center with whatever is %f %f \n", getHeuristicIdxFromPos(vec2(1280, 540)).x, getHeuristicIdxFromPos(vec2(1280, 540)).y);
//        printf("dest pos is %f %f \n", dest_pos.x, dest_pos.y);
//        printf("cur IDX is %f %f going to %f %f \n", t.x, t.y, m.destNode.x, m.destNode.y);
//        printf("cur heuristic value is %f  going to  %f \n", (float) temp1, (float) temp2);
//        printf("cur POS is %f %f going to %f %f \n", m.position.x, m.position.y, dest_pos.x - game_width/4, dest_pos.y - game_height/4);
//        printf("final xy velocity is %f %f \n", final_x_speed, final_y_speed);
//        for (int i = 0; i < 21; i++)
//        {
//           for (int j = 0; j < 21; j++)
//           {
//               std::cout << heuristic_matrix[j][i] << " ";
//           }
//
//           // Newline for new row
//            std::cout << std::endl;
//        }
//        std::cout << std::endl;
//        std::cout << std::endl;

        m.velocity = vec2(final_x_speed, final_y_speed);
        m.pastNodes[(int)t.x][(int)t.y] = 1;
    }
}

bool AISystem::compareHeuristic(vec2 lowest_pos, vec2 check) {
    int x1 = (int)lowest_pos.x;
    int y1 = (int)lowest_pos.y;
    int x2 = (int)check.x;
    int y2 = (int)check.y;

    return heuristic_matrix[x1][y1] < heuristic_matrix[x2][y2];
}

void AISystem::updateHeuristic() {
    ComponentContainer<Turret> &turrets = registry.turrets;
    ComponentContainer<Motion> &motions = registry.motions;
    for (int i = 0; i < turrets.size(); i++) {
        Entity currentTurret = turrets.entities[i];
        Motion currentTurretMotion = motions.get(currentTurret);

        vec2 relevantIdx = getHeuristicIdxFromPos(currentTurretMotion.position);
        int xIdx = relevantIdx.x;
        int yIdx = relevantIdx.y;
        float new_val = heuristic_matrix[xIdx][yIdx] - 5;
        heuristic_matrix[xIdx][yIdx] = new_val;
    }

    ComponentContainer<Wall> &walls = registry.walls;
    for (int i = 0; i < walls.size(); i++) {
        Entity currentWall = walls.entities[i];
        Motion currentWallMotion = motions.get(currentWall);

        vec2 relevantIdx = getHeuristicIdxFromPos(currentWallMotion.position);
        int xIdx = relevantIdx.x;
        int yIdx = relevantIdx.y;
        float new_val = heuristic_matrix[xIdx][yIdx] + 5;
        heuristic_matrix[xIdx][yIdx] = new_val;
    }
}

vec2 AISystem::getPosFromHeuristicIdx(vec2 idx) {
    double idxWidth = game_width/21;
    double idxHeight = game_height/21;


    float x_pos = (idxWidth*idx.x) - game_width/4;
    float y_pos = (idxHeight*idx.y) - game_height/4;

    return vec2(x_pos,y_pos);
}

vec2 AISystem::getHeuristicIdxFromPos(vec2 pos) {
    double idxWidth = game_width/21;
    double idxHeight = game_height/21;

    //since 00 is middle of the map (we have negatives), reset top left to 00
    vec2 uniPos = vec2(float(pos.x + game_width/4), float(pos.y + game_height/4));

    float xIdx = round(uniPos.x/idxWidth);
    float yIdx = round(uniPos.y/idxHeight);

    return vec2(xIdx,yIdx);
}

float AISystem::relativeEuclidianDist(float row, float col) {
    // 10, 11 is the middle
    double x = abs(row - 10); //calculating number to square in next step
    double y = abs(col - 11);
    double dist;

    dist = pow(x, 2) + pow(y, 2);       //calculating Euclidean distance
    dist = sqrt(dist);

    return (float)dist;
}

void AISystem::resetHeuristic() {
    for (int i = 0; i<21; i++) {
        // rows
        for (int j = 0; j<21; j++) {
            heuristic_matrix[i][j] = relativeEuclidianDist(i, j);
        }

    }
    heuristic_matrix[10][10] = 0;

    heuristic_initialized = true;
}

void AISystem::step(float elapsed_ms)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE FISH AI HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will likely want to write new functions and need to create
	// new data structures to implement a more sophisticated Fish AI.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on AI path
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    if (!heuristic_initialized) {
        resetHeuristic();
    }

    if (update) {
        resetHeuristic();
        updateHeuristic();
        update= false;
    }

    ComponentContainer<HardShell> &hardshell_container = registry.hardShells;

//    printf("size of hardshell is %zu", hardshell_container.size());

    for (int i = 0; i< hardshell_container.size(); i++) {
        Entity e = hardshell_container.entities[i];
//        Motion m = motion_container.get(e);
        updateNextPos(e);
    }


    // 1. set epsilon
    float epsilon = 400.f;
    
    // turrent attacking skull
    // 2. check distance between turret and skull
    // declare turrent
    auto& turret_registry = registry.turrets;
    for (uint i = 0; i<turret_registry.size(); i++){
        Turret& tower = registry.turrets.get(turret_registry.entities[i]);
        Motion& tm = registry.motions.get(turret_registry.entities[i]);
        vec2 towpos = tm.position;
        if (tower.attack == false){
            auto& skull_registry = registry.hardShells;
            Entity cs;
            float dis = minDis;
            for(uint i = 0; i < skull_registry.size(); i++) {
                Entity skull = skull_registry.entities[i];
                if (registry.states.get(skull).current_state != SkullState::Dying) {
                    Motion& motion = registry.motions.get(skull);
                    vec2 skullpos = motion.position;
    //                vec2 skullvel = motion.velocity;
                    // calculate distance between castle and skull
                    float h = sqrtf(pow(towpos.x - skullpos.x, 2)+ pow(towpos.y-skullpos.y, 2));
                    //float xdis = abs(caspos.x - skullpos.x);
                    //float ydis = abs(caspos.y - skullpos.y);
                    if (h <= epsilon && h < dis){
                        cs = skull;
                        dis = h;
                    }
                }
            }
            if (registry.hardShells.has(cs)) {
                tower.cSkull = cs;
                tower.attack = true;
                bool hasLight = false;
                for(uint i = 0; i< registry.light.size(); i++) {
                    Light& light = registry.light.components[i];
                    if (light.target == cs) {
                        hasLight = true;
                        break;
                    }
                }
                if (!hasLight) {
                    createLight(renderer, registry.motions.get(cs).position, cs);
                }
            }
            
        }
        if (tower.attack == true){
            tower.counter -= elapsed_ms * current_speed;
            if (tower.counter < 0.f) {
                tower.counter = 3000;
                Entity bullet;
                if (tower.tt == TurretType::ONE)
                    bullet = createBullet(renderer, towpos, 100);
                else
                    bullet = createBullet(renderer, towpos, 300);
                SoftShell& b = registry.softShells.get(bullet);
                b.target = tower.cSkull;
            }
        }
        auto& bullet_registry = registry.softShells;
        for(uint i = 0; i< bullet_registry.size(); i++) {
            SoftShell& s = registry.softShells.components[i];
            if (registry.states.get(s.target).current_state != SkullState::Dying){
                Motion& sm = registry.motions.get(s.target);
                Motion& bm = registry.motions.get(bullet_registry.entities[i]);
                float angle = atan2(sm.position.y - bm.position.y, sm.position.x - bm.position.x);
                bm.velocity.x = 200*cos(angle);
                bm.velocity.y = 200*sin(angle);
                //bm.angle = atan2(-sm.position.y + bm.position.y, -sm.position.x + bm.position.x);
                bm.angle = angle;
            } else {
                registry.remove_all_components_of(bullet_registry.entities[i]);
            }
        }
    }
    
}

// Compute collisions between entities
//void AISystem::handle_collisions() {
//    // Loop over all collisions detected by the physics system
//    auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
//    for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
//        // The entity and its collider
//        Entity entity = collisionsRegistry.entities[i];
//        Entity entity_other = collisionsRegistry.components[i].other;
//
//        // Collisions that involve the bullet
//        if (registry.softShells.has(entity)){
//            // Checking Bullet - HardShell collisions
//            if (registry.hardShells.has(entity_other)) {
//                // 2. decrement hardshell health status by the bullet damage
//                //float skullHealth = registry.hardShells.get(entity_other).skull_health;
//                //float bulletDamage = registry.softShells.get(entity).bullet_damage;
//                registry.hardShells.get(entity_other).skull_health -= registry.softShells.get(entity).bullet_damage;
//                registry.remove_all_components_of(entity);
//                if (registry.hardShells.get(entity_other).skull_health <= 0){
//                    registry.remove_all_components_of(entity_other);
//                    auto& motion_registry = registry.motions;
//                    for(uint x = 0; x < motion_registry.size(); x++) {
//                        Entity bullet = motion_registry.entities[x];
//                        if (registry.softShells.has(bullet)) {
//                            registry.remove_all_components_of(bullet);
//                        }
//                    }
//                    Entity castle = registry.players.entities[0];
//                    registry.players.get(castle).attack = false;
//                }
//            }
//        }
//    }
//
//    // Remove all collisions from this simulation step
//    registry.collisions.clear();
//}
