#include "DebugLine.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"

void DebugLine::Render(RenderQueue& q) const
{
	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);
	e.AddSetColour(_colour);
	e.AddSetLineWidth(_lineWidth);
	e.AddLine(_a, _b);
}
