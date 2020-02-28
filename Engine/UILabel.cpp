#include "UILabel.hpp"
#include "GLProgram.hpp"

void UILabel::_UpdateShadowTransform()
{
	_shadowTransform = _transform;
	_shadowTransform.Move(Vector3(_shadowOffset[0], _shadowOffset[1], 0.f));
}

void UILabel::Render() const
{
	if (_font)
	{
		if (_shadowOffset[0] != 0.f && _shadowOffset[1] != 0.f)
		{
			_shadowColour.ApplyToShader();
			_font->RenderString(_string.GetData(), _shadowTransform);
		}

		_colour.ApplyToShader();
		_font->RenderString(_string.GetData(), _transform);
		GLProgram::Current().SetBool(DefaultUniformVars::boolBlend, false);
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
