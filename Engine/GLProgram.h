#pragma once
#include "GL.h"
#include "Matrix.h"
#include "Vector.h"

class GLProgram
{
private:
	GLuint _id;

	static GLProgram* _currentProgram;

public:
	GLProgram() {}
	~GLProgram() {}
	GLProgram(const GLProgram &) = delete;

	inline static GLProgram& Current() { return *_currentProgram; }

	inline GLuint GetID() { return _id; }

	void Create(const char *vertSource, const char *fragSource);
	void Load(const char *vertFile, const char *fragFile);

	inline void Use() { glUseProgram(_id); _currentProgram = this; }
	GLint GetUniformLocation(const char *name);

	//Setter inlines

	inline void SetInt(const char *name, GLint x) { glUniform1i(GetUniformLocation(name), x); }
	inline void SetMat4(const char *name, const Mat4 &x) { glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &x[0][0]); }
	inline void SetVec3(const char *name, const Vector3 &x) { glUniform3fv(GetUniformLocation(name), 1, &x[0]); }
	inline void SetVec4(const char *name, const Vector4 &x) { glUniform4fv(GetUniformLocation(name), 1, &x[0]); }
};

