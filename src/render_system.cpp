// internal
#include "render_system.hpp"
#include <SDL.h>

#include <glm/gtc/matrix_transform.hpp>
#include <../ext/ft2/include/ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <random>
#include <vector>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawLight(Entity entity,
                                    const mat3 &projection)
{
    Motion &motion = registry.motions.get(entity);
    // Transformation code, see Rendering and Transformation in the template
    // specification for more info Incrementally updates transformation matrix,
    // thus ORDER IS IMPORTANT
    Transform transform;
    transform.translate(motion.position);
    transform.rotate(motion.angle);
    transform.scale(motion.scale);
    // !!! TODO A1: add rotation to the chain of transformations, mind the order
    // of transformations

    assert(registry.renderRequests.has(entity));
    const RenderRequest &render_request = registry.renderRequests.get(entity);

    const GLuint used_effect_enum = (GLuint)render_request.used_effect;
    assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
    const GLuint program = (GLuint)effects[used_effect_enum];

    // Setting shaders
    glUseProgram(program);
    gl_has_errors();

    assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
    const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
    const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

    // Setting vertex and index buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_has_errors();

    // Input data location as in the vertex buffer
    if (render_request.used_effect == EFFECT_ASSET_ID::LIGHT)
    {
        GLint in_position_loc = glGetAttribLocation(program, "in_position");
        GLint in_color_loc = glGetAttribLocation(program, "in_color");
        gl_has_errors();

        glEnableVertexAttribArray(in_position_loc);
        glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(ColoredVertex), (void *)0);
        gl_has_errors();

        glEnableVertexAttribArray(in_color_loc);
        glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
                              sizeof(ColoredVertex), (void *)sizeof(vec3));
        gl_has_errors();
    }
    else
    {
        assert(false && "Type of render request not supported");
    }

    // Getting uniform locations for glUniform* calls
    GLint color_uloc = glGetUniformLocation(program, "fcolor");
    const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
    glUniform3fv(color_uloc, 1, (float *)&color);
    gl_has_errors();

    // Get number of indices from index buffer, which has elements uint16_t
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_has_errors();

    GLsizei num_indices = size / sizeof(uint16_t);
    // GLsizei num_triangles = num_indices / 3;

    GLint currProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
    // Setting uniform values to the currently bound program
    GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
    glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
    GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
    mat3 translation = translationMatrix;
    GLuint translation_loc = glGetUniformLocation(currProgram, "translation");
    glUniformMatrix3fv(translation_loc, 1, GL_FALSE, (float *)&translation);
    gl_has_errors();
    // Drawing of num_indices/3 triangles specified in the index buffer
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
    gl_has_errors();
}

void RenderSystem::drawArc(const mat3& projection){
    int length = registry.softShells.components.size();
    std::vector<mat3> transforms;
    std::vector<vec3> colors;
    for (uint i = 0; i < length; i++) {
        Entity entity = registry.softShells.entities[i];
        Motion &motion = registry.motions.get(entity);
    // Transformation code, see Rendering and Transformation in the template
    // specification for more info Incrementally updates transformation matrix,
    // thus ORDER IS IMPORTANT
        Transform transform;
        transform.translate(motion.position);
        transform.rotate(motion.angle);
        transform.scale(motion.scale);
        
        transforms.push_back(transform.mat);
        const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
        colors.push_back(color);
    }
    // !!! TODO A1: add rotation to the chain of transformations, mind the order
    // of transformations

    //assert(registry.renderRequests.has(entity));
    //const RenderRequest &render_request = registry.renderRequests.get(entity);

    const GLuint used_effect_enum = (GLuint)EFFECT_ASSET_ID::PEBBLE;
    assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
    const GLuint program = (GLuint)effects[used_effect_enum];

    
    
    // Setting shaders
    glUseProgram(program);
    gl_has_errors();

    //assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
    const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];
    const GLuint ibo = index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];

    // Setting vertex and index buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_has_errors();
    
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
    GLint pos = glGetAttribLocation(program, "transform");
    GLint color_uloc = glGetAttribLocation(program, "app_color");
    gl_has_errors();

    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(TexturedVertex), (void *)0);
    gl_has_errors();

    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE,
                          sizeof(TexturedVertex), (void *)sizeof(vec3));
    gl_has_errors();
    
    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    gl_has_errors();

    GLuint texture_id =
        texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::FISH];

    glBindTexture(GL_TEXTURE_2D, texture_id);
    gl_has_errors();
