#include "CppUnitTest.h"

#include <ELCore/Buffer.hpp>
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/String.hpp>
#include <ELCore/IEEE754.hpp>

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

		TEST_METHOD(ByteRW)
		{
			Buffer<byte> bb;
			ByteWriter w(bb);

			const int ints1_c = 9;
			int32 ints1[ints1_c] = { 4, -20, 39, 100424849, -266, 23, 549, 549, 0 };
			for (int i = 0; i < ints1_c; ++i)
				w.Write_uint32(ints1[i]);

			const int floats1_c = 5;
			float floats1[floats1_c] = { 0.0002463f, -1023.3995882784f, 0.f, -12993437500.f, 39.00034238472634673f };
			for (int i = 0; i < floats1_c; ++i)
				w.Write_float(floats1[i]);

			const int doubles1_c = 5;
			double doubles1[doubles1_c] = { 0.0000000334455827732664, 20000.334775999000038832, 0, -124775858587457.33333, 12.000000000001 };
			for (int i = 0; i < doubles1_c; ++i)
				w.Write_double(doubles1[i]);

			const int strings1_c = 4;
			const String strings1[strings1_c] = {"", "Blah blah blah blah blah", "", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab01"};
			for (int i = 0; i < strings1_c; ++i)
				if (i > 1)
					w.Write_cstr(strings1[i].GetData());
				else
					w.Write(strings1[i]);

			ByteReader r1(bb);
			ByteReader r2(bb.Elements(), bb.GetSize());

			for (int i = 0; i < ints1_c; ++i)
			{
				Assert::AreEqual(ints1[i], (int32)r1.Read_uint32());
				Assert::AreEqual(ints1[i], (int32)r2.Read_uint32());
			}

			for (int i = 0; i < floats1_c; ++i)
			{
				Assert::AreEqual(floats1[i], r1.Read_float());
				Assert::AreEqual(floats1[i], r2.Read_float());
			}

			for (int i = 0; i < doubles1_c; ++i)
			{
				Assert::AreEqual(doubles1[i], r1.Read_double());
				Assert::AreEqual(doubles1[i], r2.Read_double());
			}
			
			for (int i = 0; i < strings1_c; ++i)
			{
				Assert::IsTrue(strings1[i] == r1.Read<String>());
				Assert::IsTrue(strings1[i] == r2.Read<String>());
			}

			Buffer<byte> bb2;
			bb2.SetSize(bb.GetSize());
			w = ByteWriter(bb2.Elements(), bb2.GetSize());

			for (int i = 0; i < ints1_c; ++i)
				w.Write_uint32(ints1[i]);

			for (int i = 0; i < floats1_c; ++i)
				w.Write_float(floats1[i]);

			for (int i = 0; i < doubles1_c; ++i)
				w.Write_double(doubles1[i]);

			for (int i = 0; i < strings1_c; ++i)
				if (i > 1)
					w.Write_cstr(strings1[i].GetData());
				else
					w.Write(strings1[i]);

			Assert::IsFalse(w.Write_byte(42));
			Assert::IsTrue(bb == bb2, L"ByteWriter: Inconsistency between static and dynamic");
		}

		TEST_METHOD(IEEE754)
		{
			for (float f = -200; f < 200; f += 0.01f)
			{
				Float_IEEE754 binary32 = Float_IEEE754::FromFloat_ForceConversion(f);
				float reconv = binary32.ToFloat_ForceConversion();

				Assert::AreEqual(f, reconv);
			}

			for (double d = -200; d < 200; d += 0.01)
			{
				Double_IEEE754 binary64 = Double_IEEE754::FromDouble_ForceConversion(d);
				double reconv = binary64.ToDouble_ForceConversion();

				Assert::AreEqual(d, reconv);
			}
		}
	};
}
