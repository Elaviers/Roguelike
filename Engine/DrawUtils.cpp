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

		transformX[3][axisY] = camera.transform.GetPosition()[axisY];
		transformY[3][axisX] = camera.transform.GetPosition()[axisX];

		float halfScreenUnitsX = vpW / 2.f;
		float halfScreenUnitsY = vpH / 2.f;

		float startX = (int)((camera.transform.GetPosition()[axisX] - halfScreenUnitsX) / gap) * gap + offset;
		float startY = (int)((camera.transform.GetPosition()[axisY] - halfScreenUnitsY) / gap) * gap + offset;

		if (camera.transform.GetPosition()[axisX] - halfScreenUnitsX > 0)
			startX += gap;

		if (camera.transform.GetPosition()[axisY] - halfScreenUnitsY > 0)
			startY += gap;

		float maxX = (camera.transform.GetPosition()[axisX] + halfScreenUnitsX);
		float maxY = (camera.transform.GetPosition()[axisY] + halfScreenUnitsY);

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

}
