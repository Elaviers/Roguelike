#pragma once
#include <ELSys/GL.hpp>

class GLTexture
{
private:
	GLuint _id;

public:
	GLTexture() : _id(0) {}
	GLTexture(const GLTexture&) = delete;
	GLTexture(GLTexture&& other) noexcept : _id(other._id) { other._id = 0; }
	~GLTexture() {}

	GLuint GetID() const { return _id; }

	static void SetCurrentUnit(byte unit) { glActiveTexture(GL_TEXTURE0 + unit); }

	void Create(GLsizei width, GLsizei height, const GLvoid *data, GLint mipLevels, GLfloat maxAnisotropy, GLint minFilter, GLint magFilter);

	void Bind(byte unit) const { SetCurrentUnit(unit); glBindTexture(GL_TEXTURE_2D, _id); }

	void Delete() { glDeleteTextures(1, &_id); }

	static void Unbind(byte unit) { SetCurrentUnit(unit); glBindTexture(GL_TEXTURE_2D, 0); }

	bool IsValid() const { return _id != 0; }
};
