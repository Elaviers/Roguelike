#pragma once
#include "MaterialManager.h"
#include "ModelManager.h"

namespace Engine
{
	namespace ObjectIDs
	{
		enum
		{
			NOT_OBJECT = 0,	//Used in level format for level messages
			RENDERABLE = 1,
			BRUSH2D = 2
		};
	}

	extern MaterialManager *materialManager;
	extern ModelManager *modelManager;
}
