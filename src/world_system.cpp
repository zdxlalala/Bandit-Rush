// Header
#include "world_system.hpp"
#include "world_init.hpp"

// stlib
#include <cassert>
#include <sstream>
#include "iostream"
#include <fstream>

#include "physics_system.hpp"
#include "ai_system.hpp"

// Game configuration
const size_t MAX_SKULL = 15;
const size_t MAX_FISH = 5;
const size_t SKULL_DELAY_MS = 2000 * 1;
const size_t SKULL_DELAY_MS2 = 2000 * 2;
const size_t SKULL_DELAY_MS3 = 2000 * 3;
const size_t FISH_DELAY_MS = 5000 * 3;

float POINTS_DELAY_MS = 1000;
float STEALING_DELAY_MS = 1000;
float ATTACKING_DELAY_MS = 1000;
float ems;

int ini_gold = 20;
int turret_cost = 10;
int turret2_cost = 20;
int wall_cost = 5;
int gold_stolen = 5;

enum LEVEL{
    ONE, TWO, THREE
};
int level = ONE;
float ROUNDONE_DELAY_MS = 1000*60;
float ROUNDTWO_DELAY_MS = 1000*90;
float ROUNDTHREE_DELAT_MS = 1000*120;

bool refresh = false;

enum SIDE{
    TOP, BOTTOM, LEFT, RIGHT, COUNT
};

bool idle = false;
bool saved = false;


