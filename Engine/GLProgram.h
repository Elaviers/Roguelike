#pragma once
#include "GL.h"
#include "Matrix.h"
#include "Vector.h"

namespace DefaultUniformVars
{
	constexpr const char *mat4Projection = "M_Projection";
	constexpr const char *mat4View = "M_View";
	constexpr const char *mat4Model = "M_Model";

	constexpr const char *vec4Colour = "Colour";

	constexpr const char *vec2UVOffset = "UVOffset";
	constexpr const char *vec2UVScale = "UVScale";

	constexpr const char *intTextureDiffuse = "T_Diffuse";
	constexpr const char *intTextureNormal = "T_Normal";
	constexpr const char *intTextureSpecular = "T_Specular";
	constexpr const char *intTextureReflection = "T_Reflection";

	constexpr const char *intCubemap = "Cubemap";
}

class GLProgram
{
private:
	GLuint _id;

	static const GLProgram* _currentProgram;

public:
	GLProgram() : _id(0) {}
	~GLProgram() {}
	GLProgram(const GLProgram &) = delete;

	inline static const GLProgram& Current() { return *_currentProgram; }

	inline GLuint GetID() const { return _id; }

	void Create(const char *vertSource, const char *fragSource);
	void Load(const char *vertFile, const char *fragFile);

	inline void Use() const { glUseProgram(_id); _currentProgram = this; }
	GLint GetUniformLocation(const char *name) const;

	//Setter inlines

	inline void SetInt(const char *name, GLint x) const { glUniform1i(GetUniformLocation(name), x); }
	inline void SetMat4(const char *name, const Mat4 &x) const { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &x[0][0]); }
	inline void SetVec2(const char *name, const Vector2 &x) const { glUniform2fv(GetUniformLocation(name), 1, &x[0]); }
	inline void SetVec3(const char *name, const Vector3 &x) const { glUniform3fv(GetUniformLocation(name), 1, &x[0]); }
	inline void SetVec4(const char *name, const Vector4 &x) const { glUniform4fv(GetUniformLocation(name), 1, &x[0]); }
};
