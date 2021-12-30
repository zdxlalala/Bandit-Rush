
#define GL3W_IMPLEMENTATION
#include <gl3w.h>
// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

// int window_width_px = 1280;
// int window_height_px = 720;
int window_width_px;
int window_height_px;

// Entry point
void A() {
    printf("A");
}

void GetMaxMonitorResolution(int* w, int* h)
{
	glfwInit();
    int maxWidth = 0;
    int maxHeight = 0;
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    maxWidth = mode->width;
    maxHeight = mode->height;
    *w = maxWidth;
    *h = maxHeight;
}


int main(int argc, char* argv[])
{
	GetMaxMonitorResolution(&window_width_px, &window_height_px);

	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics(window_height_px, window_width_px);
	AISystem ai;

	// Initializing window
	GLFWwindow* window = world.create_window(window_width_px, window_height_px);
	renderer.init(window_width_px, window_height_px, window);
	world.init(&renderer);

	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

//    // initialize the menu systems
//    menu_renderer.init(window_width_px, window_height_px, window);
//
    std::string text_file_path = data_path() + "/fonts/" + "OCRAEXT.TTF";
    renderer.InitTextRenderer(window_width_px, window_height_px);
    renderer.Load(text_file_path, 48);
    while (!(world.is_play() || world.is_over())) {
        glfwPollEvents();
        renderer.draw();
        //renderer.RenderText("Press ENTER to start", 5, 5, 1 ,glm::vec3(0.5, 0.8f, 0.2f));
    }
    
    if (world.current_state == GameState::Play) {
        world.restart_game();
        // register the observer with callbck function
        //auto bc = [&]{world.handle_boundary_collisions();};
        auto c = [&]{world.handle_collisions();};
        //physics.attach(bc);
        physics.attach(c);
        ai.SetDimensions(window_width_px, window_height_px);
        world.setAiRef(&ai);
        
	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become
		// unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

        if (!renderer.is_over && !renderer.story && !renderer.help) {
            world.step(elapsed_ms);
            ai.step(elapsed_ms);
            physics.step(elapsed_ms, window_width_px, window_height_px);
            //world.handle_collisions();
        }

		renderer.draw();

		// TODO A2: you can implement the debug freeze here but other places are possible too.
	}
    }

	return EXIT_SUCCESS;
}
