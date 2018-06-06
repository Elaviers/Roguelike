#pragma once
#include "GL.h"

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

