#include "System.hpp"
#include <Windows.h>

inline HCURSOR ECursorToHCURSOR(ECursor cursor)
{
	switch (cursor)
	{
	case ECursor::DEFAULT:
		return ::LoadCursor(NULL, IDC_ARROW);
	case ECursor::HAND:
		return ::LoadCursor(NULL, IDC_HAND);
	case ECursor::ARROWS_VERTICAL:
		return ::LoadCursor(NULL, IDC_SIZENS);
	case ECursor::ARROWS_HORIZONTAL:
		return ::LoadCursor(NULL, IDC_SIZEWE);
	case ECursor::ARROWS_QUAD:
		return ::LoadCursor(NULL, IDC_SIZEALL);
	case ECursor::IBEAM:
		return ::LoadCursor(NULL, IDC_IBEAM);
	}

	return NULL;
}

void System::SetCursor(ECursor cursor)
{
	HCURSOR hCursor = ECursorToHCURSOR(cursor);
	::SetCursor(hCursor);
}
