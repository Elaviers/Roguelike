#pragma once
#include "Entity.hpp"
#include "ColliderBox.hpp"
#include "Vector.hpp"

class ModelManager;

class EntLight : public Entity
{
	Vector3 _colour;
	float _radius;

	const static Vector3 _editorBoxExtent;
	const static ColliderBox _lightCollider;
public:
	Entity_FUNCS(EntLight, EntityID::LIGHT)

	EntLight() : Entity(FLAG_SAVEABLE), _radius(-1.f), _colour(1.f, 1.f, 1.f) { }
	virtual ~EntLight() {}

	static bool drawLightSources;

	const Vector3& GetColour() const { return _colour; }
	float GetRadius() const { return _radius; }

	void SetColour(const Vector3 &colour) { _colour = colour; }
	void SetRadius(float radius) { _radius = radius; }

	void ToShader(int glArrayIndex);

	void Render(RenderChannels) const override;

	virtual const PropertyCollection& GetProperties() override;

	virtual Bounds GetBounds() const { return Bounds(_editorBoxExtent); }
	virtual const Collider* GetCollider() const { return &_lightCollider; }

	//File IO
	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const override;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings) override;
};