//
//
//    // Getting uniform locations for glUniform* calls
//    GLint color_uloc = glGetUniformLocation(program, "fcolor");
//    const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
//    glUniform3fv(color_uloc, 1, (float *)&color);
//    gl_has_errors();
    
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * length, &colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(color_uloc);
    glVertexAttribPointer(color_uloc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(GLfloat) * 3, (void *)0);
    glVertexAttribDivisor(color_uloc, 1);
    gl_has_errors();
    
    
    int pos1 = pos + 0;
    int pos2 = pos + 1;
    int pos3 = pos + 2;
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat3) * length, &transforms[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(pos1);
    glEnableVertexAttribArray(pos2);
    glEnableVertexAttribArray(pos3);
    glVertexAttribPointer(pos1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(0));
    glVertexAttribPointer(pos2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(sizeof(float) * 3));
    glVertexAttribPointer(pos3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3 * 3, (void*)(sizeof(float) * 6));
    glVertexAttribDivisor(pos1, 1);
    glVertexAttribDivisor(pos2, 1);
    glVertexAttribDivisor(pos3, 1);
    gl_has_errors();
    
    // Get number of indices from index buffer, which has elements uint16_t
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_has_errors();

    GLsizei num_indices = size / sizeof(uint16_t);
    // GLsizei num_triangles = num_indices / 3;

    GLint currProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
//    // Setting uniform values to the currently bound program
//    GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
//    glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
    GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
    mat3 translation = translationMatrix;
    GLuint translation_loc = glGetUniformLocation(currProgram, "translation");
    glUniformMatrix3fv(translation_loc, 1, GL_FALSE, (float *)&translation);
    gl_has_errors();
//    // Drawing of num_indices/3 triangles specified in the index buffer
//    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
    
    glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, 0, length);
    gl_has_errors();
}