// Create the fish world
WorldSystem::WorldSystem()
	:current_state(GameState::Menu)
    , to_attack(&t, is_walking)
    , to_die(&d, is_dead)
    , in_walk(&w, is_walking)
    , in_attack(&a, is_attacking)
    , in_idle(&i, is_idling)
    , in_dying(&di, is_dying)
    , ba(&in_walk, &in_attack, &in_idle, &in_dying)
    , points(ini_gold)
	, next_skull_spawn(0.f)
    , next_skull2_spawn(0.f)
    , next_skull3_spawn(0.f)
	, next_fish_spawn(0.f) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
    if (skull_dead_sound != nullptr)
        Mix_FreeChunk(skull_dead_sound);
    if (bullet_hit_sound != nullptr)
        Mix_FreeChunk(bullet_hit_sound);
    if (build_turret_sound != nullptr)
        Mix_FreeChunk(build_turret_sound);
    if (game_over_sound != nullptr)
        Mix_FreeChunk(game_over_sound);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char *desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window(int width, int height) {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(width, height, "Bandit Rush", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
    auto cursor_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, cursor_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("background.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
    skull_dead_sound = Mix_LoadWAV(audio_path("skull_death.wav").c_str());
    bullet_hit_sound = Mix_LoadWAV(audio_path("bullet_hit.wav").c_str());
    build_turret_sound = Mix_LoadWAV(audio_path("build_turret.wav").c_str());
    game_over_sound = Mix_LoadWAV(audio_path("game_over.wav").c_str());

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
			audio_path("background.wav").c_str(),
			audio_path("salmon_dead.wav").c_str(),
			audio_path("salmon_eat.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	// Set all states to default
    //restart_game();
	//get window width and height
	int screen_width;
	int screen_height;
	glfwGetWindowSize(window, &screen_width, &screen_height);
    
    // Create Menu Buttons
    Entity title = createButton(renderer, { screen_width /2,screen_height /2-150});
    registry.renderRequests.get(title).used_texture = TEXTURE_ASSET_ID::TITLE;
    registry.motions.get(title).scale.x *= 4;
    registry.motions.get(title).scale.y *= 2;
    
    createButton(renderer, { screen_width/2,screen_height/2+50});
    createButton(renderer, { screen_width/2,screen_height/2+150});
    
    if (level == ONE) {
        renderer->tutorial = true;
        renderer->tutorial_pause = true;
        renderer->change_txt_tut = 1;
        points = 1000;
    }
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
    ems = elapsed_ms_since_last_update;
    // Get the screen dimensions
    int screen_width;
    int screen_height;
    glfwGetWindowSize(window, &screen_width, &screen_height);

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Gold: " << points;
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
	    registry.remove_all_components_of(registry.debugComponents.entities.back());
    
    for (uint i = 0; i< registry.motions.size(); i++){
        if (!registry.renderRequests.has(registry.motions.entities[i]) || registry.motions.components[i].scale.x == 0 ||
            registry.motions.components[i].scale.y == 0)
            registry.remove_all_components_of(registry.motions.entities[i]);
    }
    
    
    
    // game over if lost all the points
    if (points < 0) {
        //glfwSetWindowShouldClose(window, 1);
        if (!renderer->is_over) {
            Mix_PlayChannel(-1, game_over_sound, 0);
        }
        points = 0;
        renderer->is_over = true;
        auto& turret_registry = registry.turrets;
        for (uint i = 0; i< turret_registry.size(); i++){
            registry.remove_all_components_of(registry.turrets.entities[i]);
        }
    }
    
    // update gold value
//    POINTS_DELAY_MS -= elapsed_ms_since_last_update;
//    if (POINTS_DELAY_MS < 0.f && !renderer->is_over) {
//        points += 1;
//        POINTS_DELAY_MS = 1000;
//    }
    // update round level
    if (level == ONE && !renderer->tutorial){
        renderer->timeRemaining = (int)ROUNDONE_DELAY_MS/1000;
        ROUNDONE_DELAY_MS -= elapsed_ms_since_last_update;
        if (ROUNDONE_DELAY_MS < 0.f) {
            level = TWO;
            renderer->levelOne = false;
            renderer->levelTwo = true;
        }
        POINTS_DELAY_MS -= elapsed_ms_since_last_update;
        if (POINTS_DELAY_MS < 0.f && !renderer->is_over) {
            points += 1;
            POINTS_DELAY_MS = 2000;
        }
    }

    if (level == TWO){
        renderer->timeRemaining = (int)ROUNDTWO_DELAY_MS/1000;
        ROUNDTWO_DELAY_MS -= elapsed_ms_since_last_update;
        if (ROUNDTWO_DELAY_MS < 0.f) {
            level = THREE;
            renderer->levelOne = false;
            renderer->levelTwo = false;
            renderer->levelThree = true;
        }
        POINTS_DELAY_MS -= elapsed_ms_since_last_update;
        if (POINTS_DELAY_MS < 0.f && !renderer->is_over) {
            points += 1;
            POINTS_DELAY_MS = 4000;
        }
    }

    if (level == THREE){
        renderer->timeRemaining = (int)ROUNDTHREE_DELAT_MS/1000;
        ROUNDTHREE_DELAT_MS -= elapsed_ms_since_last_update;
        if (POINTS_DELAY_MS < 0.f && !renderer->is_over) {
            points += 1;
            POINTS_DELAY_MS = 6000;
        }
    }
    renderer->gold = points;

	// Removing out of screen entities
	//auto& motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
//	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
//	    Motion& motion = motions_registry.components[i];
//		if (motion.position.x + abs(motion.scale.x) < 0.f) {
//		    registry.remove_all_components_of(motions_registry.entities[i]);
//		}
//	}
    
    // Skull walikng animation
    for (int i = (int)registry.states.components.size()-1; i>=0; --i) {
        Entity e = registry.states.entities[i];
        ba.process(e);
    }

	// Spawning new skull
	next_skull_spawn -= elapsed_ms_since_last_update * current_speed;
    if ((renderer->tutorial && !renderer->tutorial_pause && registry.hardShells.components.size() <= 5)
    || (registry.hardShells.components.size() <= MAX_SKULL && next_skull_spawn < 0.f && !renderer->tutorial)) {
		// Reset timer
		next_skull_spawn = (SKULL_DELAY_MS / 2) + uniform_dist(rng) * (SKULL_DELAY_MS / 2);
		// Create skull randomly across the edges
        int side = rand()%COUNT;
        int spawnX = (side < 2) * (rand()%(screen_width+screen_width/2) - screen_width/2) + (side == RIGHT) * (screen_width+screen_width/2);
        int spawnY = (side > 1) * (rand()%(screen_height+screen_height/2) - screen_height/2) + (side == BOTTOM) * (screen_height+screen_height/2);
		createSkull(renderer, {spawnX,spawnY}, screen_width, screen_height, 0);
	}

    // Spawning new skull2
    if (level == TWO || level == THREE){
        next_skull2_spawn -= elapsed_ms_since_last_update * current_speed;
        if (next_skull2_spawn < 0.f) {
            // Reset timer
            next_skull2_spawn = (SKULL_DELAY_MS2 / 2) + uniform_dist(rng) * (SKULL_DELAY_MS2 / 2);
            // Create skull randomly across the edges
            int side = rand()%COUNT;
            int spawnX = (side < 2) * (rand()%(screen_width+screen_width/2) - screen_width/2) + (side == RIGHT) * (screen_width+screen_width/2);
            int spawnY = (side > 1) * (rand()%(screen_height+screen_height/2) - screen_height/2) + (side == BOTTOM) * (screen_height+screen_height/2);
            createSkull(renderer, {spawnX,spawnY}, screen_width, screen_height, 1);
        }
    }
    // Spawning new skull3
    if (level == THREE){
        next_skull3_spawn -= elapsed_ms_since_last_update * current_speed;
        if (next_skull3_spawn < 0.f) {
            // Reset timer
            next_skull3_spawn = (SKULL_DELAY_MS3 / 2) + uniform_dist(rng) * (SKULL_DELAY_MS3 / 2);
            // Create skull randomly across the edges
            int side = rand()%COUNT;
            int spawnX = (side < 2) * (rand()%(screen_width+screen_width/2) - screen_width/2) + (side == RIGHT) * (screen_width+screen_width/2);
            int spawnY = (side > 1) * (rand()%(screen_height+screen_height/2) - screen_height/2) + (side == BOTTOM) * (screen_height+screen_height/2);
            createSkull(renderer, {spawnX,spawnY}, screen_width, screen_height, 2);
        }
    }
	// Spawning new fish
	next_fish_spawn -= elapsed_ms_since_last_update * current_speed;
	if (registry.softShells.components.size() <= MAX_FISH && next_fish_spawn < 0.f) {
		// !!!  TODO A1: Create new fish with createFish({0,0}), as for the Turtles above
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE SPAWN HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Processing the salmon state
	assert(registry.screenStates.components.size() <= 1);
    ScreenState &screen = registry.screenStates.components[0];

    float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities) {
		// progress timer
		DeathTimer& counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if(counter.counter_ms < min_counter_ms){
		    min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0) {
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
            restart_game();
			return true;
		}
	}
	// reduce window brightness if any of the present salmons is dying
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;

	// !!! TODO A1: update LightUp timers and remove if time drops below zero, similar to the death counter

	return true;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
    // Get the screen dimensions
    int screen_width;
    int screen_height;
    glfwGetWindowSize(window, &screen_width, &screen_height);
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;
    
    std::ifstream savedFile ("save.json", std::fstream::in);

    if (!refresh && savedFile.is_open()) {
        /* ok, proceed with output */
        savedFile >> j;
        load();
        renderer->tutorial = false;
    }else {
        // Remove all entities that we created
        // All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
        while (registry.motions.entities.size() > 0)
            registry.remove_all_components_of(registry.motions.entities.back());

        // Debugging for memory/component leaks
        registry.list_all_components();

        // Create a new castle
        player_castle = createCastle(renderer, { screen_width/2, screen_height/2 });
        float sy = (float)screen_width/2560.f;
        game_bar = createBar(renderer, { screen_width/2, 300*sy/2 }, {screen_width, 300*sy});
        if (renderer->tutorial)
            points = 1000;
        else
            points = ini_gold;
        level = ONE;
        
        ROUNDONE_DELAY_MS = 1000*61;
        ROUNDTWO_DELAY_MS = 1000*91;
        ROUNDTHREE_DELAT_MS = 1000*121;
        
        renderer->levelOne = true;
        renderer->levelTwo = false;
        renderer->levelThree = false;
    }
    renderer->is_over = false;
	//registry.colors.insert(player_salmon, {1, 0.8f, 0.8f});

	// !! TODO A3: Enable static pebbles on the ground
	// Create pebbles on the floor for reference
	/*
	for (uint i = 0; i < 20; i++) {
		int w, h;
		glfwGetWindowSize(window, &w, &h);
		float radius = 30 * (uniform_dist(rng) + 0.3f); // range 0.3 .. 1.3
		Entity pebble = createPebble({ uniform_dist(rng) * w, h - uniform_dist(rng) * 20 }, 
			         { radius, radius });
		float brightness = uniform_dist(rng) * 0.5 + 0.5;
		registry.colors.insert(pebble, { brightness, brightness, brightness});
	}
	*/
}

//void WorldSystem::handle_boundary_collisions() {
//    // Get the screen dimensions
//    int screen_width;
//    int screen_height;
//    glfwGetWindowSize(window, &screen_width, &screen_height);
//    auto& boudaryRegistry = registry.boundary;
//    for (uint i = 0; i < boudaryRegistry.components.size(); i++) {
//        // The entity
//        Entity entity = boudaryRegistry.entities[i];
//        Motion& motion = registry.motions.get(entity);
//        Motion& castle = registry.motions.get(player_castle);
//        int dir = (motion.position.x > (screen_width / 2)) ? -1 : 1;
//        //int dir = (motion.position.x > (1280/2)) ? -1 : 1;
//        motion.scale = vec2({ dir*SKULL_BB_WIDTH, SKULL_BB_HEIGHT });
//    }
//    registry.boundary.clear();
//}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the salmon
		if (registry.players.has(entity)) {
			//Player& player = registry.players.get(entity);

			// Checking Player - HardShell collisions
			if (registry.hardShells.has(entity_other)) {
                State& state = registry.states.get(entity_other);
                to_attack.process(entity_other);
                if (state.current_state == SkullState::Attacking && !renderer->is_over) {
                    STEALING_DELAY_MS -= ems;
                    if (STEALING_DELAY_MS < 0.f) {
                        points -= gold_stolen;
                        //points = max(0, points-gold_stolen);
                        STEALING_DELAY_MS = 1000;
                    }
                }
            }
		}
		
		if (registry.turrets.has(entity)) {
			// Checking skull - turret collisions
			if (registry.hardShells.has(entity_other)) {
				Turret& turret = registry.turrets.get(entity);
                Motion& motion = registry.motions.get(entity_other);
				State& state = registry.states.get(entity_other);
				//int old_id = (int)render_requester.used_texture;
            
                to_attack.process(entity_other);
            
				// enemy fights the turret until the turret is destroyed
                if (state.current_state == SkullState::Attacking && !renderer->is_over) {
                    ATTACKING_DELAY_MS -= ems;
                    if (ATTACKING_DELAY_MS < 0.f) {
                        turret.health -= registry.hardShells.get(entity_other).skull_damage;
                        ATTACKING_DELAY_MS = 1000;
                    }
                    if (turret.health <= 0) {
                        registry.remove_all_components_of(entity);
                        // change the enemy's texture and velocity back
                        motion.velocity.x = motion.pre_vel.x;
                        motion.velocity.y = motion.pre_vel.y;
                        state.current_state = SkullState::Walking;
                        HardShell& hs = registry.hardShells.get(entity_other);
                        int id;
                        if ((int)hs.type == 0) {
                            id = (int)TEXTURE_ASSET_ID::WALK_SKULL_00;
                        } else if ((int)hs.type == 1) {
                            id = (int)TEXTURE_ASSET_ID::WALK_SKULL2_00;
                        } else{
                            id = (int)TEXTURE_ASSET_ID::WALK_SKULL3_00;
                        }
                        RenderRequest& render_requester = registry.renderRequests.get(entity_other);
                        render_requester.used_texture = (TEXTURE_ASSET_ID)(id);
                    }
                }
			}
		}

		if (registry.walls.has(entity)) {
			// Checking skull - wall collisions
			if (registry.hardShells.has(entity_other)) {
				Wall& wall = registry.walls.get(entity);
				Motion& motion = registry.motions.get(entity_other);
                State& state = registry.states.get(entity_other);
				//int old_id = (int)render_requester.used_texture;
                to_attack.process(entity_other);
				// enemy fights the wall until the turret is destroyed
				wall.health -= 0.5;
				if (wall.health <= 0) {
					registry.remove_all_components_of(entity);
						// change the enemy's texture and velocity back
					motion.velocity.x = motion.pre_vel.x;
					motion.velocity.y = motion.pre_vel.y;
					state.current_state = SkullState::Walking;
                    HardShell& hs = registry.hardShells.get(entity_other);
                    int id;
                    if ((int)hs.type == 0) {
                        id = (int)TEXTURE_ASSET_ID::WALK_SKULL_00;
                    } else if ((int)hs.type == 1) {
                        id = (int)TEXTURE_ASSET_ID::WALK_SKULL2_00;
                    } else{
                        id = (int)TEXTURE_ASSET_ID::WALK_SKULL3_00;
                    }
					RenderRequest& render_requester = registry.renderRequests.get(entity_other);
					render_requester.used_texture = (TEXTURE_ASSET_ID)(id);
				}
			}
		}
        
        // Collisions that involve the bullet
        if (registry.softShells.has(entity)){
            // Checking Bullet - HardShell collisions
            if (registry.hardShells.has(entity_other)) {
                // 2. decrement hardshell health status by the bullet damage
                //float skullHealth = registry.hardShells.get(entity_other).skull_health;
                //float bulletDamage = registry.softShells.get(entity).bullet_damage;
                registry.hardShells.get(entity_other).skull_health -= registry.softShells.get(entity).bullet_damage;
                
                // add gold to player depending of enemy tpe
                BTState state = to_die.process(entity_other);
                
                registry.remove_all_components_of(entity);
                Mix_PlayChannel(-1, bullet_hit_sound, 0);
                if (state == BTState::Success) {
                    Mix_PlayChannel(-1, skull_dead_sound, 0);
                    
                    //give player points for killing skull.
                    SkullType st = registry.hardShells.get(entity_other).type;
                    if (st == SkullType::ONE) {
                        points += 1;
                    } else if (st == SkullType::TWO) {
                        points += 2;
                    } else if (st == SkullType::THREE) {
                        points += 3;
                    }
                    
                    
                    auto& bullet_registry = registry.softShells;
                    for(uint x = 0; x < bullet_registry.size(); x++) {
                        SoftShell& bullet = bullet_registry.components[x];
                        if (bullet.target == entity_other) {
                            registry.remove_all_components_of(bullet_registry.entities[x]);
                        }
                    }
                    auto& turret_registry = registry.turrets;
                    for(uint x = 0; x < turret_registry.size(); x++){
                        Turret& tower = turret_registry.get(turret_registry.entities[x]);
                        if ((unsigned int)(tower.cSkull) == (unsigned int)(entity_other)){
                            tower.attack = false;
                            tower.counter = 0.f;
                        }
                    }
                    for(uint i = 0; i< registry.light.size(); i++) {
                        Light& light = registry.light.components[i];
                        if (light.target == entity_other) {
                            registry.remove_all_components_of(registry.light.entities[i]);
                        }
                    }
                    
                }
            }
        }
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

void WorldSystem::save() {
    
    j.clear();
    
    j["gold"] = points;
    
    j["level"] = level;
    
    j["bar"] = (unsigned int)(registry.gb.entities[0]);
    
    j["time"] = renderer->timeRemaining;
    
    j["RT1"] = ROUNDONE_DELAY_MS;
    j["RT2"] = ROUNDTWO_DELAY_MS;
    j["RT3"] = ROUNDTHREE_DELAT_MS;
    
    ComponentContainer<Light> &light = registry.light;
    auto jla = json::array();
    for (int i = 0; i < light.size(); i++) {
        Light& l = light.components[i];
        json jl;
        jl["id"] = (unsigned int)(light.entities[i]);
        l.to_json(jl, l);
        jla.push_back(jl);
    }
    j["light"] = jla;
    
    ComponentContainer<MenuButton> &buttons = registry.buttons;
    auto jbs = json::array();
    for (int i = 0; i < buttons.size(); i++) {
        MenuButton& b = buttons.components[i];
        json jb;
        jb["id"] = (unsigned int)(buttons.entities[i]);
        b.to_json(jb, b);
        jbs.push_back(jb);
    }
    j["buttons"] = jbs;
    
    ComponentContainer<State> &states = registry.states;
    auto jss = json::array();
    for (int i = 0; i < states.size(); i++) {
        State& s = states.components[i];
        json js;
        js["id"] = (unsigned int)(states.entities[i]);
        s.to_json(js, s);
        jss.push_back(js);
    }
    j["states"] = jss;
    
    ComponentContainer<AIDetection> &ai = registry.aiDetected;
    auto jai = json::array();
    for (int i = 0; i < ai.size(); i++) {
        json ja;
        ja["id"] = (unsigned int)(ai.entities[i]);
        jai.push_back(ja);
    }
    j["ai"] = jai;
    
    ComponentContainer<BoundaryCollision> &bc = registry.boundary;
    auto jbcs = json::array();
    for (int i = 0; i < bc.size(); i++) {
        json jbc;
        jbc["id"] = (unsigned int)(bc.entities[i]);
        jbcs.push_back(jbc);
    }
    j["boundary"] = jbcs;
    
    ComponentContainer<vec3> &colors = registry.colors;
    auto jcs = json::array();
    for (int i = 0; i < colors.size(); i++) {
        vec3& c = colors.components[i];
        json jc;
        jc["id"] = (unsigned int)(colors.entities[i]);
        jc["x"] = c.x;
        jc["y"] = c.y;
        jc["z"] = c.z;
        jcs.push_back(jc);
    }
    j["colors"] = jcs;
    
    ComponentContainer<DebugComponent> &dc = registry.debugComponents;
    auto jdcs = json::array();
    for (int i = 0; i < dc.size(); i++) {
        json jdc;
        jdc["id"] = (unsigned int)(dc.entities[i]);
        jdcs.push_back(jdc);
    }
    j["debug"] = jdcs;
    
    ComponentContainer<HardShell> &hardshell = registry.hardShells;
    auto jhss = json::array();
    for (int i = 0; i < hardshell.size(); i++) {
        HardShell& hs = hardshell.components[i];
        json jhs;
        jhs["id"] = (unsigned int)(hardshell.entities[i]);
        hs.to_json(jhs, hs);
        jhss.push_back(jhs);
    }
    j["hardshell"] = jhss;
    
    ComponentContainer<SoftShell> &softshell = registry.softShells;
    auto jsoft = json::array();
    for (int i = 0; i < softshell.size(); i++) {
        SoftShell& softs = softshell.components[i];
        json js;
        js["id"] = (unsigned int)(softshell.entities[i]);
        softs.to_json(js, softs);
        jsoft.push_back(js);
    }
    j["softshell"] = jsoft;
    
    ComponentContainer<ScreenState> &screen = registry.screenStates;
    auto jsc = json::array();
    for (int i = 0; i < screen.size(); i++) {
        ScreenState& sc = screen.components[i];
        json js;
        js["id"] = (unsigned int)(screen.entities[i]);
        sc.to_json(js, sc);
        jsc.push_back(js);
    }
    j["screenstate"] = jsc;
    
    ComponentContainer<RenderRequest> &render = registry.renderRequests;
    auto jrr = json::array();
    for (int i = 0; i < render.size(); i++) {
        RenderRequest& r = render.components[i];
        json jr;
        jr["id"] = (unsigned int)(render.entities[i]);
        r.to_json(jr, r);
        jrr.push_back(jr);
    }
    j["render_request"] = jrr;
    
    ComponentContainer<Player> &players = registry.players;
    auto jps = json::array();
    for (int i = 0; i < players.size(); i++) {
        Player& p = players.components[i];
        json jp;
        jp["id"] = (unsigned int)(players.entities[i]);
        p.to_json(jp, p);
        jps.push_back(jp);
    }
    j["players"] = jps;
    
    ComponentContainer<Collision> &collisions = registry.collisions;
    auto jcos = json::array();
    for (int i = 0; i < collisions.size(); i++) {
        Collision& c = collisions.components[i];
        json jc;
        jc["id"] = (unsigned int)(collisions.entities[i]);
        c.to_json(jc, c);
        jcos.push_back(jc);
    }
    j["collisions"] = jcos;
    
    ComponentContainer<Motion> &motions = registry.motions;
    auto jms = json::array();
    for (int i = 0; i < motions.size(); i++) {
        Motion& m = motions.components[i];
        json jm;
        jm["id"] = (unsigned int)(motions.entities[i]);
        m.to_json(jm, m);
        jms.push_back(jm);
    }
    j["motions"] = jms;
    
    ComponentContainer<Wall> &walls = registry.walls;
    auto jws = json::array();
    for (int i = 0; i < walls.size(); i++) {
        Wall& w = walls.components[i];
        json jw;
        jw["id"] = (unsigned int)(walls.entities[i]);
        w.to_json(jw, w);
        jws.push_back(jw);
    }
    j["walls"] = jws;
    
    ComponentContainer<Turret> &turrets = registry.turrets;
    auto jts = json::array();
    for (int i = 0; i < turrets.size(); i++) {
        Turret& t = turrets.components[i];
        json jt;
        jt["id"] = (unsigned int)(turrets.entities[i]);
        t.to_json(jt, t);
        jts.push_back(jt);
    }
    j["turrets"] = jts;
    
    ComponentContainer<DeathTimer> &deathTimers = registry.deathTimers;
    auto jdts = json::array();
    for (int i = 0; i < deathTimers.size(); i++) {
        DeathTimer& t = deathTimers.components[i];
        json jt;
        jt["id"] = (unsigned int)(deathTimers.entities[i]);
        t.to_json(jt, t);
        jdts.push_back(jt);
    }
    j["deathtimer"] = jdts;
    
    std::string s = j.dump();
    
    std::ofstream file;
    file.open("save.json",std::fstream::out);

    file << j;
    
}

void WorldSystem::load() {
    
    registry.clear_all_components();
    
    j.at("gold").get_to(points);
    renderer->gold = points;
    
    j.at("level").get_to(level);
    
    if (level == ONE) {
        renderer->levelOne = true;
        renderer->levelTwo = false;
        renderer->levelThree = false;
    } else if (level == TWO) {
        renderer->levelOne = false;
        renderer->levelTwo = true;
        renderer->levelThree = false;
    } else {
        renderer->levelOne = false;
        renderer->levelTwo = false;
        renderer->levelThree = true;
    }
    
    Entity e = Entity(j["bar"]);
    GameBar b;
    registry.gb.insert(e, b);
    
    renderer->timeRemaining = j["time"];
    ROUNDONE_DELAY_MS = j["RT1"];
    ROUNDTWO_DELAY_MS = j["RT2"];
    ROUNDTHREE_DELAT_MS = j["RT3"];
    
    for (auto& element : j["light"])
    {
        Light l;
        l.from_json(element, l);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.light.insert(e, l);
        
    }

    for (auto& element : j["buttons"])
    {
        MenuButton b;
        b.from_json(element, b);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.buttons.insert(e, b);
        
    }
    
    for (auto& element : j["states"])
    {
        State s;
        s.from_json(element, s);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.states.insert(e, s);
        
    }
    
    for (auto& element : j["ai"])
    {
        AIDetection a;
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.aiDetected.insert(e, a);
        
    }
    
    for (auto& element : j["boundary"])
    {
        BoundaryCollision b;
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.boundary.insert(e, b);
        
    }
    
    for (auto& element : j["colors"])
    {
        vec3 c;
        element.at("x").get_to(c.x);
        element.at("y").get_to(c.y);
        element.at("z").get_to(c.z);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.colors.insert(e, c);
        
    }
    
    for (auto& element : j["debug"])
    {
        DebugComponent d;
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.debugComponents.insert(e, d);
        
    }
    
    for (auto& element : j["hardshell"])
    {
        HardShell hs;
        hs.from_json(element, hs);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.hardShells.insert(e, hs);
        
    }
    
    for (auto& element : j["softshell"])
    {
        SoftShell ss;
        ss.from_json(element, ss);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.softShells.insert(e, ss);
        
    }
    
    for (auto& element : j["screenstate"])
    {
        ScreenState ss;
        ss.from_json(element, ss);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.screenStates.insert(e, ss);
        
    }
    
    for (auto& element : j["render_request"])
    {
        RenderRequest rr;
        rr.from_json(element, rr);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.renderRequests.insert(e, rr);
        
    }
    
    for (auto& element : j["players"])
    {
        Player p;
        p.from_json(element, p);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.players.insert(e, p);
        player_castle = e;
        
    }
    
    for (auto& element : j["collisions"])
    {
        Collision c;
        c.from_json(element, c);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.collisions.insert(e, c);
        
    }
    
    for (auto& element : j["motions"])
    {
        Motion m;
        m.from_json(element, m);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.motions.insert(e, m);
        
    }
    
    for (auto& element : j["walls"])
    {
        Wall w;
        w.from_json(element, w);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.walls.insert(e, w);
        
    }
    
    for (auto& element : j["turrets"])
    {
        Turret t;
        t.from_json(element, t);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.turrets.insert(e, t);
        
    }
    
    for (auto& element : j["deathtimer"])
    {
        DeathTimer t;
        t.from_json(element, t);
        unsigned int id = element["id"];
        Entity e = Entity(id);
        registry.deathTimers.insert(e, t);
        
    }
    
    std::string s = j.dump();
    
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

bool WorldSystem::is_play() const {
    return bool(current_state == GameState::Play);
}

void WorldSystem::setAiRef(AISystem* ai) {
    reference = ai;
}

// handle camera movement
bool moveDown = false;
bool moveUp = false;
bool moveLeft = false;
bool moveRight = false;

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (is_play()){
        if (!renderer->is_over) {
		mat3 currentLocation = renderer->translationMatrix;

		if (moveUp == true && currentLocation[1].y > -0.975) {
			renderer->translationMatrix[1].y = currentLocation[1].y - 0.025;
		}
		if (moveDown == true && currentLocation[1].y < 0.975) {
			renderer->translationMatrix[1].y = currentLocation[1].y + 0.025;
		}
		if (moveRight == true && currentLocation[0].x > -0.975) {
			renderer->translationMatrix[0].x = currentLocation[0].x - 0.025;
		}
		if (moveLeft == true && currentLocation[0].x < 0.975) {
			renderer->translationMatrix[0].x = currentLocation[0].x + 0.025;
		}
        
        if (action == GLFW_PRESS){
            if (key == GLFW_KEY_UP) {
                moveUp = true;
            }
            if (key == GLFW_KEY_DOWN) {
                moveDown = true;
            }
            if (key== GLFW_KEY_LEFT) {
                moveLeft = true;
            }
            if (key == GLFW_KEY_RIGHT) {
                moveRight = true;
            }
        }
    
        int screen_width, screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);

        if (action == GLFW_RELEASE){
            if (key == GLFW_KEY_UP) {
                moveUp= false;
            }
            if (key == GLFW_KEY_DOWN) {
                moveDown= false;
            }
            if (key== GLFW_KEY_LEFT) {
                moveLeft = false;
            }
            if (key == GLFW_KEY_RIGHT) {
                moveRight= false;
            }
            if (!renderer->story && !renderer->help) {
                if (key == GLFW_KEY_1 && points >= turret_cost) {
                    Mix_PlayChannel(-1, build_turret_sound, 0);
                    points -= turret_cost;
                    vec2 mouse_pos = this->mouse_pos;
                    vec2 turretPos = {mouse_pos.x + (-screen_width/2)*currentLocation[0].x, mouse_pos.y + (screen_height/2)*currentLocation[1].y };
                    createTurret(turretPos, renderer, 150, TurretType::ONE);
                    reference->flagUpdate();
                }
                if (key == GLFW_KEY_2 && points >= turret2_cost) {
                    Mix_PlayChannel(-1, build_turret_sound, 0);
                    points -= turret2_cost;
                    vec2 mouse_pos = this->mouse_pos;
                    vec2 turretPos = {mouse_pos.x + (-screen_width/2)*currentLocation[0].x, mouse_pos.y + (screen_height/2)*currentLocation[1].y };
                    createTurret(turretPos, renderer, 300, TurretType::TWO);
                    reference->flagUpdate();
                }
                if (key == GLFW_KEY_0 && points >= wall_cost) {
                    Mix_PlayChannel(-1, build_turret_sound, 0);
                    points -= wall_cost;
                    vec2 mouse_pos = this->mouse_pos;
                    vec2 turretPos = {mouse_pos.x + (-screen_width/2)*currentLocation[0].x, mouse_pos.y + (screen_height/2)*currentLocation[1].y };
                    createWall(turretPos, renderer, 400);
                    reference->flagUpdate();
                }
            }
        }
    
        if (action == GLFW_RELEASE && key == GLFW_KEY_P) {
            for (int i = (int)registry.states.components.size()-1; i>=0; --i) {
                Entity e = registry.states.entities[i];
                RenderRequest& rr = registry.renderRequests.get(e);
                if (registry.states.components[i].current_state != SkullState::Idle &&
                    registry.states.components[i].current_state != SkullState::Dying) {
                    
                    registry.states.components[i].pre_state = registry.states.components[i].current_state;
                    registry.states.components[i].current_state = SkullState::Idle;
                    rr.used_texture = TEXTURE_ASSET_ID::IDLE_SKULL_00;
                    if (registry.states.components[i].pre_state != SkullState::Attacking){
                        registry.motions.get(e).pre_vel = registry.motions.get(e).velocity;
                        registry.motions.get(e).velocity = {0,0};
                    }
                    
                }
            }
            idle  = true;
        }
        
            if (action == GLFW_RELEASE && key == GLFW_KEY_K) {
                renderer->story = true;
                renderer->change_txt = 1;
            }

                if (renderer->story) {
                    if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
                        renderer->move_text(1);
                    }
                }

                if (action == GLFW_RELEASE && key == GLFW_KEY_H) {
                    renderer->help = true;
                }

                if (action == GLFW_RELEASE && key == GLFW_KEY_C && (idle || renderer->story || renderer->help)) {
                    renderer->story = false;
                    renderer->help = false;
                    if (idle) {
                        for (int i = (int)registry.states.components.size()-1; i>=0; --i) {
                            Entity e = registry.states.entities[i];
                            RenderRequest& rr = registry.renderRequests.get(e);
                            if (registry.states.components[i].current_state == SkullState::Idle) {
                                registry.states.components[i].current_state = registry.states.components[i].pre_state;
                                registry.states.components[i].pre_state = SkullState::Idle;
                                if (registry.states.components[i].current_state == SkullState::Attacking){
                                    rr.used_texture = TEXTURE_ASSET_ID::ATTACK_SKULL_00;
                                }else {
                                    rr.used_texture = TEXTURE_ASSET_ID::WALK_SKULL_00;
                                    Motion& motion = registry.motions.get(e);
                                    motion.velocity = motion.pre_vel;
                                }
                            }
                        }
                        idle  = false;
                    }
                }
            
            if (!renderer->tutorial) {
                if (action == GLFW_RELEASE && key == GLFW_KEY_T) {
                    save();
                    renderer->restart = false;
                    renderer->tutorial = true;
                    renderer->tutorial_pause = true;
                    renderer->change_txt_tut = 1;
                    refresh = true;
                    restart_game();
                    refresh = false;
                }
            }
            
            if (renderer->tutorial) {
                if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
                    renderer->move_text_tut(1);
                    if (renderer->restart) {
                        renderer->restart = false;
                        renderer->change_txt_tut = 0;
                        renderer->tutorial = false;
                        renderer->tutorial_pause = false;
                        restart_game();
                    }
                }
            }
            
            if (action == GLFW_RELEASE && key == GLFW_KEY_S) {
                saved = true;
                save();
            }
                
            if (action == GLFW_RELEASE && key == GLFW_KEY_L && saved) {
                load();
            }
        }
    
        if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
            if (!renderer->is_over && !renderer->tutorial) {
                save();
            }
            glfwSetWindowShouldClose(window, 1);
        }

        // Resetting game
        if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
            refresh = true;
            renderer->restart = false;
            renderer->change_txt_tut = 0;
            renderer->tutorial = false;
            renderer->tutorial_pause = false;
            restart_game();
            refresh = false;
        }
    
