#pragma once
#include "Buffer.hpp"
#include "GL.hpp"
#include "Texture.hpp"

class GLCubemap
{
private:
	GLuint _id;

public:
	GLCubemap() : _id(0) {}
	GLCubemap(const GLCubemap&) = delete;
	GLCubemap(GLCubemap&& other) noexcept : _id(other._id) { other._id = 0; }
	~GLCubemap() { glDeleteTextures(1, &_id); }

	void Create(Texture* faces[6]);

	void Bind(byte unit) const { glActiveTexture(GL_TEXTURE0 + unit); glBindTexture(GL_TEXTURE_CUBE_MAP, _id); }
};

