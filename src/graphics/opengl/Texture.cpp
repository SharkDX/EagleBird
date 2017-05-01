#include "Texture.h"

#include <SOIL.h>
#include <assert.h>

EagleBird::Graphics::OpenGL::Texture::Texture(const std::string& path)
{
	LoadTexture(path);
}

EagleBird::Graphics::OpenGL::Texture::Texture(Texture&& other)
{
	other._texture = _texture;
	_texture = 0;
}

void EagleBird::Graphics::OpenGL::Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, _texture);
}

int EagleBird::Graphics::OpenGL::Texture::GetWidth() const
{
	return _width;
}

int EagleBird::Graphics::OpenGL::Texture::GetHeight() const
{
	return _height;
}

EagleBird::Graphics::OpenGL::Texture::~Texture()
{
	// TODO dispose the texture
}

void EagleBird::Graphics::OpenGL::Texture::LoadTexture(const std::string& path)
{
	_texture = SOIL_load_OGL_texture(
		path.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MULTIPLY_ALPHA
	);

	// TODO throw exception if 0
	assert(_texture != 0);

	// TODO IMPORATANT shouldn't the texture be bound before calling those functions?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	// Get texture size
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);
}
