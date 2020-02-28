#pragma once
#include <Engine/String.hpp>
#include <Windows.h>

class GLContext;
class GLProgram;
class MaterialManager;
class ModelManager;

enum class EResourceType
{
	MATERIAL, MODEL
};

namespace ResourceSelect
{
	void Initialise();

	String Dialog(MaterialManager&, ModelManager&, const char *searchDir, HWND parent, EResourceType type, const GLContext &context, GLProgram& programLit, const GLProgram& programUnlit);
};
