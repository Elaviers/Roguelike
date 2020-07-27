#include "Colour.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderEntry.hpp>

void UIColour::Apply(RenderEntry& e) const
{
	if (_isBlend)
		e.AddSetColour(_blendFrom, _blendTo);
	else
		e.AddSetColour(_colour);
}
