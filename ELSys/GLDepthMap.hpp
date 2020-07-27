#pragma once
#include <ELSys/GL.hpp>

class GLDepthMap
{
private:
	GLuint _fbo; //Frame buffer
	GLuint _id;

public:
	GLDepthMap() : _fbo(0), _id(0) {}
	GLDepthMap(const GLDepthMap&) = delete;
	GLDepthMap(GLDepthMap&& other) noexcept : _fbo(other._fbo), _id(other._id) { other._fbo = other._id = 0; }
	~GLDepthMap() {}

	void Create();
};
