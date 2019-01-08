#pragma once
#include "ObjCamera.h"
#include "ModelManager.h"
#include "Types.h"

namespace DrawUtils
{

	void DrawGrid(const ModelManager&, const ObjCamera&, Direction, float lineWidth, float gap, float limit = 100.f, float offset = 0.f);

	void DrawLine(const ModelManager&, const Vector3 &p1, const Vector3 &p2);

	void DrawBox(const ModelManager&, const Vector3 &p1, const Vector3 &p2);
}
