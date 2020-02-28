#pragma once
#include "EntCamera.hpp"
#include "ModelManager.hpp"
#include "Types.hpp"

namespace DrawUtils
{

	void DrawGrid(const ModelManager&, const EntCamera&, EDirection, float lineWidth, float gap, float limit = 100.f, float offset = 0.f);

	void DrawLine(const ModelManager&, const Vector3 &p1, const Vector3 &p2);

	void DrawBox(const ModelManager&, const Vector3 &p1, const Vector3 &p2);

	void DrawRing(const ModelManager&, const Vector3& centre, const Vector3& nomal, float radius, int segments);
}
