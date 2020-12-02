#include "CppUnitTest.h"

#include <ELCore/String.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	TEST_CLASS(CoreTests)
	{

	public:

		TEST_METHOD(Strings)
		{
			//		  0123456789		  0123456789		  012
			//					0123456789			0123456789
			String a("the quick brown fox jumps over the lazy dog");

			Assert::IsTrue(a.IndexOf('t') == 0);
			Assert::IsTrue(a.IndexOf("the") == 0);
			Assert::IsTrue(a.IndexOf("dog") == 40);
			Assert::IsTrue(a.IndexOf("the", 1) == 31);
			Assert::IsTrue(a.IndexOf("brown") == 10);
			Assert::IsTrue(a.IndexOf('c') == 7);
			Assert::IsTrue(a.IndexOf('o', 20) == 26);
			Assert::IsTrue(a.IndexOfAny("oe", 20) == 26);
			Assert::IsTrue(a.LastIndexOf("the") == 31);
			Assert::IsTrue(a.LastIndexOf("dog") == 40);
			Assert::IsTrue(a.LastIndexOfAny("et") == 33);
			Assert::IsTrue(a.LastIndexOf('g') == 42);
			Assert::IsTrue(a.LastIndexOf("the", 20) == 0);

			Assert::IsTrue(a.Replace("the", "a") == "a quick brown fox jumps over a lazy dog");
		}
	};
}
