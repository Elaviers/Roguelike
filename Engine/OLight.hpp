#pragma once
#include "WorldObject.hpp"

class OLight : public WorldObject
{
	Vector3 _colour;
	float _radius;

	static Vector3 _editorBoxExtent;

protected:
	OLight(World& world) : WorldObject(world), _colour(1.f, 1.f, 1.f), _radius(-1.f) {}

public:
	WORLDOBJECT_VFUNCS(OLight, EObjectID::LIGHT);

	virtual ~OLight() {}

	static bool drawLightSources;

	const Vector3& GetColour() const { return _colour; }
	float GetRadius() const { return _radius; }

	void SetColour(const Vector3 &colour) { _colour = colour; }
	void SetRadius(const float& radius);

	void Render(RenderQueue&) const override;

	virtual const PropertyCollection& GetProperties() override;

	virtual bool IsVisible(const Frustum& view) const override;

	//File IO
	virtual void Read(ByteReader& buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter& buffer, ObjectIOContext& ctx) const override;
};
