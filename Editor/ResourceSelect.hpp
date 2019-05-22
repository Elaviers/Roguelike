#pragma once
#include <Engine/String.hpp>
#include <Windows.h>

class GLContext;
class GLProgram;
class MaterialManager;
class ModelManager;

enum class ResourceType
{
	MATERIAL, MODEL
};

namespace ResourceSelect
{
	void Initialise();

	String Dialog(MaterialManager&, ModelManager&, const char *searchDir, HWND parent, ResourceType type, const GLContext &context, const GLProgram &program);
};

