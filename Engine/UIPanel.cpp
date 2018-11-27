#include "UIPanel.h"
#include "Engine.h"
#include "GLProgram.h"
#include "RenderParam.h"

inline void RenderPlaneWithTransform(const Transform &t) 
{ 
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix()); 
	Engine::modelManager->Plane().Render();
}

void UIPanel::Render() const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);

	RenderParam param;
	param.type = RenderParam::Type::GRID_PARAM;

	if (!_material)
		return;

	param.gridData.row = param.gridData.column = 0;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[0]);
	
	param.gridData.column = 1;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[1]);
	
	param.gridData.column = 2;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[2]);

	param.gridData.row = 1;
	param.gridData.column = 0;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[3]);

	param.gridData.column = 1;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[4]);

	param.gridData.column = 2;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[5]);

	param.gridData.row = 2;
	param.gridData.column = 0;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[6]);

	param.gridData.column = 1;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[7]);

	param.gridData.column = 2;
	_material->Apply(&param);
	RenderPlaneWithTransform(_transforms[8]);
}

void UIPanel::OnBoundsChanged()
{
	float halfBordersize = (float)_borderSize / 2.f;
	float x0 = _x + halfBordersize;
	float y0 = _y + halfBordersize;
	float centreX = _x + _w / 2.f;
	float centreY = _y + _h / 2.f;
	float centreW = _w - _borderSize * 2.f;
	float centreH = _h - _borderSize * 2.f;
	float x2 = _x + _w - halfBordersize;
	float y2 = _y + _h - halfBordersize;

	_transforms[0].SetScale(Vector3(_borderSize, _borderSize, 1.f));
	_transforms[2].SetScale(Vector3(_borderSize, _borderSize, 1.f));
	_transforms[6].SetScale(Vector3(_borderSize, _borderSize, 1.f));
	_transforms[8].SetScale(Vector3(_borderSize, _borderSize, 1.f));
	_transforms[1].SetScale(Vector3(centreW, _borderSize, 1.f));
	_transforms[7].SetScale(Vector3(centreW, _borderSize, 1.f));
	_transforms[3].SetScale(Vector3(_borderSize, centreH, 1.f));
	_transforms[5].SetScale(Vector3(_borderSize, centreH, 1.f));
	_transforms[4].SetScale(Vector3(centreW, centreH, 1.f));

	_transforms[0].SetPosition(Vector3(x0,		y0,			_z));
	_transforms[1].SetPosition(Vector3(centreX, y0,			_z));
	_transforms[2].SetPosition(Vector3(x2,		y0,			_z));
	_transforms[3].SetPosition(Vector3(x0,		centreY,	_z));
	_transforms[4].SetPosition(Vector3(centreX, centreY,	_z));
	_transforms[5].SetPosition(Vector3(x2,		centreY,	_z));
	_transforms[6].SetPosition(Vector3(x0,		y2,			_z));
	_transforms[7].SetPosition(Vector3(centreX, y2,			_z));
	_transforms[8].SetPosition(Vector3(x2,		y2,			_z));
}
