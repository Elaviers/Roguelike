#pragma once
#include "WorldObject.hpp"
#include <ELMaths/Volume.hpp>

class Geometry;

class OGeometryCollection : public WorldObject
{
private:
	uint32 _nextUID;

	List<Geometry*> _geometry;

	Frustum _lastViewFrustum;

protected:
	OGeometryCollection(World& world);
	OGeometryCollection(const OGeometryCollection& other);

public:
	WORLDOBJECT_VFUNCS(OGeometryCollection, EObjectID::GEOMETRY);

	virtual ~OGeometryCollection();

	uint32 TakeNextUID() { return _nextUID++; }
	const List<Geometry*>& GetGeometry() const { return _geometry; }

	template <typename T>
	requires Concepts::DerivedFrom<T, Geometry>
	T* CreateGeometry() 
	{
		T* g = new T(*this);
		_geometry.Add(g);
		return g;
	}

	void DeleteGeometry(Geometry* geometry);

	virtual void Render(RenderQueue&) const override;
	virtual bool IsVisible(const Frustum& view) const override;

	virtual const Volume* GetVolume() const override { return nullptr; }

	virtual void Read(ByteReader& buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter& buffer, ObjectIOContext& ctx) const override;

	virtual void RenderID(RenderQueue&, uint32 red) const;

	Geometry* DecodeIDMapValue(uint32 red, uint32 green) const;
};
