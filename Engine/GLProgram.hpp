#pragma once
#include "GL.hpp"
#include "Matrix.hpp"
#include "Vector.hpp"

namespace DefaultUniformVars
{
	constexpr const char *mat4Projection = "M_Projection";
	constexpr const char *mat4View = "M_View";
	constexpr const char *mat4Model = "M_Model";

	constexpr const char* mat4aBones = "Bones";

	constexpr const char *vec4Colour = "Colour";

	constexpr const char *vec2UVOffset = "UVOffset";
	constexpr const char *vec2UVScale = "UVScale";

	constexpr const char *intTextureDiffuse = "T_Diffuse";
	constexpr const char *intTextureNormal = "T_Normal";
	constexpr const char *intTextureSpecular = "T_Specular";
	constexpr const char *intTextureReflection = "T_Reflection";

	constexpr const char *intCubemap = "Cubemap";

	constexpr const char *boolIsFont = "IsFont";
}

class GLProgram
{
private:
	GLuint _id;
	byte _channels;

	static const GLProgram* _currentProgram;

public:
	GLProgram() : _channels(0), _id(0) {}
	GLProgram(const GLProgram&) = delete;
	GLProgram(GLProgram&& other) noexcept : _id(other._id), _channels(other._channels) { other._id = other._channels = 0; }
	~GLProgram() {}

	inline static const GLProgram& Current() { return *_currentProgram; }

	inline GLuint GetID() const { return _id; }
	inline byte GetChannels() const { return _channels; }

	void Create(const char *vertSource, const char *fragSource, byte channels);
	void Load(const char *vertFile, const char *fragFile, byte channels);

	inline void Use() const { glUseProgram(_id); _currentProgram = this; }
	GLint GetUniformLocation(const char *name) const;

	inline GLProgram& operator=(GLProgram&& other) { _id = other._id; _channels = other._channels; other._id = other._channels = 0; }

	//Setter inlines

	inline void SetBool(const char* name, GLboolean x) const { glUniform1i(GetUniformLocation(name), x ? 1 : 0); }
	inline void SetInt(const char *name, GLint x) const { glUniform1i(GetUniformLocation(name), x); }
	inline void SetMat4(const char *name, const Mat4 &x) const { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &x[0][0]); }
	inline void SetVec2(const char *name, const Vector2 &x) const { glUniform2fv(GetUniformLocation(name), 1, &x[0]); }
	inline void SetVec3(const char *name, const Vector3 &x) const { glUniform3fv(GetUniformLocation(name), 1, &x[0]); }
	inline void SetVec4(const char *name, const Vector4 &x) const { glUniform4fv(GetUniformLocation(name), 1, &x[0]); }
};