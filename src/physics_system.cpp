// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"


// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	float other_r_squared = dot(other_bonding_box, other_bonding_box);

	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
    const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

//bool PhysicsSystem::boundary_collides(const Motion& motion1)
//{
//    return (motion1.position.x >= (screen_width+screen_width/2) || motion1.position.x <= (-screen_width/2) ||
//            motion1.position.y >= (screen_height+screen_height/2) || motion1.position.y <= (-screen_height/2));
//}

void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
        // (enemy) go x direction then y direction
        if (registry.hardShells.has(entity)) {
            HardShell& hs = registry.hardShells.get(entity);
            if (hs.type == SkullType::ONE) {
                if (motion.velocity.x != 0 || motion.velocity.x != 0) {
                    float angle = atan2((window_height_px/2) - motion.position.y, (window_width_px/2) - motion.position.x);
                    motion.position.x += 25*cos(angle)*step_seconds;
                    motion.position.y += 25*sin(angle)*step_seconds;
                }
            }
            else {
                motion.position += motion.velocity*step_seconds;
            }
            for(uint i = 0; i< registry.light.size(); i++) {
                Light& light = registry.light.components[i];
                if (light.target == entity) {
                    Entity le = registry.light.entities[i];
                    motion_registry.get(le).position = motion.position;
                }
            }
        }
        else
            motion.position += motion.velocity*step_seconds;
        (void)elapsed_ms; // placeholder to silence unused warning until implemented
    }

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
//        if (boundary_collides(motion_i)) {
//            registry.boundary.emplace(entity_i);
//        }
		for(uint j = 0; j<motion_container.components.size(); j++) // i+1
		{
			if (i == j)
				continue;

			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}
    
    
    //call the callback function to handle collision
    setState();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE SALMON - WALL collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on Salmon mesh collision
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)motion_container.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];
            if (!registry.light.has(entity_i) && !registry.gb.has(entity_i)) {

    //			// visualize the radius with two axis-aligned lines
    //			const vec2 bonding_box = get_bounding_box(motion_i);
    //			float radius = sqrt(dot(bonding_box/2.f, bonding_box/2.f));
    //			vec2 line_scale1 = { motion_i.scale.x / 10, 2*radius };
    //			Entity line1 = createLine(motion_i.position, line_scale1);
    //			vec2 line_scale2 = { 2*radius, motion_i.scale.x / 10};
    //			Entity line2 = createLine(motion_i.position, line_scale2);
                
                // !!! TODO A2: implement debugging of bounding boxes and mesh
                // visualize the radius with two axis-aligned lines
                const vec2 bonding_box = get_bounding_box(motion_i);
                //float radius = sqrt(dot(bonding_box/2.f, bonding_box/2.f));
                vec2 line_scale1 = { motion_i.scale.x / 20, bonding_box.y };
                createLine({motion_i.position.x-(cos(motion_i.angle)*(bonding_box.x/2)),motion_i.position.y-(sin(motion_i.angle)*(bonding_box.x/2))}, line_scale1, motion_i.angle);
                createLine({motion_i.position.x+(cos(motion_i.angle)*bonding_box.x/2),motion_i.position.y+(sin(motion_i.angle)*(bonding_box.x/2))}, line_scale1, motion_i.angle);
                vec2 line_scale2 = { bonding_box.x, motion_i.scale.x / 20};
                createLine({motion_i.position.x-(sin(motion_i.angle)*(bonding_box.y/2)),motion_i.position.y-(cos(motion_i.angle)*bonding_box.y/2)}, line_scale2, motion_i.angle);
                createLine({motion_i.position.x+(sin(motion_i.angle)*(bonding_box.y/2)),motion_i.position.y+(cos(motion_i.angle)*bonding_box.y/2)}, line_scale2, motion_i.angle);
            }
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}


void PhysicsSystem::attach(std::function<void ()> fn) {
    callbacks.push_back(fn);
}

void PhysicsSystem::setState() {
    for (std::function<void ()> fn : callbacks) {
        fn();
    }
}