//    if (action == GLFW_RELEASE && key == GLFW_KEY_B) {
//        current_state = GameState::Play;
//        renderer->is_play = true;
//    }

        // Debugging
        if (key == GLFW_KEY_D) {
            if (action == GLFW_RELEASE)
                debugging.in_debug_mode = false;
            else
                debugging.in_debug_mode = true;
        }

        // Control the current speed with `<` `>`
        if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
            current_speed -= 0.1f;
            printf("Current speed = %f\n", current_speed);
        }
        if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
            current_speed += 0.1f;
            printf("Current speed = %f\n", current_speed);
        }
        current_speed = fmax(0.f, current_speed);
    }
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A1: HANDLE SALMON ROTATION HERE
	// xpos and ypos are relative to the top-left of the window, the salmon's
	// default facing direction is (1, 0)
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	this->mouse_pos = mouse_position;
    if (!is_play()) {
        MenuButton& b1 = registry.buttons.get(registry.buttons.entities[1]);
        MenuButton& b2 = registry.buttons.get(registry.buttons.entities[2]);
        b1.mouse_pos = mouse_position;
        b2.mouse_pos = mouse_position;
        (vec2)mouse_position; // dummy to avoid compiler warning
    }
}

void WorldSystem::on_mouse(int button, int action, int mod) {
	if (!is_play()) {
		Motion& motion1 = registry.motions.get(registry.buttons.entities[1]);
		Motion& motion2 = registry.motions.get(registry.buttons.entities[2]);
		MenuButton& b1 = registry.buttons.get(registry.buttons.entities[1]);
		MenuButton& b2 = registry.buttons.get(registry.buttons.entities[2]);
		if (b1.mouse_pos.x >= motion1.position.x - abs(motion1.scale.x) / 2 &&
			b1.mouse_pos.x <= motion1.position.x + abs(motion1.scale.x) / 2 &&
			b1.mouse_pos.y >= motion1.position.y - abs(motion1.scale.y) / 2 &&
			b1.mouse_pos.y <= motion1.position.y + abs(motion1.scale.y) / 2) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				current_state = GameState::Play;
				renderer->is_play = true;
				registry.remove_all_components_of(registry.buttons.entities[0]);
				registry.remove_all_components_of(registry.buttons.entities[1]);
				//registry.remove_all_components_of(registry.buttons.entities[2]);
				//for (i = 0; )
			}
		}
		else if (b2.mouse_pos.x >= motion2.position.x - abs(motion2.scale.x) / 2 &&
			b2.mouse_pos.x <= motion2.position.x + abs(motion2.scale.x) / 2 &&
			b2.mouse_pos.y >= motion2.position.y - abs(motion2.scale.y) / 2 &&
			b2.mouse_pos.y <= motion2.position.y + abs(motion2.scale.y) / 2) {
			if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				glfwSetWindowShouldClose(window, 1);
			}
		}
    }
}
