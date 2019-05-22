#include "DebugString.hpp"
#include "Font.hpp"

void DebugString::Render() const
{
	_PreRender();

	_font->RenderString(_string.GetData(), _transform);
}
