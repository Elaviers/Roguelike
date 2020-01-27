#include "DebugLine.hpp"
#include "DrawUtils.hpp"
#include "Engine.hpp"

void DebugLine::Render() const
{
	_PreRender();

	if (Engine::Instance().pModelManager)
		DrawUtils::DrawLine(*Engine::Instance().pModelManager, _a, _b);
}
