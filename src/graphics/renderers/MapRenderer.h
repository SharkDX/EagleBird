#pragma once
#define BUFFER_OFFSET(i) ((void*)(i))
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtx\vector_angle.hpp>
#include <glm\gtx\normal.hpp>
#include "..\..\Config.h"
#include "..\..\world_generation\WorldGenerator.h"
#include <noise\interp.h>
#include "..\opengl\Shader.h"
#include "..\opengl\Vertex.h"

using namespace std;
using namespace EagleBird::Graphics;

namespace EagleBird
{
	namespace Graphics
	{
		namespace Renderers
		{
			class MapRenderer
			{
			public:
				void InitMap(WorldGeneration::WorldGenerator* generator, const uint32_t x_size, const uint32_t y_size, const uint32_t z_size);
				void Render(glm::mat4 &mvpMatrix);
			private:
				OpenGL::Shader* _shader;
				GLuint _vbo;
				GLuint _vbo_length;
			};
		}
	}
}

