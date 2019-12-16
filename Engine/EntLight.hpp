#pragma once
#include "Entity.hpp"
#include "Collider.hpp"
#include "Vector.hpp"

class ModelManager;

class EntLight : public Entity
{
	Vector3 _colour;
	float _radius;

	static int _frameLightCounter;

	static Vector3 _editorBoxExtent;
public:
	Entity_FUNCS(EntLight, EntityID::LIGHT)

	EntLight() : Entity(Flags::SAVEABLE), _radius(-1.f), _colour(1.f, 1.f, 1.f) { }
	virtual ~EntLight() {}

	static bool drawLightSources;

	static void FinaliseLightingForFrame();

	const Vector3& GetColour() const { return _colour; }
	float GetRadius() const { return _radius; }

	void SetColour(const Vector3 &colour) { _colour = colour; }
	void SetRadius(float radius) { _radius = radius; }

	void ToShader(int glArrayIndex) const;

	void Update(float deltaTime) override;
	void Render(RenderChannels) const override;

	virtual const PropertyCollection& GetProperties() override;

	virtual Bounds GetBounds() const 
	{
		static Bounds bounds(_editorBoxExtent);
		return bounds;
	}

	virtual const Collider* GetCollider() const 
	{
		static Collider editorCollider(CollisionChannels::EDITOR, Box(_editorBoxExtent));
		return &editorCollider;
	}

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};
