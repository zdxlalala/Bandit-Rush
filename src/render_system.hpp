#pragma once

#include <array>
#include <utility>

#include <map>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // size of glyph
    glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
    unsigned int Advance;   // horizontal offset to advance to next glyph
};

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("salmon.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("arche.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_000.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_001.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_002.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_003.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_004.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_005.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_006.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_007.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_008.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_009.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_010.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_011.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_012.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_013.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_014.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_015.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_016.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Walking/Walking_017.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_000.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_001.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_002.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_003.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_004.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_005.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_006.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_007.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_008.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_009.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_010.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Attacking/Attacking_011.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_000.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_001.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_002.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_003.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_004.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_005.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_006.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_007.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_008.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_009.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_010.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Idle Blink/Idle Blink_011.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_000.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_001.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_002.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_003.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_004.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_005.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_006.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_007.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_008.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_009.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_010.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_011.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_012.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_013.png"),
            textures_path("/PNG/Skull 01/PNG Sequences/Dying/Dying_014.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_000.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_001.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_002.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_003.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_004.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_005.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_006.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_007.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_008.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_009.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_010.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_011.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_012.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_013.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_014.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_015.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_016.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Walking/Walking_017.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_000.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_001.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_002.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_003.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_004.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_005.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_006.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_007.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_008.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_009.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_010.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Attacking/Attacking_011.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_000.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_001.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_002.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_003.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_004.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_005.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_006.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_007.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_008.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_009.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_010.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Idle Blink/Idle Blink_011.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_000.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_001.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_002.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_003.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_004.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_005.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_006.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_007.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_008.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_009.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_010.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_011.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_012.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_013.png"),
            textures_path("/PNG/Skull 02/PNG Sequences/Dying/Dying_014.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_000.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_001.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_002.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_003.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_004.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_005.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_006.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_007.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_008.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_009.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_010.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_011.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_012.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_013.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_014.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_015.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_016.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Walking/Walking_017.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_000.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_001.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_002.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_003.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_004.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_005.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_006.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_007.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_008.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_009.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_010.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Attacking/Attacking_011.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_000.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_001.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_002.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_003.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_004.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_005.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_006.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_007.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_008.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_009.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_010.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Idle Blink/Idle Blink_011.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_000.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_001.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_002.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_003.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_004.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_005.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_006.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_007.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_008.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_009.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_010.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_011.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_012.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_013.png"),
            textures_path("/PNG/Skull 03/PNG Sequences/Dying/Dying_014.png"),
            textures_path("turret.png"),
            textures_path("turret2.png"),
            textures_path("wall.png"),
            textures_path("castle.png"),
            textures_path("grass.jpg"),
            textures_path("button.png"),
            textures_path("title.png"),
            textures_path("layer1.png"),
            textures_path("layer2.png"),
            textures_path("layer3.png"),
            textures_path("layer4.png"),
            textures_path("layer5.png"),
            textures_path("bar.png")
    };

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("coloured"),
		shader_path("pebble"),
		shader_path("salmon"),
		shader_path("textured"),
		shader_path("water"),
        shader_path("text"),
        shader_path("light"),
        shader_path("line"),
        shader_path("bar")
    };

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;
    
    unsigned int instanceVBO;
    unsigned int instanceVBO2;

public:
	// Initialize the window
	bool init(int width, int height, GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();
    
    // matrix to update
    mat3 translationMatrix = {{0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}};

	mat3 createProjectionMatrix();
    
    // holds a list of pre-compiled Characters
    std::map<char, Character> Characters;
    
    // render state
    bool is_play;
    bool is_over;
    bool story;
    bool help = false;
    int change_txt;
    int change_txt_tut;
    bool next_lvl = false;
    bool levelOne = true;
    bool levelTwo = false;
    bool levelThree = false;
    int timeRemaining;
    int gold;
    bool tutorial = false;;
    bool restart;
    bool tutorial_pause = true;
    std::vector<std::string> end_array = {"Good Job!", "Well Done!", "Living another day!", "Not today burglars!", "Success!",
    "Perfection!", "Well done!", "Good Work!", "Excellent Job!", "On to the next day!"};

    
    vec2 offset1;
    vec2 offset2;
    vec2 offset3;
    vec2 offset4;
    vec2 offset5;
    
    // constructor
    void InitTextRenderer(unsigned int width, unsigned int height);
    // pre-compiles a list of characters from the given font
    void Load(std::string font, unsigned int fontSize);
    // renders a string of text using the precompiled list of characters
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
    void move_text(int index);
    void move_text_tut(int index);

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();

    // draw background
    void drawBackground(GLuint texture_asset_id, bool repeat);
    
    void drawArc(const mat3& projection);
    
    void drawLight(Entity entity, const mat3 &projection);
    
	// Window handle
	GLFWwindow* window;
	float screen_scale;  // Screen to pixel coordinates scale factor (for apple
						 // retina display?)

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
    
    // Text vao/vbo
    unsigned int VAO, VBO;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);
