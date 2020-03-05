#include "CppUnitTest.h"

#include <Engine/Random.hpp>
#include <Engine/Rotation.hpp>
#include <Engine/Vector.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	TEST_CLASS(MathsTests)
	{
	public:
		TEST_METHOD(TestRandom)
		{
			Random r;

			for (int i = 0; i < 100000; ++i)
			{
				float x = r.NextFloat();
				Assert::IsTrue(x >= 0.f && x < 1.f, L"Random::NextFloat returned out of range");
			}

			for (int i = 0; i < 100000; ++i)
			{
				int x = r.Next(5, 6);
				Assert::IsTrue(x >= 5 && x <= 6, L"Random::NextBetween returned out of range");
			}
		}

		TEST_METHOD(TestVectors)
		{
			Assert::IsTrue(Vector3::Dot(Vector3(1, 2, 3), Vector3(10, 9, 8)) == 52.f, L"Vector3::Dot is wrong");

			Vector3 v1(1, 2, 3), v2(v1);
			Assert::IsTrue(v1 + v2 == Vector3(2, 4, 6));
		}

		TEST_METHOD(TestRotations)
		{
			Assert::IsTrue(Quaternion(VectorMaths::V3X, 45.f).ToEuler().AlmostEqual(Vector3(45.f, 0.f, 0.f), .1f));
			Assert::IsTrue(Quaternion(VectorMaths::V3Y, 45.f).ToEuler().AlmostEqual(Vector3(0.f, -45.f, 0.f), .1f));
			Assert::IsTrue(Quaternion(VectorMaths::V3Z, 45.f).ToEuler().AlmostEqual(Vector3(0.f, 0.f, 45.f), .1f));
		}

		TEST_METHOD(TestMatrices)
		{
			Mat4 m = Matrix::Transformation(Vector3(0.f, 1.f, 0.f), Quaternion(Vector3(0, 1, 0), 90.f), Vector3(2, 2, 2));

			Assert::IsTrue((Vector3(1, 0, 0) * m).AlmostEqual(Vector3(0, 1, 2), .1f));
		}
	};
}
