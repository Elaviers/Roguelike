#include "UILabel.h"
#include "GLProgram.h"

void UILabel::Render() const
{
	if (_font)
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
		_font->RenderString(_string.GetData(), _transform);
	}
}

void UILabel::OnBoundsChanged()
{
	float x = _x + _w / 2.f - _font->CalculateStringWidth(_string.GetData(), _h) / 2.f;
	_transform.SetPosition(Vector3(x, _y, _z));
	_transform.SetScale(Vector3(_h, _h, 1.f));
}
