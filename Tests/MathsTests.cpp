#include "CppUnitTest.h"
#include <ELMaths/Matrix4.hpp>
#include <ELMaths/Random.hpp>
#include <ELMaths/Rotation.hpp>
#include <ELMaths/Vector2.hpp>
#include <ELMaths/Vector3.hpp>
#include <ELMaths/Vector4.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	TEST_CLASS(MathsTests)
	{
	public:
		TEST_METHOD(TestRandom)
		{
			Random r(32);

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
			Assert::IsTrue(Vector3(1, 2, 3).Dot(Vector3(10, 9, 8)) == 52.f, L"Vector3::Dot is wrong");

			Vector3 v1(1, 2, 3), v2(v1);
			Assert::IsTrue(v1 + v2 == Vector3(2, 4, 6));
		}

		TEST_METHOD(TestRotations)
		{
			Assert::IsTrue(Quaternion(VectorMaths::V3X, 45.f).ToEulerYXZ().AlmostEquals(Vector3(45.f, 0.f, 0.f), .1f));
			Assert::IsTrue(Quaternion(VectorMaths::V3Y, 45.f).ToEulerYXZ().AlmostEquals(Vector3(0.f, -45.f, 0.f), .1f));
			Assert::IsTrue(Quaternion(VectorMaths::V3Z, 45.f).ToEulerYXZ().AlmostEquals(Vector3(0.f, 0.f, 45.f), .1f));
		}

		TEST_METHOD(TestMatrices)
		{
			Matrix4 m = Matrix4::Transformation(Vector3(0.f, 1.f, 0.f), Quaternion(Vector3(0, 1, 0), 90.f), Vector3(2, 2, 2));

			Assert::IsTrue((Vector4(1, 0, 0, 1) * m).GetXYZ().AlmostEquals(Vector3(0, 1, 2), .1f));


			//LU decomposition
			const int testCount = 4;
			const Matrix4 testMatrices[testCount] =
			{
				Matrix4(1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1),

				Matrix4(30, -2, .42, 1,
						0.2, -239, 39, 522,
						-111, 0, .001, 2.2,
						-1, 999, 2, -0.8),

				Matrix4(1, 2, 3, 4,
						2, .5, 2, 3,
						3, 2, 0, 2,
						4, 3, 2, -1),

				Matrix4(.02, 0, 0, 0,
						.0024, .1, -0.98, 0.01,
						.000084, .345, .01, .244,
						.012, -.244, .2, -0.24),
			};

			for (int i = 0; i < testCount; ++i)
			{
				Matrix4 lu_permutated;
				int permutation[4];
				Assert::IsTrue(testMatrices[i].DecomposeLU(lu_permutated, permutation));

				Matrix4 l, u;
				for (int r = 0; r < 4; ++r)
				{
					for (int c = 0; c < 4; ++c)
					{
						if (r > c)
							l[r][c] = lu_permutated[r][c];
						else
							u[r][c] = lu_permutated[r][c];
					}
				}

				m = (l * u).UndoRowPermutation(permutation);
				for (int r = 0; r < 4; ++r)
					for (int c = 0; c < 4; ++c)
						Assert::IsTrue(Maths::AlmostEquals(m[r][c], testMatrices[i][r][c], 0.01f), L"LU decomposition failure");

				m = testMatrices[i] * testMatrices[i].Inverse();
				for (int r = 0; r < 4; ++r)
					for (int c = 0; c < 4; ++c)
						Assert::IsTrue(Maths::AlmostEquals(m[r][c], Matrix4::Identity()[r][c], 0.01f), L"Matrix inversion failure");

				m = testMatrices[i] * testMatrices[i].Inverse3x3();
				for (int r = 0; r < 3; ++r)
					for (int c = 0; c < 3; ++c)
						Assert::IsTrue(Maths::AlmostEquals(m[r][c], Matrix4::Identity()[r][c], 0.01f), L"3x3 matrix inversion failure");
			}
		}
	};
}
