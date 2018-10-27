#pragma once
#include "Camera.h"
#include "ModelManager.h"
#include "Types.h"

namespace DrawUtils
{

	void DrawGrid(const ModelManager&, const Camera&, Direction, float lineWidth, float gap, float boundsScale = 1.f, float offset = 0.f);

	void DrawLine(const ModelManager&, const Vector3&, const Vector3&);
}
