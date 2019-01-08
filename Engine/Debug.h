#pragma once

#ifndef _DEBUG
	#define Debug::Assert(UNUSED)
	#define Assert(UNUSED)
#endif

namespace Debug
{
	void Break();

	void PrintLine(const char *message);
	void Error(const char *message);
	void FatalError(const char *message);

#ifdef _DEBUG
	inline void Assert(bool condition, const char *message = "Assertion Failed")
	{
		if (!condition)
		{
			PrintLine(message);
			Break();
			Error(message);
		}
	}
#endif
}
