#include "CppUnitTest.h"

#include <Engine/Engine.hpp>
#include <Engine/Entity.hpp>
#include <Engine/EntBrush2D.hpp>
#include <Engine/EntBrush3D.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntConnector.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <Engine/EntSprite.hpp>

#include <Engine/Material_Surface.hpp>
#include <Engine/Model.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	SharedPointer<Material> material;
	SharedPointer<MaterialSprite> materialSprite;
	SharedPointer<Model> model;

	TEST_CLASS(EntityTests)
	{
		TEST_CLASS_INITIALIZE(ClassInit)
		{
			Engine::Instance().Init(EngineCreateFlags(0), nullptr); //Registry only
			auto materialManager = Engine::Instance().pMaterialManager = new MaterialManager();
			auto modelManager = Engine::Instance().pModelManager = new ModelManager();

			material = materialManager->Add("dummy", new MaterialSurface());
			materialSprite = materialManager->Add("dummy_sprite", new MaterialSprite()).Cast<MaterialSprite>();
			model = modelManager->Add("dummy", new Model());
		}

	public:

		TEST_METHOD(TestHierachy)
		{
			Entity go1, go2, go3;
			Entity* a = &go1, * b = &go2, * c = &go3;

			c->SetParent(b);
			b->SetParent(a);

			Assert::IsTrue(b->IsChildOf(a) && c->IsChildOf(a) && c->IsChildOf(b), L"IsChildOf returned the wrong value");
		}

		template<typename T> void AssertEqual(const T& a, const T& b) {}

		template<>
		void AssertEqual(const Entity& a, const Entity& b)
		{
			Assert::IsTrue(a.GetName() == b.GetName(), L"Names are not equal");
			Assert::IsTrue(a.GetRelativePosition() == b.GetRelativePosition(), L"Positions are not equal");
			Assert::IsTrue(a.GetRelativeScale() == b.GetRelativeScale(), L"Scales are not equal");
			Assert::IsTrue(
				a.GetRelativeRotation().GetEuler().AlmostEqual(b.GetRelativeRotation().GetEuler(), 0.1f),
				L"Rotations are not similar");
		}

		template<>
		void AssertEqual(const EntBrush2D& a, const EntBrush2D& b)
		{
			AssertEqual<Entity>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
		}

		template<>
		void AssertEqual(const EntBrush3D& a, const EntBrush3D& b)
		{
			AssertEqual<Entity>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
		}

		template<>
		void AssertEqual(const EntCamera& a, const EntCamera& b)
		{
			AssertEqual<Entity>(a, b);

			Assert::IsTrue(a.GetProjectionType() == b.GetProjectionType(), L"Projection types are not equal");
			Assert::IsTrue(a.GetFOV() == b.GetFOV(), L"Fields of view are not equal");
			Assert::IsTrue(a.GetScale() == b.GetScale(), L"Ortho scales are not equal");
			Assert::IsTrue(a.GetNear() == b.GetNear(), L"Near clips are not equal");
			Assert::IsTrue(a.GetFar() == b.GetFar(), L"Far clips are not equal");
			Assert::IsTrue(a.GetViewport() == b.GetViewport(), L"Viewports are not equal");
		}

		template<>
		void AssertEqual(const EntConnector& a, const EntConnector& b)
		{
			AssertEqual<Entity>(a, b);

			Assert::IsTrue(a.direction == b.direction, L"Directions are not equal");
		}

		template<>
		void AssertEqual(const EntLight& a, const EntLight& b)
		{
			AssertEqual<Entity>(a, b);

			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
			Assert::IsTrue(a.GetRadius() == b.GetRadius(), L"Radii are not equal");
		}

		template<>
		void AssertEqual(const EntRenderable& a, const EntRenderable& b)
		{
			AssertEqual<Entity>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
			Assert::IsTrue(a.GetModel() == b.GetModel(), L"Models are not equal");
			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
		}

		template<>
		void AssertEqual(const EntSprite& a, const EntSprite& b)
		{
			AssertEqual<Entity>(a, b);

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
			Entity* dest = Entity::CreateFromData(reader, strings);
			AssertEqual<T>((T&)src, (T&)*dest);
			dest->Delete();
		}

#define SRC(TYPE) \
		TYPE src;									\
		src.SetFlags(Entity::Flags::SAVEABLE);		\
		src.SetRelativeTransform(t);				\
		src.SetName(name);
		TEST_METHOD(TestReadWrite)
		{
			Transform t(Vector3(1.f, 2.f, 3.f), Vector3(4.f, 5.f, 6.f), Vector3(7.f, 8.f, 9.f));
			const char* name = "TEST";
			const Colour colour(1.f, 2.f, 3.f, 4.f);

			{
				SRC(Entity);
				CheckRW(src);
			}

			{
				SRC(EntBrush2D);
				src.SetMaterial(material);
				src.level = 69.f;
				CheckRW(src);
			}

			{
				SRC(EntBrush3D);
				src.SetMaterial(material);
				CheckRW(src);
			}

			{
				SRC(EntCamera);
				src.SetProjectionType(ProjectionType::PERSPECTIVE);
				src.SetFOV(110.f);
				src.SetScale(69.f);
				src.SetZBounds(33.f, 66.f);
				src.SetViewport(300, 301);
				CheckRW(src);
			}

			{
				SRC(EntConnector);
				src.direction = Direction2D::SOUTH;
				src.connected = true;
				CheckRW(src);
			}

			{
				SRC(EntLight);
				src.SetColour(Vector3(1,2,3));
				src.SetRadius(420.f);
				CheckRW(src);
			}

			{
				SRC(EntRenderable);
				src.SetColour(colour);
				src.SetMaterial(material);
				src.SetModel(model);
				CheckRW(src);
			}

			{
				SRC(EntSprite);
				src.SetColour(colour);
				src.SetMaterial(materialSprite);
				src.SetSize(200.f);
				CheckRW(src);
			}
		}
	};
}
