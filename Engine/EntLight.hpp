#pragma once
#include "Entity.hpp"

class EntLight : public Entity
{
	Bounds _bounds;

	Vector3 _colour;
	float _radius;

	static Vector3 _editorBoxExtent;
public:
	Entity_FUNCS(EntLight, EEntityID::LIGHT)

	EntLight() : Entity(EFlags::SAVEABLE | EFlags::ALWAYS_DRAW), _radius(-1.f), _colour(1.f, 1.f, 1.f) { }
	virtual ~EntLight() {}

	static bool drawLightSources;

	const Vector3& GetColour() const { return _colour; }
	float GetRadius() const { return _radius; }

	void SetColour(const Vector3 &colour) { _colour = colour; }
	void SetRadius(const float& radius);

	void Render(RenderQueue&) const override;

	virtual const PropertyCollection& GetProperties() override;

	virtual Bounds GetBounds() const 
	{
		return _bounds;
	}

	//File IO
	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;
};
