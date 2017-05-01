#include "MapRenderer.h"

void EagleBird::Graphics::Renderers::MapRenderer::InitMap(WorldGeneration::WorldGenerator* generator, const uint32_t x_size, const uint32_t y_size, const uint32_t z_size)
{
	_shader = new OpenGL::Shader("Shaders/terrain.vert", "Shaders/terrain.frag");
	glm::vec3 base_color = glm::vec3(0.133f, 0.545f, 0.133f);
	glm::vec3 sea_color = glm::vec3(0.06f, 0.43f, 0.86f);
	glm::vec3 beach_color = glm::vec3(231 / 256.0, 207 / 256.0, 133 / 256.0);

	float color_differ = 0.00f;

	// Create the vertices vector and allocate enough memory.
	OpenGL::Vertex* vertices = new OpenGL::Vertex[6 * x_size / SIMPLENESS * z_size / SIMPLENESS];
	int vertices_counter = 0;

	glm::vec3** height_points = new glm::vec3*[x_size / SIMPLENESS];
	for (int i = 0; i < x_size / SIMPLENESS; i++)
	{
		height_points[i] = new glm::vec3[z_size / SIMPLENESS];
		for (int j = 0; j < z_size / SIMPLENESS; j++)
		{
			height_points[i][j] = glm::vec3(i + (rand() % 1000) / 2000.0f - 0.25f, 0.0f, j + (rand() % 1000) / 2000.0f - 0.25f);
			height_points[i][j].y = generator->GetHeight(height_points[i][j].x, height_points[i][j].z);
			height_points[i][j].x *= SIMPLENESS;
			height_points[i][j].z *= SIMPLENESS;
		}
	}

	for (int i = 0; i < x_size / SIMPLENESS - 1; i++)
	{
		for (int j = 0; j < z_size / SIMPLENESS - 1; j++)
		{
			glm::vec3 color = glm::vec3(base_color.r + ((rand() % 1000 - 500) / 500.0f * 0.0),
				base_color.g + ((rand() % 1000 - 500) / 500.0f * color_differ),
				base_color.b + ((rand() % 1000 - 500) / 500.0f * 0.0));


			// Generate 2 triangles per tile
			// First:
			glm::vec3* points;

			if (rand() % 2)
			{
				points = new glm::vec3[6]{
					glm::vec3(i + 1, 0, j),
					glm::vec3(i, 0, j),
					glm::vec3(i, 0, j + 1),
					glm::vec3(i + 1, 0, j),
					glm::vec3(i, 0, j + 1),
					glm::vec3(i + 1, 0, j + 1)
				};
			}
			else
			{
				points = new glm::vec3[6]{
					glm::vec3(i, 0, j),
					glm::vec3(i + 1, 0, j + 1),
					glm::vec3(i + 1, 0, j),
					glm::vec3(i, 0, j),
					glm::vec3(i, 0, j + 1),
					glm::vec3(i + 1, 0, j + 1)
				};
			}

			for (int i = 0; i < 6; i++)
			{
				glm::vec3 p = points[i];
				points[i] = height_points[(int)p.x][(int)p.z];
			}

			glm::vec3 normal = glm::triangleNormal(points[0], points[1], points[2]);
			glm::vec3 normal2 = glm::triangleNormal(points[3], points[4], points[5]);

			for (int i = 0; i < 6; i++)
			{
				if (points[i].y < 128.0)
				{
					color = sea_color;
				}
				else if (points[i].y < 132.0)
				{
					color = beach_color;
				}
			}
			glm::vec3 color3 = glm::vec3(0.5);
			glm::vec3 color2 = color;

			double angle1 = glm::angle(normal, glm::vec3(0.0, 1.0, 0.0));
			double angle2 = glm::angle(normal2, glm::vec3(0.0, 1.0, 0.0));

			if (angle1 > 45.0)
			{
				color.r = LinearInterp(color.r, color3.r, angle1 / 90.0f);
				color.g = LinearInterp(color.g, color3.g, angle1 / 90.0f);
				color.b = LinearInterp(color.b, color3.b, angle1 / 90.0f);
			}

			if (angle2 > 45.0)
			{
				color2.r = LinearInterp(color2.r, color3.r, angle2 / 90.0f);
				color2.g = LinearInterp(color2.g, color3.g, angle2 / 90.0f);
				color2.b = LinearInterp(color2.b, color3.b, angle2 / 90.0f);
			}
			

			/*if (glm::angle(normal, glm::vec3(0.0, 1.0, 0.0)) > 45.0)
			{
				color = glm::vec3(0.5);
			}
			if (glm::angle(normal2, glm::vec3(0.0, 1.0, 0.0)) > 45.0)
			{
				color2 = glm::vec3(0.5);
			}*/
			
			vertices[vertices_counter++] = OpenGL::Vertex(points[0], normal, color);
			vertices[vertices_counter++] = OpenGL::Vertex(points[1], normal, color);
			vertices[vertices_counter++] = OpenGL::Vertex(points[2], normal, color);
			vertices[vertices_counter++] = OpenGL::Vertex(points[3], normal2, color2);
			vertices[vertices_counter++] = OpenGL::Vertex(points[4], normal2, color2);
			vertices[vertices_counter++] = OpenGL::Vertex(points[5], normal2, color2);
		}
	}
	_vbo_length = vertices_counter * OpenGL::Vertex::GetSizeInByte();

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	if (_vbo_length > 0)
		glBufferData(GL_ARRAY_BUFFER, _vbo_length, &vertices[0], GL_STATIC_DRAW);

	printf("Vertices: %g\n", (float)_vbo_length / OpenGL::Vertex::GetSizeInByte());
}

void EagleBird::Graphics::Renderers::MapRenderer::Render(glm::mat4 &mvpMatrix)
{
	_shader->Bind();

	_shader->SetUniform(_shader->GetUniformLocation("MVP"), mvpMatrix);

	_shader->BindAttributeLocation("pos", 0);
	_shader->BindAttributeLocation("normal", 1);
	_shader->BindAttributeLocation("color", 2);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, OpenGL::Vertex::GetSizeInByte(), BUFFER_OFFSET(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, OpenGL::Vertex::GetSizeInByte(), BUFFER_OFFSET(12));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, OpenGL::Vertex::GetSizeInByte(), BUFFER_OFFSET(24));

	glDrawArrays(GL_TRIANGLES, 0, _vbo_length / (OpenGL::Vertex::GetSizeInByte()));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_shader->Release();
}
