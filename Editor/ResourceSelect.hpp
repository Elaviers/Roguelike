#pragma once
#include <ELCore/String.hpp>
#include <Windows.h>

class EngineInstance;
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

	String Dialog(EngineInstance&, const char *searchDir, HWND parent, EResourceType type, const GLContext &context, GLProgram& programLit, const GLProgram& programUnlit);
};
