#include "CppUnitTest.h"

#include <Engine/EngineInstance.hpp>
#include <Engine/OBrush2D.hpp>
#include <Engine/OBrush3D.hpp>
#include <Engine/OCamera.hpp>
#include <Engine/OConnector.hpp>
#include <Engine/OLight.hpp>
#include <Engine/ORenderable.hpp>
#include <Engine/OSprite.hpp>
#include <Engine/Model.hpp>
#include <Engine/World.hpp>
#include <Engine/WorldObject.hpp>

#include <ELGraphics/Material_Surface.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	EngineInstance engine;

	SharedPointer<Material> material;
	SharedPointer<MaterialSprite> materialSprite;
	SharedPointer<Model> model;

	World world(nullptr);

	TEST_CLASS(WorldObjectTests)
	{
		TEST_CLASS_INITIALIZE(ClassInit)
		{
			engine.Init(EEngineCreateFlags(0)); //Registry only
			MaterialManager* materialManager = engine.pMaterialManager = new MaterialManager();
			ModelManager* modelManager = engine.pModelManager = new ModelManager();

			engine.context.Set(materialManager);
			engine.context.Set(modelManager);

			material = materialManager->Add("dummy", new MaterialSurface());
			materialSprite = materialManager->Add("dummy_sprite", new MaterialSprite()).Cast<MaterialSprite>();
			model = modelManager->Add("dummy", new Model());

			world.SetContext(&engine.context);
		}

	public:

		TEST_METHOD(TestHierarchy)
		{
			WorldObject *o1 = world.CreateObject<WorldObject>(), *o2 = world.CreateObject<WorldObject>(), *o3 = world.CreateObject<WorldObject>();
			o3->SetParent(o2, false);
			o2->SetParent(o1, false);

			Assert::IsTrue(o2->IsChildOf(*o1) && o3->IsChildOf(*o1) && o3->IsChildOf(*o2), L"IsChildOf returned the wrong value");
		}

		template<typename T> void AssertEqual(const T& a, const T& b) {}

		template<>
		void AssertEqual(const WorldObject& a, const WorldObject& b)
		{
			Assert::IsTrue(a.GetName() == b.GetName(), L"Names are not equal");
			Assert::IsTrue(a.GetRelativePosition() == b.GetRelativePosition(), L"Positions are not equal");
			Assert::IsTrue(a.GetRelativeScale() == b.GetRelativeScale(), L"Scales are not equal");
			Assert::IsTrue(
				a.GetRelativeRotation().GetEuler().AlmostEquals(b.GetRelativeRotation().GetEuler(), 0.1f),
				L"Rotations are not similar");
		}

		template<>
		void AssertEqual(const OBrush2D& a, const OBrush2D& b)
		{
			AssertEqual<WorldObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
		}

		template<>
		void AssertEqual(const OBrush3D& a, const OBrush3D& b)
		{
			AssertEqual<WorldObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
		}

		template<>
		void AssertEqual(const OCamera& a, const OCamera& b)
		{
			AssertEqual<WorldObject>(a, b);

			Assert::IsTrue(a.GetProjection().GetType() == b.GetProjection().GetType(), L"Projection types are not equal");
			Assert::IsTrue(a.GetProjection().GetPerspectiveVFOV() == b.GetProjection().GetPerspectiveVFOV(), L"Fields of view are not equal");
			Assert::IsTrue(a.GetProjection().GetOrthographicScale() == b.GetProjection().GetOrthographicScale(), L"Ortho scales are not equal");
			Assert::IsTrue(a.GetProjection().GetNear() == b.GetProjection().GetNear(), L"Near clips are not equal");
			Assert::IsTrue(a.GetProjection().GetFar() == b.GetProjection().GetFar(), L"Far clips are not equal");
			Assert::IsTrue(a.GetProjection().GetDimensions() == b.GetProjection().GetDimensions(), L"Viewports are not equal");
		}

		template<>
		void AssertEqual(const OLight& a, const OLight& b)
		{
			AssertEqual<WorldObject>(a, b);

			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
			Assert::IsTrue(a.GetRadius() == b.GetRadius(), L"Radii are not equal");
		}

		template<>
		void AssertEqual(const ORenderable& a, const ORenderable& b)
		{
			AssertEqual<WorldObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
			Assert::IsTrue(a.GetModel() == b.GetModel(), L"Models are not equal");
			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
		}

		template<>
		void AssertEqual(const OSprite& a, const OSprite& b)
		{
			AssertEqual<WorldObject>(a, b);

			Assert::IsTrue(a.GetMaterial() == b.GetMaterial(), L"Materials are not equal");
			Assert::IsTrue(a.GetSize() == b.GetSize(), L"Sizes are not equal");
			Assert::IsTrue(a.GetColour() == b.GetColour(), L"Colours are not equal");
		}

		template<typename T>
		void CheckRW(T* src, const Context& ctx)
		{
			WorldObject* temp = world.CreateObject<WorldObject>();

			Buffer<byte> buffer;
			ByteWriter writer(buffer);

			src->SetParent(temp, false);
			world.WriteObjects(writer, temp);

			src->SetParent(nullptr, false);

			ByteReader reader(buffer);
			world.ReadObjects(reader, temp);

			AssertEqual<T>((T&)*src, *(T*)*temp->GetChildren().begin());
			
			src->Destroy();
			temp->Destroy();
		}

#define SRC(TYPE) \
		TYPE* src = world.CreateObject<TYPE>();			\
		src->SetRelativeTransform(t);				\
		src->SetName(name);

		TEST_METHOD(TestReadWrite)
		{
			Transform t(Vector3(1.f, 2.f, 3.f), Vector3(4.f, 5.f, 6.f), Vector3(7.f, 8.f, 9.f));
			Text name = Text::FromString("TEST");
			const Colour colour(1.f, 2.f, 3.f, 4.f);

			Context& ctx = engine.context;

			{
				SRC(WorldObject);
				CheckRW(src, ctx);
			}

			{
				SRC(OBrush2D);
				src->SetMaterial(material);
				src->SetLevel(69.f);
				CheckRW(src, ctx);
			}

			{
				SRC(OBrush3D);
				src->SetMaterial(material);
				CheckRW(src, ctx);
			}

			{
				SRC(OCamera);
				src->GetProjection().SetType(EProjectionType::PERSPECTIVE);
				src->GetProjection().SetPerspectiveFOV(110.f);
				src->GetProjection().SetNearFar(33.f, 66.f);
				src->GetProjection().SetDimensions(Vector2T(300, 301));
				CheckRW(src, ctx);
			}

			{
				SRC(OLight);
				src->SetColour(Vector3(1,2,3));
				src->SetRadius(420.f);
				CheckRW(src, ctx);
			}

			{
				SRC(ORenderable);
				src->SetColour(colour);
				src->SetMaterial(material);
				src->SetModel(model, Context::Empty());
				CheckRW(src, ctx);
			}

			{
				SRC(OSprite);
				src->SetColour(colour);
				src->SetMaterial(materialSprite);
				src->SetSize(200.f);
				CheckRW(src, ctx);
			}
		}
	};
}
