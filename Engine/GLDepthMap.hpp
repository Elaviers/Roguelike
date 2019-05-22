#pragma once
#include "GL.hpp"

class GLDepthMap
{
private:
	GLuint _fbo; //Frame buffer
	GLuint _id;

public:
	GLDepthMap();
	~GLDepthMap();

	void Create();
};

