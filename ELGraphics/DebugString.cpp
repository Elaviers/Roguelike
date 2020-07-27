#include "DebugString.hpp"
#include "Font.hpp"
#include "RenderQueue.hpp"

void DebugString::Render(RenderQueue& q) const
{
	_font->RenderString(q.NewDynamicEntry(ERenderChannels::UNLIT), _string.GetData(), _transform);
}
