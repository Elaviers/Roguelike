#pragma once

namespace Debug
{
	void Break();

	void Print(const char* message);
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
#else
	inline void Assert(bool condition, const char* message) {}
#endif

}
