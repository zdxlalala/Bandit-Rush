#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
    int screen_height;
    int screen_width;
public:
    std::vector<std::function<void ()>> callbacks;

    void attach(std::function<void ()> fn);
    void setState();
    
	void step(float elapsed_ms, float window_width_px, float window_height_px);
    
    bool boundary_collides(const Motion& motion1);

	PhysicsSystem(int h, int w)
	{
        screen_height = h;
        screen_width = w;
	}
};
