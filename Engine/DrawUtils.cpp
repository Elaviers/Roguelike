#include "DrawUtils.h"
#include "GL.h"
#include "GLProgram.h"
#include "Matrix.h"

#define X 0
#define Y 1
#define Z 2

namespace DrawUtils
{

	void DrawGrid(const ModelManager &modelManager, const Camera &camera, Direction dir, float width, float gap, float boundsScale, float offset)
	{
		float vpW = camera.GetViewport()[0] * boundsScale / camera.GetScale();
		float vpH = camera.GetViewport()[1] * boundsScale / camera.GetScale();

		Mat4 transformX = Matrix::Scale(Vector3(1.f, vpH, 1.f));
		Mat4 transformY = Matrix::Scale(Vector3(1.f, vpW, 1.f));

		int axisX;
		int axisY;

		switch (dir)
		{
		case Direction::RIGHT:
			axisX = Z;
			axisY = Y;
			transformX *= Matrix::RotationY(90.f);
			transformY *= Matrix::RotationX(90.f);
			break;
		case Direction::UP:
			axisX = X;
			axisY = Z;
			transformX *= Matrix::RotationX(90.f);
			transformY *= Matrix::RotationZ(90.f);
			break;
		case Direction::FORWARD:
			axisX = X;
			axisY = Y;
			transformY *= Matrix::RotationZ(90.f);
			break;
		}

		transformX[3][axisY] = camera.transform.Position()[axisY];
		transformY[3][axisX] = camera.transform.Position()[axisX];

		float halfScreenUnitsX = vpW / 2.f;
		float halfScreenUnitsY = vpH / 2.f;

		float startX = (int)((camera.transform.Position()[axisX] - halfScreenUnitsX) / gap) * gap + offset;
		float startY = (int)((camera.transform.Position()[axisY] - halfScreenUnitsY) / gap) * gap + offset;

		if (camera.transform.Position()[axisX] - halfScreenUnitsX > 0)
			startX += gap;

		if (camera.transform.Position()[axisY] - halfScreenUnitsY > 0)
			startY += gap;

		float maxX = (camera.transform.Position()[axisX] + halfScreenUnitsX);
		float maxY = (camera.transform.Position()[axisY] + halfScreenUnitsY);

		for (float pos = startX; pos < maxX; pos += gap)
		{
			transformX[3][axisX] = pos;
			GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, transformX);
			modelManager.Line().Render();
		}

		for (float pos = startY; pos < maxY; pos += gap)
		{
			transformY[3][axisY] = pos;
			GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, transformY);
			modelManager.Line().Render();
		}
	}

	void DrawLine(const ModelManager &modelManager, const Vector3 &a, const Vector3 &b)
	{
		Transform t;
		t.SetPosition((a + b) / 2);
		t.SetScale(Vector3(0, (b - a).Length(), 0));

		Vector3 dir = (b - a).Normalise();

		if (dir[2] >= 0.f)
			t.SetRotation(Vector3(90 + Maths::ArcSineDegrees(dir[1]), Maths::ArcTangentDegrees2(dir[2], dir[0]), 0.f));
		else
			t.SetRotation(Vector3(90 - Maths::ArcSineDegrees(dir[1]), Maths::ArcTangentDegrees2(dir[2], dir[0]), 0.f));

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		modelManager.Line().Render();
	}
}
