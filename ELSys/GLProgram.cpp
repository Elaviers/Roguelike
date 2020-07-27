#include "GLProgram.hpp"
#include "IO.hpp"
#include <ELCore/String.hpp>
#include <ELSys/Debug.hpp>

const GLProgram* GLProgram::_currentProgram = nullptr;

void CompileShader(GLuint shader, const char *src)
{
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	GLint value;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &value);
	if (value == 0)
	{
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &value);
		
		char* error = new char[value];
		glGetShaderInfoLog(shader, value, &value, error);
		Debug::Error(CSTR("Error compiling shader\n", error));
		delete[] error;

		glDeleteShader(shader);
	}
}

void GLProgram::Create(const char *vertSrc, const char *fragSrc)
{
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	CompileShader(vert, vertSrc);
	CompileShader(frag, fragSrc);

	_id = glCreateProgram();
	glAttachShader(_id, vert);
	glAttachShader(_id, frag);
	glLinkProgram(_id);

	GLint value;
	glGetProgramiv(_id, GL_LINK_STATUS, &value);
	if (value == 0)
	{
		glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &value);
		GLchar* error_str = (GLchar*)malloc(value);

		glGetProgramInfoLog(_id, value, &value, error_str);

		char* error = new char[value];
		glGetProgramInfoLog(_id, value, &value,  error);
		Debug::Error(error);
		delete[] error;

		glDeleteShader(frag);
		glDeleteShader(vert);
		glDeleteProgram(_id);
		return;
	}

	glDeleteShader(frag);
	glDeleteShader(vert);
}

void GLProgram::Load(const char *vertFile, const char *fragFile)
{
	String vertSrc = IO::ReadFileString(vertFile);
	String fragSrc = IO::ReadFileString(fragFile);

	Create(vertSrc.GetData(), fragSrc.GetData());
}

GLint GLProgram::GetUniformLocation(const char *name) const
{
	GLint location = glGetUniformLocation(_id, name);

	//if (location < 0) Error(CSTR("Uniform variable \"" + name + "\" does not exist"));

	return location;
}
