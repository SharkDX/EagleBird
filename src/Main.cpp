#include <iostream>
#include <assert.h>
#include <ctime>
#include "windows.h"
#include "psapi.h"
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include <random>

void error_callback(int error, const char* description);
void ScrollWheelCallback(GLFWwindow * window, double x, double y);
void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow * window, int button, int action, int bits);
void CharCallback(GLFWwindow * window, unsigned int keycode);

EagleBird::Game* game = nullptr;

int main(int argc, char const* argv[])
{
	#ifdef _WIN32
		// Turn on vertical screen sync under Windows.
		// (I.e. it uses the WGL_EXT_swap_control extension)
		typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(1);

		// Move console window to position
		HWND consoleWindow = GetConsoleWindow();
		SetWindowPos(consoleWindow, 0, 0, 180, 640, 720, SWP_NOZORDER);
	#endif

	// Setup randomness
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<double> dist(0, INT_MAX);
	int seed = (int)(dist(mt));
	srand(seed);
	std::cout << "Random seed: " << seed << std::endl;

	// TODO add try/catch after adding exceptions

	int err = glfwInit();
	assert(err); // TODO

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwSetErrorCallback(error_callback);
	GLFWwindow* window = glfwCreateWindow(1280, 720, "Eagle Bird", nullptr, nullptr);
	glfwSetWindowPos(window, 640, 180);
	assert(window); // TODO terminate and report
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	err = glewInit();
	assert(!err); // TODO

	// TODO important! verify that all those callbacks gets called on the same thread. (probably glfwPollEvents calls them synchronously)
	glfwSetScrollCallback(window, &ScrollWheelCallback);
	glfwSetKeyCallback(window, &KeyCallback);
	glfwSetCharCallback(window, &CharCallback);
	glfwSetMouseButtonCallback(window, &MouseButtonCallback);
	
	game = new EagleBird::Game(window);
	game->LoadResources();
	game->Initialize();
	// TODO maybe move the main loop, to Main.cpp?
	game->Run();
	delete game;

	glfwTerminate();
	// TODO de-initialize glew?

	return 0;
}

void error_callback(int error, const char* description)
{
	fputs(description, stderr);
	std::cout << std::endl;
	system("pause");
}

void ScrollWheelCallback(GLFWwindow* window, double x, double y)
{
	
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int bits)
{
	
}

void CharCallback(GLFWwindow* window, unsigned int keycode)
{
	
}