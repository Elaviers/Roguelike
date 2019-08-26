#pragma once
#include "GL.hpp"

class GLTexture
{
private:
	GLuint _id;

public:
	GLTexture() : _id(0) {}
	GLTexture(const GLTexture&) = delete;
	GLTexture(GLTexture&& other) noexcept : _id(other._id) { other._id = 0; }
	~GLTexture() {}

	inline static void SetCurrentUnit(byte unit) { glActiveTexture(GL_TEXTURE0 + unit); }

	void Create(GLsizei width, GLsizei height, const GLvoid *data, GLint mipLevels, GLint magFilter);

	inline void Bind(byte unit) const { SetCurrentUnit(unit); glBindTexture(GL_TEXTURE_2D, _id); }

	inline void Delete() { glDeleteTextures(1, &_id); }

	static inline void Unbind(byte unit) { SetCurrentUnit(unit); glBindTexture(GL_TEXTURE_2D, 0); }

	inline bool IsValid() const { return _id != 0; }
};
