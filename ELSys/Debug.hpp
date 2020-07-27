#pragma once

namespace Debug
{
	void Break();

	void Print(const char* message);
	void PrintLine(const char *message);
	void Error(const char *message);
	void FatalError(const char *message);

	void Message(const char *message, const char *title);

#ifdef _DEBUG
	inline void Assert(bool condition, const char *message = "Assertion Failed")
	{
		if (!condition)
		{
			PrintLine(message);
			Error(message);
			Break();
		}
	}
#else
	__forceinline void Assert(bool condition) {}
	__forceinline void Assert(bool condition, const char* message) {}
#endif

}
