#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

#include "render_system.hpp"
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem
{
public:
    float screen_width;
    float screen_height;
    int base_heuristic[21][21];
    int heuristic_matrix[21][21];
    
    RenderSystem* renderer;
    float next_fish_spawn = 0.0f;
    // starts the game
    void init(RenderSystem* renderer);
    
    //new ai functions
    void SetDimensions(float width, float height);
    void flagUpdate();
    void updateNextPos(Entity e);
    bool compareHeuristic(vec2 lowest_pos, vec2 check);
    void updateHeuristic();
    vec2 getPosFromHeuristicIdx(vec2 idx);
    vec2 getHeuristicIdxFromPos(vec2 pos);
    float relativeEuclidianDist(float row, float col);
    void resetHeuristic();
    
	void step(float elapsed_ms);
    
    // Check for collisions
    void handle_collisions();
    
    // C++ random number generator
    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
