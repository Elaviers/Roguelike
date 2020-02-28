#include "Engine.hpp"
#include <Windows.h>

//static
void Engine::UseCursor(ECursor cursor)
{
	switch (cursor)
	{
	case ECursor::DEFAULT:
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		break;
	case ECursor::HAND:
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
		break;
	case ECursor::ARROWS_VERTICAL:
		::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
		break;
	case ECursor::ARROWS_HORIZONTAL:
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
		break;
	case ECursor::ARROWS_QUAD:
		::SetCursor(::LoadCursor(NULL, IDC_SIZEALL));
		break;
	case ECursor::IBEAM:
		::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
		break;
	}
}
