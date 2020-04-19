#include "DrawUtils.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "Matrix.hpp"
#include "Ray.hpp"

#include "Engine.hpp"

#define X 0
#define Y 1
#define Z 2

using namespace Maths;
using namespace Utilities;

namespace DrawUtils
{
	inline bool FindZOverlap(Vector3 &out, const Ray &r, int axisX, int axisY, int axisZ, float limit, const Vector3 &camPos)
	{
		if (r.direction[axisZ] == 0.f)
			return false;

		float t = -r.origin[axisZ] / r.direction[axisZ];

		if (t < 0.f)
			return false;

		if (limit > 0.f)
		{
			out[axisX] = Clamp(r.origin[axisX] + r.direction[axisX] * t, camPos[axisX] - limit, camPos[axisX] + limit);
			out[axisY] = Clamp(r.origin[axisY] + r.direction[axisY] * t, camPos[axisY] - limit, camPos[axisY] + limit);
		}
		else
		{
			out[axisX] = r.origin[axisX] + r.direction[axisX] * t;
			out[axisY] = r.origin[axisY] + r.direction[axisY] * t;
		}
		return true;
	}

	inline void FindLimitOverlap(Vector3 &out, const Ray &r, int axisX, int axisY, int axisFWD, float limit, const Vector3 &camPos)
	{
		float t = limit / Abs(r.direction[axisFWD]);

		if (limit > 0.f)
		{
			out[axisX] = Clamp(r.origin[axisX] + r.direction[axisX] * t, camPos[axisX] - limit, camPos[axisX] + limit);
			out[axisY] = Clamp(r.origin[axisY] + r.direction[axisY] * t, camPos[axisY] - limit, camPos[axisY] + limit);
		}
		else
		{
			out[axisX] = r.origin[axisX] + r.direction[axisX] * t;
			out[axisY] = r.origin[axisY] + r.direction[axisY] * t;
		}
	}

	void DrawGrid(const ModelManager &modelManager, const EntCamera &camera, EDirection dir, float width, float gap, float limit, float offset)
	{
		Mat4 transformX;
		Mat4 transformY;

		int axisX;
		int axisY;
		int axisZ;

		switch (dir)
		{
		case EDirection::RIGHT:
			axisX = Z;
			axisY = Y;
			axisZ = X;
			transformX = Matrix::RotationY(90.f);
			transformY = Matrix::RotationX(90.f);
			break;
		case EDirection::UP:
			axisX = X;
			axisY = Z;
			axisZ = Y;
			transformX = Matrix::RotationX(90.f);
			transformY = Matrix::RotationZ(90.f);
			break;
		case EDirection::FORWARD:
			axisX = X;
			axisY = Y;
			axisZ = Z;
			transformY = Matrix::RotationZ(90.f);
			break;
		}

		bool results[4];
		Vector3 points[4];
		const Ray rays[4] = {
			camera.ScreenCoordsToRay(Vector2(-.5f, -.5f)),
			camera.ScreenCoordsToRay(Vector2(0.5f, -.5f)),
			camera.ScreenCoordsToRay(Vector2(-.5f, 0.5f)), 
			camera.ScreenCoordsToRay(Vector2(0.5f, 0.5f)) };

		Vector3 camPosition = camera.GetWorldPosition();

		for (int i = 0; i < 4; ++i)
		{
			points[i][axisX] = camPosition[axisX];
			points[i][axisY] = camPosition[axisY];
		}

		results[0] = FindZOverlap(points[0], rays[0], axisX, axisY, axisZ, limit, camPosition);
		results[1] = FindZOverlap(points[1], rays[1], axisX, axisY, axisZ, limit, camPosition);
		results[2] = FindZOverlap(points[2], rays[2], axisX, axisY, axisZ, limit, camPosition);
		results[3] = FindZOverlap(points[3], rays[3], axisX, axisY, axisZ, limit, camPosition);

		float startX;
		float startY;
		float maxX;
		float maxY;

		if (results[0] != results[1] || results[1] != results[2] || results[2] != results[3])
		{
			Vector3 fv = camera.GetWorldTransform().GetForwardVector();
			int axisFwd = Abs(fv[axisX]) > Abs(fv[axisY]) ? axisX : axisY;

			Vector3 morePoints[4];

			for (int i = 0; i < 4; ++i)
			{
				morePoints[i][axisX] = camPosition[axisX];
				morePoints[i][axisY] = camPosition[axisY];
			}

			FindLimitOverlap(morePoints[0], rays[0], axisX, axisY, axisFwd, limit, camPosition);
			FindLimitOverlap(morePoints[1], rays[1], axisX, axisY, axisFwd, limit, camPosition);
			FindLimitOverlap(morePoints[2], rays[2], axisX, axisY, axisFwd, limit, camPosition);
			FindLimitOverlap(morePoints[3], rays[3], axisX, axisY, axisFwd, limit, camPosition);

			startX =	Min(
						Min(points[0][axisX],
						Min(points[1][axisX],
						Min(points[2][axisX],
							points[3][axisX]))),
						Min(morePoints[0][axisX],
						Min(morePoints[1][axisX],
						Min(morePoints[2][axisX],
							morePoints[3][axisX]))));

			startY =	Min(
						Min(points[0][axisY],
						Min(points[1][axisY],
						Min(points[2][axisY],
							points[3][axisY]))),
						Min(morePoints[0][axisY],
						Min(morePoints[1][axisY],
						Min(morePoints[2][axisY],
							morePoints[3][axisY]))));

			maxX =		Max(
						Max(points[0][axisX],
						Max(points[1][axisX],
						Max(points[2][axisX],
							points[3][axisX]))),
						Max(morePoints[0][axisX],
						Max(morePoints[1][axisX],
						Max(morePoints[2][axisX],
							morePoints[3][axisX]))));

			maxY =		Max(
						Max(points[0][axisY],
						Max(points[1][axisY],
						Max(points[2][axisY],
							points[3][axisY]))),
						Max(morePoints[0][axisY],
						Max(morePoints[1][axisY],
						Max(morePoints[2][axisY],
							morePoints[3][axisY]))));
		}
		else
		{
			startX = Min(points[0][axisX], Min(points[1][axisX], Min(points[2][axisX], points[3][axisX])));
			startY = Min(points[0][axisY], Min(points[1][axisY], Min(points[2][axisY], points[3][axisY])));
			maxX = Max(points[0][axisX], Max(points[1][axisX], Max(points[2][axisX], points[3][axisX])));
			maxY = Max(points[0][axisY], Max(points[1][axisY], Max(points[2][axisY], points[3][axisY])));
		}

		transformX = Matrix::Scale(Vector3(1.f, maxY - startY, 1.f)) * transformX;
		transformY = Matrix::Scale(Vector3(1.f, maxX - startX, 1.f)) * transformY;
		transformX[3][axisY] = (startY + maxY) / 2.f;
		transformY[3][axisX] = (startX + maxX) / 2.f;

		startX = (int)(startX / gap) * gap + offset + (startX > 0 ? gap : 0);
		startY = (int)(startY / gap) * gap + offset + (startY > 0 ? gap : 0);

		for (float pos = startX; pos < maxX; pos += gap)
		{
			transformX[3][axisX] = pos;
			GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, transformX);
			modelManager.Line()->Render();
		}

