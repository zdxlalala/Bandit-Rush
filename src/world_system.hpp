#pragma once

// internal
#include "common.hpp"
#include "json.hpp"
using json = nlohmann::json;

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "btree.cpp"
#include "ai_system.hpp"
// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
enum class GameState {
        Menu,
        Play
};
class WorldSystem
{
public:
    GameState current_state;
    AISystem* reference;
    void setAiRef(AISystem* ai);
    
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window(int width, int height);

	// starts the game
	void init(RenderSystem* renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

    // Check for boundary collisions
    //void handle_boundary_collisions();
    
	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;
    
    bool is_play()const;
    
    // restart level
    void restart_game();
    
    void save();
    void load();
    
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
    void on_mouse(int button, int action, int mod);

    std::function<bool(Entity)> is_walking = [](Entity e) {return registry.states.get(e).current_state == SkullState::Walking; };
    std::function<bool(Entity)> is_attacking = [](Entity e) {return registry.states.get(e).current_state == SkullState::Attacking; };
    std::function<bool(Entity)> is_idling = [](Entity e) {return registry.states.get(e).current_state == SkullState::Idle; };
    std::function<bool(Entity)> is_dying = [](Entity e) {return registry.states.get(e).current_state == SkullState::Dying; };
    std::function<bool(Entity)> is_dead = [](Entity e) {return registry.hardShells.get(e).skull_health <= 0; };
    BTIfCondition to_attack;
    BTIfCondition to_die;
    BTIfCondition in_walk;
    BTIfCondition in_attack;
    BTIfCondition in_idle;
    BTIfCondition in_dying;
    BTAnimation ba;
    ToAttack t;
    ToDie d;
    InWalk w;
    InAttack a;
    InIdle i;
    Dying di;
    
    
    
    //JSON
    json j;

	// OpenGL window handle
	GLFWwindow* window;

	// Number of fish eaten by the salmon, displayed in the w1280indow title
    int points;
    

	// Game state
	RenderSystem* renderer;
	float current_speed;
	float next_skull_spawn;
    float next_skull2_spawn;
    float next_skull3_spawn;
	float next_fish_spawn;
    vec2 mouse_pos;
	Entity player_castle;
    Entity game_bar;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
    Mix_Chunk* skull_dead_sound;
    Mix_Chunk* bullet_hit_sound;
    Mix_Chunk* build_turret_sound;
    Mix_Chunk* game_over_sound;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
