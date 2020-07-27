#include "CppUnitTest.h"

#include <Engine/EngineInstance.hpp>
#include <Engine/Entity.hpp>
#include <Engine/EntBrush2D.hpp>
#include <Engine/EntBrush3D.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntConnector.hpp>
#include <Engine/EntLight.hpp>
#include <Engine/EntRenderable.hpp>
#include <Engine/EntSprite.hpp>
#include <Engine/Model.hpp>

#include <ELGraphics/Material_Surface.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	EngineInstance engine;

	SharedPointer<Material> material;
	SharedPointer<MaterialSprite> materialSprite;
	SharedPointer<Model> model;

	TEST_CLASS(EntityTests)
	{
		TEST_CLASS_INITIALIZE(ClassInit)
		{
			engine.Init(EEngineCreateFlags(0), nullptr); //Registry only
			MaterialManager* materialManager = engine.pMaterialManager = new MaterialManager();
			ModelManager* modelManager = engine.pModelManager = new ModelManager();

			engine.context.Set(materialManager);
			engine.context.Set(modelManager);

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
				a.GetRelativeRotation().GetEuler().AlmostEquals(b.GetRelativeRotation().GetEuler(), 0.1f),
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

			Assert::IsTrue(a.GetProjection().GetType() == b.GetProjection().GetType(), L"Projection types are not equal");
			Assert::IsTrue(a.GetProjection().GetPerspectiveVFOV() == b.GetProjection().GetPerspectiveVFOV(), L"Fields of view are not equal");
			Assert::IsTrue(a.GetProjection().GetOrthographicScale() == b.GetProjection().GetOrthographicScale(), L"Ortho scales are not equal");
			Assert::IsTrue(a.GetProjection().GetNear() == b.GetProjection().GetNear(), L"Near clips are not equal");
			Assert::IsTrue(a.GetProjection().GetFar() == b.GetProjection().GetFar(), L"Far clips are not equal");
			Assert::IsTrue(a.GetProjection().GetDimensions() == b.GetProjection().GetDimensions(), L"Viewports are not equal");
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
		void CheckRW(const T &src, const Context& ctx)
		{
			Buffer<byte> buffer;
			ByteWriter writer(buffer);
			NumberedSet<String> strings;
			src.WriteAllToFile(writer, strings, ctx);

			ByteReader reader(buffer);
			Entity* dest = Entity::CreateFromData(reader, strings, ctx);
			AssertEqual<T>((T&)src, (T&)*dest);
			dest->Delete(ctx);
		}

#define SRC(TYPE) \
		TYPE src;									\
		src.SetFlags(Entity::EFlags::SAVEABLE);		\
		src.SetRelativeTransform(t);				\
		src.SetName(name);
		TEST_METHOD(TestReadWrite)
		{
			Transform t(Vector3(1.f, 2.f, 3.f), Vector3(4.f, 5.f, 6.f), Vector3(7.f, 8.f, 9.f));
			const char* name = "TEST";
			const Colour colour(1.f, 2.f, 3.f, 4.f);

			Context& ctx = engine.context;

			{
				SRC(Entity);
				CheckRW(src, ctx);
			}

			{
				SRC(EntBrush2D);
				src.SetMaterial(material);
				src.level = 69.f;
				CheckRW(src, ctx);
			}

			{
				SRC(EntBrush3D);
				src.SetMaterial(material);
				CheckRW(src, ctx);
			}

			{
				SRC(EntCamera);
				src.GetProjection().SetType(EProjectionType::PERSPECTIVE);
				src.GetProjection().SetPerspectiveFOV(110.f);
				src.GetProjection().SetNearFar(33.f, 66.f);
				src.GetProjection().SetDimensions(Vector2T(300, 301));
				CheckRW(src, ctx);
			}

			{
				SRC(EntLight);
				src.SetColour(Vector3(1,2,3));
				src.SetRadius(420.f);
				CheckRW(src, ctx);
			}

			{
				SRC(EntRenderable);
				src.SetColour(colour);
				src.SetMaterial(material);
				src.SetModel(model, Context::Empty());
				CheckRW(src, ctx);
			}

			{
				SRC(EntSprite);
				src.SetColour(colour);
				src.SetMaterial(materialSprite);
				src.SetSize(200.f);
				CheckRW(src, ctx);
			}
		}
	};
}
