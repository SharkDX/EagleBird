#include "Game.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

EagleBird::Game::Game(GLFWwindow* window)
{
	glfwGetWindowSize(window, &_screen_width, &_screen_height);

	_is_running = false;
	_window = window;
}

EagleBird::Game::~Game()
{
	
}

void EagleBird::Game::LoadResources()
{
}

void EagleBird::Game::Initialize()
{
	_profiler = new Utils::Profiler();
	_profiler->StartNewProfile("Total Init");

	// OpenGL setup
	glClearColor(0.1f, 0.4f, 0.8f, 1);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Input setup
	// TODO make a custom cursor
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Init basic mechanics
	_camera = new Utils::Camera(_window);

	_profiler->StartNewProfile("World Generation");
	_world = new World(2048, 256, 2048);
	_profiler->EndProfile("World Generation");

	_profiler->StartNewProfile("Rendering Generation");
	_map_renderer = new Renderers::MapRenderer();
	_map_renderer->InitMap(_world->GetWorldGenerator(), _world->GetSizeX(), _world->GetSizeY(), _world->GetSizeZ());
	_profiler->EndProfile("Rendering Generation");

	_profiler->EndProfile("Total Init");
}

void EagleBird::Game::Update(float deltaTime)
{
	_camera->Update(deltaTime);
}

void EagleBird::Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4x4 mvp = _camera->getMVP();

	_map_renderer->Render(mvp);
}

void EagleBird::Game::Run()
{
	int frames = 0;
	double lastFrame = glfwGetTime();
	double deltaTime = 0;
	double lastUpdate = glfwGetTime();

	while (!glfwWindowShouldClose(_window) && glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		frames++;
		deltaTime = glfwGetTime() - lastUpdate;
		lastUpdate = glfwGetTime();

		if (glfwGetTime() - lastFrame >= 1.0)
		{
			printf("FPS: %d, Ram: %d MB\n", frames, Utils::SystemResources::get_total_memory_usage() / 1000 / 1000);
			lastFrame = glfwGetTime();
			frames = 0;
		}

		Update((float)deltaTime);
		Render();

		glfwSwapBuffers(_window);
		glfwPollEvents();

		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void EagleBird::Game::ScrollWheelCallback(double x, double y)
{
}

void EagleBird::Game::KeyCallback(int key, int scancode, int action, int mods)
{
}

void EagleBird::Game::MouseButtonCallback(int button, int action, int bits)
{
}

void EagleBird::Game::CharCallback(unsigned int keycode)
{
}