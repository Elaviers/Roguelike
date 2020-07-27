#include "Label.hpp"
#include <ELGraphics/RenderQueue.hpp>

void UILabel::_UpdateShadowTransform()
{
	_shadowTransform = _transform;
	_shadowTransform.Move(Vector3(_shadowOffset.x, _shadowOffset.y, 0.f));
}

void UILabel::Render(RenderQueue& q) const
{
	if (_font)
	{
		//todo- priority
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::UNLIT);

		if (_shadowOffset.x != 0.f && _shadowOffset.y != 0.f)
		{
			_shadowColour.Apply(e);
			_font->RenderString(e, _string.GetData(), _shadowTransform);
		}

		_colour.Apply(e);
		_font->RenderString(e, _string.GetData(), _transform);
	}
}

void UILabel::_OnBoundsChanged()
{
	if (_font)
	{
		float x = _absoluteBounds.x + _absoluteBounds.w / 2.f - _font->CalculateStringWidth(_string.GetData(), _absoluteBounds.h) / 2.f;
		_transform.SetPosition(Vector3(x, _absoluteBounds.y, _z));
		_transform.SetScale(Vector3(_absoluteBounds.h, _absoluteBounds.h, 1.f));
	}

	_UpdateShadowTransform();
}
