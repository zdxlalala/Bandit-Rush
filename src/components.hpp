#pragma once
#include "common.hpp"
#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"

#include "json.hpp"
using json = nlohmann::json;


enum class SkullType{
    ONE = 0,
    TWO = ONE + 1,
    THREE = TWO + 1,
    COUNT = THREE + 1
};

enum class TurretType{
    ONE = 0,
    TWO = ONE + 1,
    COUNT = TWO + 1
};

struct Light
{
    Entity target;
    void to_json(json& j, Light& l) {
        j["target"] = int(l.target);
    }
    void from_json(const json& j, Light& l) {
        int id;
        j.at("target").get_to(id);
        l.target = Entity(id);
    }
};

struct GameBar
{
    
};

// Player component
struct Player
{
    //1. set initial health status for the castle
    float cas_health = 900;
    
    void to_json(json& j, const Player& p) {
        j["health"] = p.cas_health;
    }
    void from_json(const json& j, Player& p) {
        j.at("health").get_to(p.cas_health);
    }

};

// Turtles and pebbles have a hard shell
// Skull
struct HardShell
{
    //2. set initial health status for the skull
    float skull_health;
    float skull_damage;
    int prev_id;
    vec2 prev_velocity;
    SkullType type;

    void to_json(json& j, const HardShell& hs) {
        j["health"] = hs.skull_health;
        j["damage"] = hs.skull_damage;
        j["prev_id"] = hs.prev_id;
        j["prev_v_x"] = hs.prev_velocity.x;
        j["prev_v_y"] = hs.prev_velocity.y;
        j["type"] = hs.type;
    }
    void from_json(const json& j, HardShell& hs) {
        j.at("health").get_to(hs.skull_health);
        j.at("damage").get_to(hs.skull_damage);
        j.at("prev_id").get_to(hs.prev_id);
        j.at("prev_v_x").get_to(hs.prev_velocity.x);
        j.at("prev_v_y").get_to(hs.prev_velocity.y);
        j.at("type").get_to(hs.type);
    }
};

// Fish and Salmon have a soft shell
// Bullet
struct SoftShell
{
    int bullet_damage;
    Entity target;
    
    void to_json(json& j, SoftShell& ss) {
        j["damage"] = ss.bullet_damage;
        j["target"] = (unsigned int)(ss.target);
    }
    void from_json(const json& j, SoftShell& ss) {
        j.at("damage").get_to(ss.bullet_damage);
        unsigned int id;
        j.at("target").get_to(id);
        ss.target = Entity(id);
    }

};

struct MenuButton
{
    vec2 mouse_pos;
    
    void to_json(json& j, const MenuButton& b) {
        j["mouse_pos_x"] = b.mouse_pos.x;
        j["mouse_pos_y"] = b.mouse_pos.y;
    }
    void from_json(const json& j, MenuButton& b) {
        float x;
        j.at("mouse_pos_x").get_to(x);
        b.mouse_pos.x = x;
        j.at("mouse_pos_y").get_to(b.mouse_pos.y);
    }
};

// Skull states
enum class SkullState {
    Walking,
    Attacking,
    Idle,
    Dying
};

struct State {
    SkullState current_state;
    SkullState pre_state;
    void to_json(json& j, const State& s) {
        j["current_state"] = s.current_state;
        j["pre_state"] = s.pre_state;
    }
    void from_json(const json& j, State& s) {
        j.at("current_state").get_to(s.current_state);
        j.at("pre_state").get_to(s.pre_state);
    }
};
// All data relevant to the shape and motion of entities
struct Motion {
    int pastNodes[21][21];
    vec2 destNode = {-1, -1};
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
    //int path;
    vec2 pre_vel;
    
