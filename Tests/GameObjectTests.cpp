#include "CppUnitTest.h"

#include <Engine/Engine.hpp>
#include <Engine/GameObject.hpp>
#include <Engine/ObjBrush2D.hpp>
#include <Engine/ObjBrush3D.hpp>
#include <Engine/ObjCamera.hpp>
#include <Engine/ObjConnector.hpp>
#include <Engine/ObjLight.hpp>
#include <Engine/ObjRenderable.hpp>
#include <Engine/ObjSprite.hpp>

#include <Engine/Material_Surface.hpp>
#include <Engine/Model.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	Material* material;
	MaterialSprite* materialSprite;
	Model* model;

	TEST_CLASS(GameObjectTests)
	{
		TEST_CLASS_INITIALIZE(ClassInit)
		{
			Engine::Instance().Init(EngineCreateFlags(0)); //Registry only
			Engine::Instance().pMaterialManager = new MaterialManager();
			Engine::Instance().pModelManager = new ModelManager();

			material = new MaterialSurface();
			materialSprite = new MaterialSprite();
			model = new Model();

			Engine::Instance().pMaterialManager->Add("dummy", material);
			Engine::Instance().pMaterialManager->Add("dummy_sprite", materialSprite);
			Engine::Instance().pModelManager->Add("dummy", model);
		}

	public:

		TEST_METHOD(TestHierachy)
		{
			GameObject go1, go2, go3;
			GameObject* a = &go1, * b = &go2, * c = &go3;

			c->SetParent(b);
			b->SetParent(a);

			Assert::IsTrue(b->IsChildOf(a) && c->IsChildOf(a) && c->IsChildOf(b), L"IsChildOf returned the wrong value");
		}

		template<typename T> void AssertEqual(const T& a, const T& b) {}

		template<>
		void AssertEqual(const GameObject& a, const GameObject& b)
		{
			Assert::IsTrue(a.GetName() == b.GetName(), L"Names are not equal");
			Assert::IsTrue(a.GetRelativePosition() == b.GetRelativePosition(), L"Positions are not equal");
			Assert::IsTrue(a.GetRelativeScale() == b.GetRelativeScale(), L"Scales are not equal");
			Assert::IsTrue(
				a.GetRelativeRotation().GetEuler().AlmostEqual(b.GetRelativeRotation().GetEuler(), 0.1f),
				L"Rotations are not similar");
		}

		template<>
		void AssertEqual(const ObjBrush2D& a, const ObjBrush2D& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
		}

		template<>
		void AssertEqual(const ObjBrush3D& a, const ObjBrush3D& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
		}

		template<>
		void AssertEqual(const ObjCamera& a, const ObjCamera& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.GetProjectionType() == b.GetProjectionType(), L"Projection types are not equal");
			Assert::IsTrue(a.GetFOV() == b.GetFOV(), L"Fields of view are not equal");
			Assert::IsTrue(a.GetScale() == b.GetScale(), L"Ortho scales are not equal");
			Assert::IsTrue(a.GetNear() == b.GetNear(), L"Near clips are not equal");
			Assert::IsTrue(a.GetFar() == b.GetFar(), L"Far clips are not equal");
			Assert::IsTrue(a.GetViewport() == b.GetViewport(), L"Viewports are not equal");
		}

		template<>
		void AssertEqual(const ObjConnector& a, const ObjConnector& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.direction == b.direction, L"Directions are not equal");
		}

		template<>
		void AssertEqual(const ObjLight& a, const ObjLight& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
			Assert::IsTrue(a.GetRadius() == b.GetRadius(), L"Radii are not equal");
		}

		template<>
		void AssertEqual(const ObjRenderable& a, const ObjRenderable& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
			Assert::IsTrue(a.GetModel() == b.GetModel(), L"Models are not equal");
			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
		}

		template<>
		void AssertEqual(const ObjSprite& a, const ObjSprite& b)
		{
			AssertEqual<GameObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
			Assert::IsTrue(a.GetSize() == b.GetSize(), L"Sizes are not equal");
			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
		}

		template<typename T>
		void CheckRW(const T &src)
		{
			Buffer<byte> buffer;
			BufferWriter<byte> writer(buffer);
			NumberedSet<String> strings;
			src.WriteAllToFile(writer, strings);

			BufferReader<byte> reader(buffer);
			GameObject* dest = GameObject::CreateFromData(reader, strings);
			AssertEqual<T>((T&)src, (T&)*dest);
			dest->Delete();
		}

#define SRC(TYPE) \
		TYPE src;									\
		src.SetFlags(GameObject::FLAG_SAVEABLE);	\
		src.SetRelativeTransform(t);				\
		src.SetName(name);
		TEST_METHOD(TestReadWrite)
		{
			Transform t(Vector3(1.f, 2.f, 3.f), Vector3(4.f, 5.f, 6.f), Vector3(7.f, 8.f, 9.f));
			const char* name = "TEST";
			const Vector4 colour(1.f, 2.f, 3.f, 4.f);

			{
				SRC(GameObject);
				CheckRW(src);
			}

			{
				SRC(ObjBrush2D);
				src.SetMaterial(material);
				src.level = 69.f;
				CheckRW(src);
			}

			{
				SRC(ObjBrush3D);
				src.SetMaterial(material);
				CheckRW(src);
			}

			{
				SRC(ObjCamera);
				src.SetProjectionType(ProjectionType::PERSPECTIVE);
				src.SetFOV(110.f);
				src.SetScale(69.f);
				src.SetZBounds(33.f, 66.f);
				src.SetViewport(300, 301);
				CheckRW(src);
			}

			{
				SRC(ObjConnector);
				src.direction = Direction2D::SOUTH;
				src.connected = true;
				CheckRW(src);
			}

			{
				SRC(ObjLight);
				src.SetColour(Vector3(1,2,3));
				src.SetRadius(420.f);
				CheckRW(src);
			}

			{
				SRC(ObjRenderable);
				src.SetColour(colour);
				src.SetMaterial(material);
				src.SetModel(model);
				CheckRW(src);
			}

			{
				SRC(ObjSprite);
				src.SetColour(colour);
				src.SetMaterial(materialSprite);
				src.SetSize(200.f);
				CheckRW(src);
			}
		}
	};
}
