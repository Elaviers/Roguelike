#include "CppUnitTest.h"

#include <Engine/Buffer.hpp>
#include <Engine/List.hpp>
#include <Engine/Map.hpp>
#include <Engine/Maths.hpp>
#include <Engine/Hashmap.hpp>
#include <Engine/NumberedSet.hpp>
#include <Engine/String.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

String RandomString()
{
	size_t length = Maths::RandomInRange(1, 1000);
	String string(length);
	
	for (size_t i = 0; i < length; ++i)
		string[i] = (char)Maths::RandomInRange((float)'1', 255.f);

	return string;
}

namespace EngineTests
{


	TEST_CLASS(ContainerTests)
	{
	public:
		
		TEST_METHOD(TestBuffer)
		{
			{
				constexpr int sz = 1000;

				Buffer<int> buffer, buffer2;
				buffer.SetSize(sz);

				//Test assignment / addition
				for (int i = 0; i < sz; ++i)
				{
					buffer[i] = i;
					buffer2.Add(i);
				}

				Assert::IsTrue(buffer.GetSize() == sz, L"Buffer assignment error");
				Assert::IsTrue(buffer2.GetSize() == sz, L"Buffer addition error");

				int* ptr = &buffer[0], * ptr2 = &buffer2[0];
				for (int i = 0; i < sz; ++i)
				{
					Assert::IsTrue(*ptr == i && *ptr == *ptr2, L"Buffer element error");
					++ptr;
					++ptr2;
				}

				for (int i = 0; i < sz; ++i)
				{
					buffer.RemoveIndex(0);
					buffer2.Remove(i);
				}

				Assert::IsTrue(buffer.GetSize() == 0, L"RemoveIndex error");
				Assert::IsTrue(buffer2.GetSize() == 0, L"Remove error");
			}

			{
				float floats[3] = { 1.f, 2.f, 3.f };

				Buffer<float> b1(floats, 3), b2({1.f, 2.f, 3.f});
				Buffer<float> b3(b1), b4 = b1;

				Assert::IsTrue(b1 == b2 && b2 == b3 && b3 == b4, L"Buffer construction/assignment error");

				Buffer<float> b5 = b1 + b2 + b3 + b4 + b5;
				Assert::IsTrue(b5 == Buffer<float>({ 1,2,3,1,2,3,1,2,3,1,2,3 }), L"Buffer addition error");
			}
		}

		TEST_METHOD(TestList)
		{
			List<String> list;
			constexpr int size = 1000;
			constexpr int count = 1;

			for (int i = 0; i < count; ++i)
			{
				list.Clear();

				for (int i = 0; i < size; ++i)
				{
					list += RandomString();
					Logger::WriteMessage(CSTR(String::FromInt(i)));
				}

				Assert::IsTrue(list.GetSize() == size, L"List size is incorrect");

				int itCount = 0;
				for (auto it = list.First(); it; ++it)
					++itCount;

				Assert::IsTrue(itCount == size, L"List iterator error");

				for (int listSize = size; listSize > 0; --listSize)
					list.Remove(list.Get(Maths::RandomInRange(0, listSize - 1)));

				Assert::IsTrue(list.IsEmpty() && list.GetSize() == 0, L"List remove error");
			}
		}

		template<typename T>
		void TestMap(T& map)
		{
			constexpr int size = 1000;
			const int halfSize = size / 2;

			for (int i = 0; i < halfSize; ++i)
			{
				map[i]++;
				map[size - 1 - i]++;
			}

			Assert::IsTrue(map.GetSize() == size, L"Map size is wrong");

			map.ForEach([](const int& k, int& v)
				{
					Assert::IsTrue(v == 1, L"All incremented values are not 1");
				});
		}

		TEST_METHOD(TestMap)
		{
			TestMap(Map<int, int>());
		}

		TEST_METHOD(TestHashmap)
		{
			TestMap(Hashmap<int, int>());
		}

		TEST_METHOD(TestSet)
		{
			NumberedSet<int> set(1);

			set.Add(1);
			set.Add(1);
			set.Add(1);
			set.Add(2);
			set.Add(3);

			Assert::IsTrue(set.IDOf(1) && *set.IDOf(1) == 1, L"NumberedSet ID error");
			Assert::IsTrue(set.IDOf(2) && *set.IDOf(2) == 2, L"NumberedSet ID error");
			Assert::IsTrue(set.IDOf(3) && *set.IDOf(3) == 3, L"NumberedSet ID error");
			Assert::IsTrue(*set.Get(1) == 1 && set[1] == 1);
			Assert::IsTrue(*set.Get(2) == 2 && set[2] == 2);
			Assert::IsTrue(*set.Get(3) == 3 && set[3] == 3);
		}
	};
}
