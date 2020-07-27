#include "Rect.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>

void UIRect::_OnBoundsChanged()
{
	_transform.SetPosition(Vector3(_absoluteBounds.x + _absoluteBounds.w / 2.f, _absoluteBounds.y + _absoluteBounds.h / 2.f, _z));
	_transform.SetScale(Vector3(_absoluteBounds.w, _absoluteBounds.h, 0.f));
}

void UIRect::Render(RenderQueue& q) const
{
	//todo- priority
	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::UNLIT, 0);
	_colour.Apply(e);

	if (_texture)
		e.AddSetTexture(*_texture, 0);
	else
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	
	e.AddSetTransform(_transform.GetTransformationMatrix());
	e.AddCommand(RCMDSetUVOffset::Default());
	e.AddCommand(RCMDSetUVScale::Default());
	e.AddCommand(RCMDRenderMesh::PLANE);
}