		for (float pos = startY; pos < maxY; pos += gap)
		{
			transformY[3][axisY] = pos;
			GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, transformY);
			modelManager.Line()->Render();
		}
	}

	void DrawLine(const ModelManager &modelManager, const Vector3 &a, const Vector3 &b)
	{
		Transform t;
		t.SetPosition((a + b) / 2);
		t.SetScale(Vector3(0, (b - a).Length(), 0));

		Vector3 dir = (b - a).Normalise();

		t.SetRotation(Vector3(90 + Maths::ArcSineDegrees(dir.y), Maths::ArcTangentDegrees2(dir.x, dir.z), 0.f));

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		modelManager.Line()->Render();
	}

	void DrawBox(const ModelManager &modelManager, const Vector3 &p1, const Vector3 &p2)
	{
		DrawLine(modelManager, p1, Vector3(p2.x, p1.y, p1.z));
		DrawLine(modelManager, p1, Vector3(p1.x, p2.y, p1.z));
		DrawLine(modelManager, p1, Vector3(p1.x, p1.y, p2.z));

		DrawLine(modelManager, p2, Vector3(p1.x, p2.y, p2.z));
		DrawLine(modelManager, p2, Vector3(p2.x, p1.y, p2.z));
		DrawLine(modelManager, p2, Vector3(p2.x, p2.y, p1.z));

		DrawLine(modelManager, Vector3(p1.x, p2.y, p1.z), Vector3(p2.x, p2.y, p1.z));
		DrawLine(modelManager, Vector3(p1.x, p2.y, p1.z), Vector3(p1.x, p2.y, p2.z));
		DrawLine(modelManager, Vector3(p2.x, p1.y, p2.z), Vector3(p2.x, p1.y, p1.z));
		DrawLine(modelManager, Vector3(p2.x, p1.y, p2.z), Vector3(p1.x, p1.y, p2.z));

		DrawLine(modelManager, Vector3(p2.x, p1.y, p1.z), Vector3(p2.x, p2.y, p1.z));
		DrawLine(modelManager, Vector3(p1.x, p1.y, p2.z), Vector3(p1.x, p2.y, p2.z));
	}

	void DrawRing(const ModelManager& modelManager, const Vector3& centre, const Vector3& normal, float radius, int segments)
	{
		const float angStep = 2.f * Maths::PI_F / (float)segments;

		Vector3 r = VectorMaths::GetPerpendicularVector(normal);
		Vector3 u = normal.Cross(r).Normalise();

		r *= radius;
		u *= radius;

		float angle = 0.f;
		float x1 = 1.f, y1 = 0.f;
		float x2, y2;
		for (int i = 0; i < segments; ++i)
		{
			angle += angStep;
			x2 = Maths::Cosine(angle);
			y2 = Maths::Sine(angle);

			DrawLine(modelManager, centre + r * x1 + u * y1, centre + r * x2 + u * y2);

			x1 = x2;
			y1 = y2;
		}
	}
}
