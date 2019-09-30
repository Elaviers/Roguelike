#include "CppUnitTest.h"
#include <Engine/PropertyCollection.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	TEST_CLASS(CvarTests)
	{
	public:
		TEST_METHOD(TestRelativeCvar)
		{
			class TestClass
			{
				String _private;

			public:
				int a = 0;
				float b = 0.f;

				void SetPrivate(const String& p) { _private = p; }
				const String& GetPrivate() const { return _private; }
			};

			PropertyCollection map;

			map.Add<int>(
				"TEST-A",
				offsetof(TestClass, a));

			map.Add<float>(
				"TEST-B",
				offsetof(TestClass, b));

			map.Add(
				"TEST-PV",
				MemberGetter<TestClass, const String&>(&TestClass::GetPrivate),
				MemberSetter<TestClass, String>(&TestClass::SetPrivate));

			TestClass instance;
			instance.a = 33;
			instance.b = 2.f;
			instance.SetPrivate("asdf");

			int a; float b; String p;

			bool gotA = map.Get<int>(&instance, "TEST-A", a);
			bool gotB = map.Get<float>(&instance, "TEST-B", b);
			bool gotP = map.Get<String>(&instance, "TEST-PV", p);

			Assert::IsTrue(gotA && gotB && gotP && a == instance.a && b == instance.b && p == instance.GetPrivate(), L"PropertyCollection.Get returned unexpected value");

			map.Set<int>(&instance, "TEST-A", 69);
			map.Set<float>(&instance, "TEST-B", 64.f);
			map.Set<String>(&instance, "TEST-PV", "lmao");

			Assert::IsTrue(instance.a == 69 && instance.b == 64.f && instance.GetPrivate() == "lmao", L"PropertyCollection.Set error");
		}
	};
}
