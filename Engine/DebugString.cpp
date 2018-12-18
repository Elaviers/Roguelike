#include "DebugString.h"
#include "Font.h"

void DebugString::Render() const
{
	_PreRender();

	_font->RenderString(_string.GetData(), _transform);
}
