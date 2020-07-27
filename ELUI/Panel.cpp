#include "Panel.hpp"
#include <ELGraphics/Material_Grid.hpp>
#include <ELGraphics/MaterialParam.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void UIPanel::Render(RenderQueue& q) const
{
	if (!_material)
		return;
	
	//todo- priority
	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::UNLIT, 0);
	_colour.Apply(e);

	if (dynamic_cast<const MaterialGrid*>(_material.Ptr()))
	{
		MaterialParam param;
		param.type = MaterialParam::EType::GRID_PARAM;

		param.gridData.row = param.gridData.column = 0;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[0].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
		
		param.gridData.column = 1;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[1].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.column = 2;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[2].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.row = 1;
		param.gridData.column = 0;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[3].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.column = 1;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[4].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.column = 2;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[5].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.row = 2;
		param.gridData.column = 0;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[6].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.column = 1;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[7].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);

		param.gridData.column = 2;
		_material->Apply(e, &param);
		e.AddSetTransform(_transforms[8].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
	else
	{
		_material->Apply(e);
		e.AddSetTransform(_transforms[4].GetTransformationMatrix());
		e.AddCommand(RCMDRenderMesh::PLANE);
	}
}

void UIPanel::_OnBoundsChanged()
{
	float halfBordersize = (float)_borderSize / 2.f;
	float x0 = _absoluteBounds.x + halfBordersize;
	float y0 = _absoluteBounds.y + halfBordersize;
	float centreX = _absoluteBounds.x + _absoluteBounds.w / 2.f;
	float centreY = _absoluteBounds.y + _absoluteBounds.h / 2.f;
	float x2 = _absoluteBounds.x + _absoluteBounds.w - halfBordersize;
	float y2 = _absoluteBounds.y + _absoluteBounds.h - halfBordersize;

	float centreW = _absoluteBounds.w;
	float centreH = _absoluteBounds.h;
	if (_borderSize > 0.f)
	{
		centreW -= _borderSize * 2.f;
		centreH -= _borderSize * 2.f;
	}

	float absBorderSize = Maths::Abs(_borderSize);

	_transforms[0].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[2].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[6].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[8].SetScale(Vector3(absBorderSize, absBorderSize, 1.f));
	_transforms[1].SetScale(Vector3(centreW, absBorderSize, 1.f));
	_transforms[7].SetScale(Vector3(centreW, absBorderSize, 1.f));
	_transforms[3].SetScale(Vector3(absBorderSize, centreH, 1.f));
	_transforms[5].SetScale(Vector3(absBorderSize, centreH, 1.f));
	_transforms[4].SetScale(Vector3(centreW, centreH, 1.f));

	_transforms[0].SetPosition(Vector3(x0,		y2,			_z));
	_transforms[1].SetPosition(Vector3(centreX, y2,			_z));
	_transforms[2].SetPosition(Vector3(x2,		y2,			_z));
	_transforms[3].SetPosition(Vector3(x0,		centreY,	_z));
	_transforms[4].SetPosition(Vector3(centreX, centreY,	_z));
	_transforms[5].SetPosition(Vector3(x2,		centreY,	_z));
	_transforms[6].SetPosition(Vector3(x0,		y0,			_z));
	_transforms[7].SetPosition(Vector3(centreX, y0,			_z));
	_transforms[8].SetPosition(Vector3(x2,		y0,			_z));
}