    void to_json(json& j, const Motion& m) {
        j["destN_x"] = m.destNode.x;
        j["destN_y"] = m.destNode.y;
        j["pos_x"] = m.position.x;
        j["pos_y"] = m.position.y;
        j["angle"] = m.angle;
        j["vel_x"] = m.velocity.x;
        j["vel_y"] = m.velocity.y;
        j["scale_x"] = m.scale.x;
        j["scale_y"] = m.scale.y;
        j["prev_x"] = m.pre_vel.x;
        j["prev_y"] = m.pre_vel.y;
    }
    void from_json(const json& j, Motion& m) {
        j.at("destN_x").get_to(m.destNode.x);
        j.at("destN_y").get_to(m.destNode.y);
        j.at("pos_x").get_to(m.position.x);
        j.at("pos_y").get_to(m.position.y);
        j.at("angle").get_to(m.angle);
        j.at("vel_x").get_to(m.velocity.x);
        j.at("vel_y").get_to(m.velocity.y);
        j.at("scale_x").get_to(m.scale.x);
        j.at("scale_y").get_to(m.scale.y);
        j.at("prev_x").get_to(m.pre_vel.x);
        j.at("prev_y").get_to(m.pre_vel.y);
    }
    
};

struct Wall
{
    vec2 position;
    float health;
    
    void to_json(json& j, const Wall& w) {
        j["pos_x"] = w.position.x;
        j["pos_y"] = w.position.y;
        j["health"] = w.health;
    }
    void from_json(const json& j, Wall& w) {
        j.at("pos_x").get_to(w.position.x);
        j.at("pos_y").get_to(w.position.y);
        j.at("health").get_to(w.health);
    }
};

struct Turret
{
    vec2 position;
    float health;
    bool attack;
    Entity cSkull;
    TurretType tt;
    float counter;
    
    void to_json(json& j, Turret& t) {
        j["pos_x"] = t.position.x;
        j["pos_y"] = t.position.y;
        j["health"] = t.health;
        j["attack"] = t.attack;
        j["cSkull"] = (unsigned int)(t.cSkull);
        j["type"] = t.tt;
        j["counter"] = t.counter;
    }
    void from_json(const json& j, Turret& t) {
        j.at("pos_x").get_to(t.position.x);
        j.at("pos_y").get_to(t.position.y);
        j.at("health").get_to(t.health);
        j.at("attack").get_to(t.attack);
        unsigned int id;
        j.at("cSkull").get_to(id);
        t.cSkull = Entity(id);
        j.at("type").get_to(t.tt);
        j.at("counter").get_to(t.counter);
    }
};


// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
    Collision() {};
    
    void to_json(json& j, Collision& c) {
        j["other"] = (unsigned int)(c.other);
    }
    void from_json(const json& j, Collision& c) {
        unsigned int id;
        j.at("other").get_to(id);
        c.other = Entity(id);
    }
};

// For boundary collides
struct BoundaryCollision
{

};

// For AI detection
struct AIDetection {
    
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
    
    void to_json(json& j, const Debug& d) {
        j["in_d"] = d.in_debug_mode;
        j["in_f"] = d.in_freeze_mode;
    }
    void from_json(const json& j, Debug& d) {
        j.at("in_d").get_to(d.in_debug_mode);
        j.at("in_f").get_to(d.in_freeze_mode);
    }
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
    