void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	transform.translate(motion.position);
    transform.rotate(motion.angle);
	transform.scale(motion.scale);
	// !!! TODO A1: add rotation to the chain of transformations, mind the order
	// of transformations

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED || render_request.used_effect == EFFECT_ASSET_ID::BAR)
	{
        if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED) {
            GLuint b_uloc = glGetUniformLocation(program, "b");
            glUniform1i(b_uloc, false);
        }
        
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::SALMON || render_request.used_effect == EFFECT_ASSET_ID::LINE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();

		if (render_request.used_effect == EFFECT_ASSET_ID::SALMON)
		{
			// Light up?
			GLint light_up_uloc = glGetUniformLocation(program, "light_up");
			assert(light_up_uloc >= 0);

			// !!! TODO A1: set the light_up shader variable using glUniform1i,
			// similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
			gl_has_errors();
		}
	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
    mat3 translation = translationMatrix;
    GLuint translation_loc = glGetUniformLocation(currProgram, "translation");
    glUniformMatrix3fv(translation_loc, 1, GL_FALSE, (float *)&translation);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(water_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
    if (this->is_play) {
        screen.darken_screen_factor = 1;
    } else {
        screen.darken_screen_factor = 1;
    }
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 1, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
    
    // Draw background map for game
    if (!this->is_play) {
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER1, false);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER1, true);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER2, false);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER2, true);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER3, false);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER3, true);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER4, false);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER4, true);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER5, false);
        drawBackground((GLuint)TEXTURE_ASSET_ID::LAYER5, true);
    } else {
        drawBackground((GLuint)TEXTURE_ASSET_ID::BACKGROUND, false);
    }
    
    // Draw castle for game
    //drawTexture((GLuint)TEXTURE_ASSET_ID::CASTLE);
    
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity) || registry.softShells.has(entity) || registry.light.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);
	}
    if (registry.softShells.size() > 0)
        drawArc(projection_2D);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (Entity entity : registry.light.entities)
    {
        // Note, its not very efficient to access elements indirectly via the entity
        // albeit iterating through all Sprites in sequence. A good point to optimize
        drawLight(entity, projection_2D);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Truely render to the screen
	drawToScreen();
    int w1, h1;
    glfwGetWindowSize(window, &w1, &h1);
    if (!this->is_play) {
        RenderText("Bandit Rush", w1/2-156*1.5, h1/2-150-24, 1.5,glm::vec3(1, 0, 0));
        RenderText("Play", w1/2-48*0.75,h1/2+50-24*0.75,0.75,glm::vec3(1, 0.0f, 0.0f));
        RenderText("Quit", w1/2-48*0.75,h1/2+150-24*0.75,0.75,glm::vec3(1, 0.0f, 0.0f));
    } else {
        if (this->is_over) {
            RenderText("GAME OVER!", w1/2-156*1.5, h1/2-150-24, 1.5,glm::vec3(1, 0, 0));
            RenderText("Press R to Restart", w1/2-156, h1/2-150-24 + 80, 0.5,glm::vec3(1, 0, 0));
            RenderText("Press ESC to Quit", w1/2-156, h1/2-150-24 + 160, 0.5,glm::vec3(1, 0, 0));
        } else {
            if (this->story) {
                if (this->change_txt == 1) {
                    RenderText("Your castle is under attack!", 150, 225, 0.5,glm::vec3(.5, .2, .3));
                    RenderText("Bandits and burglars want to steal all of your treasures!", 150, 245, 0.5,glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt == 2) {
                    RenderText("You can place walls and towers to stop the bandits from reaching your home,", 150,225,0.5,glm::vec3(.5, .2, .3));
                    RenderText("but be careful, they are not cheap, and they don't last forever.", 150, 245, 0.5, glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt == 3) {
                    RenderText("Good news is that you expected this,", 150,225,0.5,glm::vec3(.5, .2, .3));
                    RenderText("so you bought a shooter which is able to shoot the burglars.", 150,245,0.5,glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt == 4) {
                    RenderText("However,", 150,225,0.5,glm::vec3(.5, .2, .3));
                    RenderText("Once you kill all bandits, you will be able to live another day.", 150,245,0.5,glm::vec3(.5, .2, .3));
                    RenderText("Good Luck!", 150,265,0.5,glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt == 5) {
                    RenderText("Press C to continue with the game.", 400, 300, 0.5,glm::vec3(1, 0, 0));
                }
                else {
                    this->story = false;
                }
                if (this->change_txt != 5) RenderText("Press space to continue", 400, 200, 0.5,glm::vec3(.5, .2, .3));
            }
            else if (this->tutorial) {
                // font size based on the screen res of player
                float txt_size = w1*1e-3*0.4;
                if (this->change_txt_tut == 1) {
                    // pause the game
                    RenderText("Welcome to the tutorial!", 125, 150, txt_size,glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt_tut == 2) {
                    // continue to stay in pause, nothing else to do here
                    RenderText("Move your mouse around. Press your arrow keys to move the camera.", 125, 150, txt_size, glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt_tut == 3) {
                    RenderText("Press 0 to insert a wall in the path of the enemy. Each wall costs 5 golds.", 125,150,txt_size,glm::vec3(.5, .2, .3));
                    RenderText("A wall is only able to stop the enemy from moving, and cannot fight back.", 125,175,txt_size,glm::vec3(.5, .2, .3));
                    this->tutorial_pause = false;
                    // unpause. wait for the enemy to reach the wall and destroy it. if it doesn't, restart the tutorial
                }
                else if (this->change_txt_tut == 4) {
                    // pause

                    RenderText("Now, press 1 to insert a turret. Each turret costs 10 golds.", 125,150,txt_size,glm::vec3(.5, .2, .3));
                    RenderText("A turret is able to shoot at the enemy if the enemy is close to it.", 125,175,txt_size,glm::vec3(.5, .2, .3));
                    RenderText("You have a limited amount of gold, so be careful of how you spend it.", 125, 200,txt_size,glm::vec3(.5, .2, .3));
                    this->tutorial_pause = false;
                    // unpause, wait for the enemy to reach the turret
                }
                else if (this->change_txt_tut == 5) {
                    // this->tutorial_pause = false;
                    // unpause, wait for enemy to reach the castle
                    RenderText("As time goes by and you move on to harder levels,", 125,150, txt_size,glm::vec3(.5, .2, .3));
                    RenderText("the enemies become stronger and faster.", 175,175, txt_size,glm::vec3(.5, .2, .3));
                    RenderText("If an enemy reaches the castle, they will continue to steal your gold", 125, 200, txt_size,glm::vec3(.5, .2, .3));
                    RenderText("until they die or there are no more golds left to steal.", 175, 225, txt_size,glm::vec3(.5, .2, .3));
                }
                else if (this->change_txt_tut == 6) {
                    // once it reaches game over, render this instead, automatically restart the game (auto press r)
                    RenderText("Now you are ready to defend your castle from the bandits!", 125, 150, txt_size,glm::vec3(.5, .2, .3));
                    // this->restart = true;
                    // this->tutorial = false;
                }
                else if (this->change_txt_tut == 7) {
                    // once it reaches game over, render this instead, automatically restart the game (auto press r)
                    RenderText("Press space to leave the tutorial and start the game,", 125, 150, txt_size,glm::vec3(.5, .2, .3));
                    RenderText("or continue to practice.", 125, 175, txt_size,glm::vec3(.5, .2, .3));
                    this->restart = true;
                }
                if (this->change_txt_tut <= 6 && this->change_txt_tut != 0) {
                    RenderText("Press space to continue", 250, 250, txt_size,glm::vec3(.5, .2, .3));
                    RenderText(std::to_string(this->gold), w1/2+425, h1/2-375, 0.8, glm::vec3(1, 1, 1));
                }
                    // Welcome to the tutorial! Move your mouse around and you could see the spots you can insert your turrets and walls
                    // (after doing so) wait for an enemy to enter
                    // (have an enemy walk in in a straight line, have an arrow pointing to where to place the wall/turret, pause the game)
                    // press 0 to insert a wall. a wall is only able to stop the enemy from moving, and cannot fight back.
                    // press 1 to insert a turret. a turret is able to shoot at the enemy if the enemy is close to it
                    // as time goes by and you move on to harder levels, the enemies become stronger and faster
                    // if an enemy reaches the castle, they will continue to steal your gold until they die or there are no more golds left to steal.
                    // now you are ready to defend your castle from the bandits!
            }
            else if (this->help) {
                RenderText("1.Press 1 to place tower at mouse position used 10 GOLD", 5,150,0.5,glm::vec3(.5, .2, .3));
                RenderText("2.Press 0 to place wall at mouse position for 5 GOLD", 5, 170, 0.5, glm::vec3(.5, .2, .3));
                RenderText("3.Press 2 to place advanced tower at mouse position for 20 GOLD", 5, 190, 0.5, glm::vec3(.5, .2, .3));
                RenderText("4.Press P to pause the motion of skulls", 5,210,0.5,glm::vec3(.5, .2, .3));
                RenderText("5.Press C to make skull continue moving", 5,230,0.5,glm::vec3(.5, .2, .3));
                RenderText("6.Press ESC to exit the game", 5,250,0.5,glm::vec3(.5, .2, .3));
                RenderText("Note the turret will be destroyed when health become zero", 5,270,0.5,glm::vec3(.5, .2, .3));
                RenderText("Press C to continue with the game.", 5, 325, 0.5,glm::vec3(1, 0, 0));

            }
            else if (this->is_play) {
//                RenderText("Press H for Help", 5,25,0.5,glm::vec3(1, 0.0f, 0.0f));
//                RenderText("Press K for Story", 5, 45, 0.5, glm::vec3(1, 0.0f, 0.0f));
//                RenderText("Press S to SAVE", 5, 65, 0.5, glm::vec3(1, 0.0f, 0.0f));
//                RenderText("Press L to LOAD", 5, 85, 0.5, glm::vec3(1, 0.0f, 0.0f));
                if (levelOne) {
                    RenderText("LEVEL ONE", w1/2-90*1.25, h1/2-375, 1,glm::vec3(1, 1, 1));
                }else if (levelTwo){
                    RenderText("LEVEL TWO", w1/2-90*1.25, h1/2-375, 1,glm::vec3(1, 1, 1));
                }else if(levelThree){
                    RenderText("LEVEL THREE", w1/2-90*1.25, h1/2-375, 1,glm::vec3(1, 1, 1));
                }
                RenderText(std::to_string(this->timeRemaining), w1/2+600, h1/2-375, 0.8, glm::vec3(1, 1, 1));
                RenderText(std::to_string(this->gold), w1/2+425, h1/2-375, 0.8, glm::vec3(1, 1, 1));
            }
            else if (this->next_lvl) {
                // for when user has ended this level
                RenderText(this->end_array[std::rand()%10], 5, 5, 0.5,glm::vec3(.5, .2, .3));
            }

        }
    }
	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

void RenderSystem::move_text(int index) {
    this->change_txt += index;
};

void RenderSystem::move_text_tut(int index) {
    this->change_txt_tut += index;
};

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	gl_has_errors();
	float right = (float)w / screen_scale;
	float bottom = (float)h / screen_scale;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

void RenderSystem::drawBackground(GLuint texture_asset_id, bool repeat) {
    
    const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::TEXTURED];
    
    // Setting shaders
    glUseProgram(program);
    gl_has_errors();

    //assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
    const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];
    const GLuint ibo = index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];

    // Setting vertex and index buffers
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    gl_has_errors();
    
    GLint in_position_loc = glGetAttribLocation(program, "in_position");
    GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
    gl_has_errors();
    assert(in_texcoord_loc >= 0);

    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
                          sizeof(TexturedVertex), (void *)0);
    gl_has_errors();

    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(
        in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
        (void *)sizeof(
            vec3)); // note the stride to skip the preceeding vertex position
    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    gl_has_errors();

    // assert(registry.renderRequests.has(entity));
    GLuint texture_id = texture_gl_handles[texture_asset_id];

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl_has_errors();

    // Getting uniform locations for glUniform* calls
    GLint color_uloc = glGetUniformLocation(program, "fcolor");
    const vec3 color = vec3(1);
    glUniform3fv(color_uloc, 1, (float *)&color);
    gl_has_errors();

    // Get number of indices from index buffer, which has elements uint16_t
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_has_errors();

    GLsizei num_indices = size / sizeof(uint16_t);
    // GLsizei num_triangles = num_indices / 3;

    GLint currProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
    // Setting uniform values to the currently bound program
    mat3 projection;
    mat3 transformation;
    projection = {{1, 0.f, 0}, {0.f, 1, 0}, {0, 0, 1.f}};
    
    if(this->is_play) {
        GLuint b_uloc = glGetUniformLocation(program, "b");
        glUniform1i(b_uloc, true);
        
        GLuint resolution_uloc = glGetUniformLocation(program, "resolution");
        int screen_width;
        int screen_height;
        glfwGetWindowSize(window, &screen_width, &screen_height);
        const vec3 resolution = vec3(screen_width, screen_height,1);
        glUniform3fv(resolution_uloc, 1, (float *)&resolution);
        gl_has_errors();
        if (!repeat) {
            transformation = {{4, 0.f, 0}, {0.f, -4, 0}, {0, 0, 1.f}};
        }
    } else {
        GLuint b_uloc = glGetUniformLocation(program, "b");
        glUniform1i(b_uloc, false);
        if (texture_asset_id == (GLuint)TEXTURE_ASSET_ID::LAYER1){
            if (!repeat) {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset1.x, 0, 1.f}};
            } else {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset1.y, 0, 1.f}};
            }
        } else if (texture_asset_id == (GLuint)TEXTURE_ASSET_ID::LAYER2){
            if (!repeat) {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset2.x, 0, 1.f}};
            } else {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset2.y, 0, 1.f}};
            }
        }else if (texture_asset_id == (GLuint)TEXTURE_ASSET_ID::LAYER3){
            if (!repeat) {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset3.x, 0, 1.f}};
            } else {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset3.y, 0, 1.f}};
            }
        } else if (texture_asset_id == (GLuint)TEXTURE_ASSET_ID::LAYER4){
            if (!repeat) {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset4.x, 0, 1.f}};
            } else {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset4.y, 0, 1.f}};
            }
        } else if (texture_asset_id == (GLuint)TEXTURE_ASSET_ID::LAYER5){
            if (!repeat) {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset5.x, 0, 1.f}};
            } else {
                transformation = {{2.1, 0.f, 0}, {0.f, -2, 0}, {offset5.y, 0, 1.f}};
            }
        }
        offset1 += 0.0001;
        offset2 += 0.0002;
        offset3 += 0.0003;
        offset4 += 0.0004;
        offset5 += 0.0005;
        if (offset1.x > 2)
            offset1.x-=4.f;
        if (offset1.y > 2)
            offset1.y-=4.f;
        if (offset2.x >= 2)
            offset2.x-=4.f;
        if (offset2.y >= 2)
            offset2.y-=4.f;
        if (offset3.x >= 2)
            offset3.x-=4.f;
        if (offset3.y >= 2)
            offset3.y-=4.f;
        if (offset4.x >= 2)
            offset4.x-=4.f;
        if (offset4.y >= 2)
            offset4.y-=4.f;
        if (offset5.x >= 2)
            offset5.x-=4.f;
        if (offset5.y >= 2)
            offset5.y-=4.f;
    }
    
    GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
    glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transformation);
    GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
    gl_has_errors();
    // Drawing of num_indices/3 triangles specified in the index buffer
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
    gl_has_errors();
    
}

void RenderSystem::InitTextRenderer(unsigned int width, unsigned int height)
{
    const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::TEXT];

    // Setting shaders
    glUseProgram(program);
    gl_has_errors();
    
    mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, false, (float *)&projection);
    glUniform1i(glGetUniformLocation(program, "text"), 0);
    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
//    glBindVertexArray(this->VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
}

void RenderSystem::Load(std::string font, unsigned int fontSize)
{
    // first clear the previously loaded Characters
    this->Characters.clear();
    // then initialize and load the FreeType library
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; c++) // lol see what I did there
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
            );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    //glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void RenderSystem::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::TEXT];

    // Setting shaders
    glUseProgram(program);
    gl_has_errors();
    
//    glGenVertexArrays(1, &this->VAO);
//    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    
    // activate corresponding render state
    //this->TextShader.Use();
    //this->TextShader.SetVector3f("textColor", color);
    glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
    
    glActiveTexture(GL_TEXTURE0);
    //glBindVertexArray(this->VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        //glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        //glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
    //glBindVertexArray(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
}