    void to_json(json& j, const ScreenState& s) {
        j["darken"] = s.darken_screen_factor;
    }
    void from_json(const json& j, ScreenState& s) {
        j.at("darken").get_to(s.darken_screen_factor);
    }
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer
{
	float counter_ms = 3000;
    
    void to_json(json& j, const DeathTimer& t) {
        j["counter"] = t.counter_ms;
    }
    void from_json(const json& j, DeathTimer& t) {
        j.at("counter").get_to(t.counter_ms);
    }
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
    
    void to_json(json& j, const ColoredVertex& cv) {
        j["pos_x"] = cv.position.x;
        j["pos_y"] = cv.position.y;
        j["pos_z"] = cv.position.z;
        j["c_x"] = cv.color.x;
        j["c_y"] = cv.color.y;
        j["c_z"] = cv.color.z;
    }
    void from_json(const json& j, ColoredVertex& cv) {
        j.at("pos_x").get_to(cv.position.x);
        j.at("pos_y").get_to(cv.position.y);
        j.at("pos_z").get_to(cv.position.z);
        j.at("c_x").get_to(cv.color.x);
        j.at("c_y").get_to(cv.color.y);
        j.at("c_z").get_to(cv.color.z);
    }
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
    
    void to_json(json& j, const TexturedVertex& tv) {
        j["pos_x"] = tv.position.x;
        j["pos_y"] = tv.position.y;
        j["pos_z"] = tv.position.z;
        j["t_x"] = tv.texcoord.x;
        j["t_y"] = tv.texcoord.y;
    }
    void from_json(const json& j, TexturedVertex& tv) {
        j.at("pos_x").get_to(tv.position.x);
        j.at("pos_y").get_to(tv.position.y);
        j.at("pos_z").get_to(tv.position.z);
        j.at("t_x").get_to(tv.texcoord.x);
        j.at("t_y").get_to(tv.texcoord.y);
    }
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

enum class TEXTURE_ASSET_ID {
	FISH = 0,
    WALK_SKULL_00 = FISH + 1,
    WALK_SKULL_01 = WALK_SKULL_00 + 1,
    WALK_SKULL_02 = WALK_SKULL_01 + 1,
    WALK_SKULL_03 = WALK_SKULL_02 + 1,
    WALK_SKULL_04 = WALK_SKULL_03 + 1,
    WALK_SKULL_05 = WALK_SKULL_04 + 1,
    WALK_SKULL_06 = WALK_SKULL_05 + 1,
    WALK_SKULL_07 = WALK_SKULL_06 + 1,
    WALK_SKULL_08 = WALK_SKULL_07 + 1,
    WALK_SKULL_09 = WALK_SKULL_08 + 1,
    WALK_SKULL_10 = WALK_SKULL_09 + 1,
    WALK_SKULL_11 = WALK_SKULL_10 + 1,
    WALK_SKULL_12 = WALK_SKULL_11 + 1,
    WALK_SKULL_13 = WALK_SKULL_12 + 1,
    WALK_SKULL_14 = WALK_SKULL_13 + 1,
    WALK_SKULL_15 = WALK_SKULL_14 + 1,
    WALK_SKULL_16 = WALK_SKULL_15 + 1,
    WALK_SKULL_17 = WALK_SKULL_16 + 1,
    ATTACK_SKULL_00 = WALK_SKULL_17 + 1,
    ATTACK_SKULL_01 = ATTACK_SKULL_00 + 1,
    ATTACK_SKULL_02 = ATTACK_SKULL_01 + 1,
    ATTACK_SKULL_O3 = ATTACK_SKULL_02 + 1,
    ATTACK_SKULL_04 = ATTACK_SKULL_O3 + 1,
    ATTACK_SKULL_05 = ATTACK_SKULL_04 + 1,
    ATTACK_SKULL_06 = ATTACK_SKULL_05 + 1,
    ATTACK_SKULL_07 = ATTACK_SKULL_06 + 1,
    ATTACK_SKULL_08 = ATTACK_SKULL_07 + 1,
    ATTACK_SKULL_09 = ATTACK_SKULL_08 + 1,
    ATTACK_SKULL_10 = ATTACK_SKULL_09 + 1,
    ATTACK_SKULL_11 = ATTACK_SKULL_10 + 1,
    IDLE_SKULL_00 = ATTACK_SKULL_11 + 1,
    IDLE_SKULL_01 = IDLE_SKULL_00 + 1,
    IDLE_SKULL_02 = IDLE_SKULL_01 + 1,
    IDLE_SKULL_03 = IDLE_SKULL_02 + 1,
    IDLE_SKULL_04 = IDLE_SKULL_03 + 1,
    IDLE_SKULL_05 = IDLE_SKULL_04 + 1,
    IDLE_SKULL_06 = IDLE_SKULL_05 + 1,
    IDLE_SKULL_07 = IDLE_SKULL_06 + 1,
    IDLE_SKULL_08 = IDLE_SKULL_07 + 1,
    IDLE_SKULL_09 = IDLE_SKULL_08 + 1,
    IDLE_SKULL_10 = IDLE_SKULL_09 + 1,
    IDLE_SKULL_11 = IDLE_SKULL_10 + 1,
    DIE_SKULL_00 = IDLE_SKULL_11 + 1,
    DIE_SKULL_01 = DIE_SKULL_00 + 1,
    DIE_SKULL_02 = DIE_SKULL_01 + 1,
    DIE_SKULL_03 = DIE_SKULL_02 + 1,
    DIE_SKULL_04 = DIE_SKULL_03 + 1,
    DIE_SKULL_05 = DIE_SKULL_04 + 1,
    DIE_SKULL_06 = DIE_SKULL_05 + 1,
    DIE_SKULL_07 = DIE_SKULL_06 + 1,
    DIE_SKULL_08 = DIE_SKULL_07 + 1,
    DIE_SKULL_09 = DIE_SKULL_08 + 1,
    DIE_SKULL_10 = DIE_SKULL_09 + 1,
    DIE_SKULL_11 = DIE_SKULL_10 + 1,
    DIE_SKULL_12 = DIE_SKULL_11 + 1,
    DIE_SKULL_13 = DIE_SKULL_12 + 1,
    DIE_SKULL_14 = DIE_SKULL_13 + 1,
    WALK_SKULL2_00 = DIE_SKULL_14 + 1,
    WALK_SKULL2_01 = WALK_SKULL2_00 + 1,
    WALK_SKULL2_02 = WALK_SKULL2_01 + 1,
    WALK_SKULL2_03 = WALK_SKULL2_02 + 1,
    WALK_SKULL2_04 = WALK_SKULL2_03 + 1,
    WALK_SKULL2_05 = WALK_SKULL2_04 + 1,
    WALK_SKULL2_06 = WALK_SKULL2_05 + 1,
    WALK_SKULL2_07 = WALK_SKULL2_06 + 1,
    WALK_SKULL2_08 = WALK_SKULL2_07 + 1,
    WALK_SKULL2_09 = WALK_SKULL2_08 + 1,
    WALK_SKULL2_10 = WALK_SKULL2_09 + 1,
    WALK_SKULL2_11 = WALK_SKULL2_10 + 1,
    WALK_SKULL2_12 = WALK_SKULL2_11 + 1,
    WALK_SKULL2_13 = WALK_SKULL2_12 + 1,
    WALK_SKULL2_14 = WALK_SKULL2_13 + 1,
    WALK_SKULL2_15 = WALK_SKULL2_14 + 1,
    WALK_SKULL2_16 = WALK_SKULL2_15 + 1,
    WALK_SKULL2_17 = WALK_SKULL2_16 + 1,
    ATTACK_SKULL2_00 = WALK_SKULL2_17 + 1,
    ATTACK_SKULL2_01 = ATTACK_SKULL2_00 + 1,
    ATTACK_SKULL2_02 = ATTACK_SKULL2_01 + 1,
    ATTACK_SKULL2_O3 = ATTACK_SKULL2_02 + 1,
    ATTACK_SKULL2_04 = ATTACK_SKULL2_O3 + 1,
    ATTACK_SKULL2_05 = ATTACK_SKULL2_04 + 1,
    ATTACK_SKULL2_06 = ATTACK_SKULL2_05 + 1,
    ATTACK_SKULL2_07 = ATTACK_SKULL2_06 + 1,
    ATTACK_SKULL2_08 = ATTACK_SKULL2_07 + 1,
    ATTACK_SKULL2_09 = ATTACK_SKULL2_08 + 1,
    ATTACK_SKULL2_10 = ATTACK_SKULL2_09 + 1,
    ATTACK_SKULL2_11 = ATTACK_SKULL2_10 + 1,
    IDLE_SKULL2_00 = ATTACK_SKULL2_11 + 1,
    IDLE_SKULL2_01 = IDLE_SKULL2_00 + 1,
    IDLE_SKULL2_02 = IDLE_SKULL2_01 + 1,
    IDLE_SKULL2_03 = IDLE_SKULL2_02 + 1,
    IDLE_SKULL2_04 = IDLE_SKULL2_03 + 1,
    IDLE_SKULL2_05 = IDLE_SKULL2_04 + 1,
    IDLE_SKULL2_06 = IDLE_SKULL2_05 + 1,
    IDLE_SKULL2_07 = IDLE_SKULL2_06 + 1,
    IDLE_SKULL2_08 = IDLE_SKULL2_07 + 1,
    IDLE_SKULL2_09 = IDLE_SKULL2_08 + 1,
    IDLE_SKULL2_10 = IDLE_SKULL2_09 + 1,
    IDLE_SKULL2_11 = IDLE_SKULL2_10 + 1,
    DIE_SKULL2_00 = IDLE_SKULL2_11 + 1,
    DIE_SKULL2_01 = DIE_SKULL2_00 + 1,
    DIE_SKULL2_02 = DIE_SKULL2_01 + 1,
    DIE_SKULL2_03 = DIE_SKULL2_02 + 1,
    DIE_SKULL2_04 = DIE_SKULL2_03 + 1,
    DIE_SKULL2_05 = DIE_SKULL2_04 + 1,
    DIE_SKULL2_06 = DIE_SKULL2_05 + 1,
    DIE_SKULL2_07 = DIE_SKULL2_06 + 1,
    DIE_SKULL2_08 = DIE_SKULL2_07 + 1,
    DIE_SKULL2_09 = DIE_SKULL2_08 + 1,
    DIE_SKULL2_10 = DIE_SKULL2_09 + 1,
    DIE_SKULL2_11 = DIE_SKULL2_10 + 1,
    DIE_SKULL2_12 = DIE_SKULL2_11 + 1,
    DIE_SKULL2_13 = DIE_SKULL2_12 + 1,
    DIE_SKULL2_14 = DIE_SKULL2_13 + 1,
    WALK_SKULL3_00 = DIE_SKULL2_14 + 1,
    WALK_SKULL3_01 = WALK_SKULL3_00 + 1,
    WALK_SKULL3_02 = WALK_SKULL3_01 + 1,
    WALK_SKULL3_03 = WALK_SKULL3_02 + 1,
    WALK_SKULL3_04 = WALK_SKULL3_03 + 1,
    WALK_SKULL3_05 = WALK_SKULL3_04 + 1,
    WALK_SKULL3_06 = WALK_SKULL3_05 + 1,
    WALK_SKULL3_07 = WALK_SKULL3_06 + 1,
    WALK_SKULL3_08 = WALK_SKULL3_07 + 1,
    WALK_SKULL3_09 = WALK_SKULL3_08 + 1,
    WALK_SKULL3_10 = WALK_SKULL3_09 + 1,
    WALK_SKULL3_11 = WALK_SKULL3_10 + 1,
    WALK_SKULL3_12 = WALK_SKULL3_11 + 1,
    WALK_SKULL3_13 = WALK_SKULL3_12 + 1,
    WALK_SKULL3_14 = WALK_SKULL3_13 + 1,
    WALK_SKULL3_15 = WALK_SKULL3_14 + 1,
    WALK_SKULL3_16 = WALK_SKULL3_15 + 1,
    WALK_SKULL3_17 = WALK_SKULL3_16 + 1,
    ATTACK_SKULL3_00 = WALK_SKULL3_17 + 1,
    ATTACK_SKULL3_01 = ATTACK_SKULL3_00 + 1,
    ATTACK_SKULL3_02 = ATTACK_SKULL3_01 + 1,
    ATTACK_SKULL3_O3 = ATTACK_SKULL3_02 + 1,
    ATTACK_SKULL3_04 = ATTACK_SKULL3_O3 + 1,
    ATTACK_SKULL3_05 = ATTACK_SKULL3_04 + 1,
    ATTACK_SKULL3_06 = ATTACK_SKULL3_05 + 1,
    ATTACK_SKULL3_07 = ATTACK_SKULL3_06 + 1,
    ATTACK_SKULL3_08 = ATTACK_SKULL3_07 + 1,
    ATTACK_SKULL3_09 = ATTACK_SKULL3_08 + 1,
    ATTACK_SKULL3_10 = ATTACK_SKULL3_09 + 1,
    ATTACK_SKULL3_11 = ATTACK_SKULL3_10 + 1,
    IDLE_SKULL3_00 = ATTACK_SKULL3_11 + 1,
    IDLE_SKULL3_01 = IDLE_SKULL3_00 + 1,
    IDLE_SKULL3_02 = IDLE_SKULL3_01 + 1,
    IDLE_SKULL3_03 = IDLE_SKULL3_02 + 1,
    IDLE_SKULL3_04 = IDLE_SKULL3_03 + 1,
    IDLE_SKULL3_05 = IDLE_SKULL3_04 + 1,
    IDLE_SKULL3_06 = IDLE_SKULL3_05 + 1,
    IDLE_SKULL3_07 = IDLE_SKULL3_06 + 1,
    IDLE_SKULL3_08 = IDLE_SKULL3_07 + 1,
    IDLE_SKULL3_09 = IDLE_SKULL3_08 + 1,
    IDLE_SKULL3_10 = IDLE_SKULL3_09 + 1,
    IDLE_SKULL3_11 = IDLE_SKULL3_10 + 1,
    DIE_SKULL3_00 = IDLE_SKULL3_11 + 1,
    DIE_SKULL3_01 = DIE_SKULL3_00 + 1,
    DIE_SKULL3_02 = DIE_SKULL3_01 + 1,
    DIE_SKULL3_03 = DIE_SKULL3_02 + 1,
    DIE_SKULL3_04 = DIE_SKULL3_03 + 1,
    DIE_SKULL3_05 = DIE_SKULL3_04 + 1,
    DIE_SKULL3_06 = DIE_SKULL3_05 + 1,
    DIE_SKULL3_07 = DIE_SKULL3_06 + 1,
    DIE_SKULL3_08 = DIE_SKULL3_07 + 1,
    DIE_SKULL3_09 = DIE_SKULL3_08 + 1,
    DIE_SKULL3_10 = DIE_SKULL3_09 + 1,
    DIE_SKULL3_11 = DIE_SKULL3_10 + 1,
    DIE_SKULL3_12 = DIE_SKULL3_11 + 1,
    DIE_SKULL3_13 = DIE_SKULL3_12 + 1,
    DIE_SKULL3_14 = DIE_SKULL3_13 + 1,
    TURRET = DIE_SKULL3_14 + 1,
    TURRET2 = TURRET + 1,
    WALL = TURRET2 + 1,
    CASTLE = WALL + 1,
    BACKGROUND = CASTLE + 1,
    BUTTON = BACKGROUND + 1,
    TITLE = BUTTON + 1,
    LAYER1 = TITLE + 1,
    LAYER2 = LAYER1 + 1,
    LAYER3 = LAYER2 + 1,
    LAYER4 = LAYER3 + 1,
    LAYER5 = LAYER4 + 1,
    BAR = LAYER5 + 1,
    TEXTURE_COUNT = BAR + 1
};
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	COLOURED = 0,
	PEBBLE = COLOURED + 1,
	SALMON = PEBBLE + 1,
	TEXTURED = SALMON + 1,
	WATER = TEXTURED + 1,
    TEXT = WATER + 1,
    LIGHT = TEXT + 1,
    LINE = LIGHT + 1,
    BAR = LINE + 1,
	EFFECT_COUNT = BAR + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	PEBBLE = SPRITE + 1,
	DEBUG_LINE = PEBBLE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	GEOMETRY_COUNT = SCREEN_TRIANGLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
    
    void to_json(json& j, const RenderRequest& rr) {
        j["TID"] = rr.used_texture;
        j["EID"] = rr.used_effect;
        j["GID"] = rr.used_geometry;
    }
    void from_json(const json& j, RenderRequest& rr) {
        j.at("TID").get_to(rr.used_texture);
        j.at("EID").get_to(rr.used_effect);
        j.at("GID").get_to(rr.used_geometry);
    }
};

