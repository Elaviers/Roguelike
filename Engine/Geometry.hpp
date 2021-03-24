#pragma once
#include <ELCore/NumberedSet.hpp>
#include <ELPhys/Collider.hpp>
#include "EGeometryID.hpp"
#include "OGeometryCollection.hpp"

class RenderQueue;
class Volume;

#define GEOMETRY_VFUNCS(CLASSNAME, ID)									\
public:																	\
	friend OGeometryCollection;											\
	static const EGeometryID TypeID = ID;								\
	virtual EGeometryID GetTypeID() const override { return TypeID; }			\
	virtual const char* GetClassString() const override { return #CLASSNAME; }

//Represents static geometry
class Geometry
{
private:
	OGeometryCollection* _collection;
	uint32 _uid;

protected:
	Geometry(OGeometryCollection& collection) : _collection(&collection), _uid(collection.TakeNextUID()) {}
	Geometry(const Geometry& other) : _collection(other._collection), _uid(other._collection->TakeNextUID()) {}
	Geometry(Geometry&& other) noexcept = delete;

	Geometry& operator=(const Geometry& other) = delete;
	Geometry& operator=(Geometry&& other) noexcept = delete;
	
public:
	virtual ~Geometry() {}

	OGeometryCollection* GetOwner() { return _collection; }
	const OGeometryCollection* GetOwner() const { return _collection; }
	uint32 GetUID() const { return _uid; }

	virtual void Render(RenderQueue& q) const {}

	virtual void Read(ByteReader&, ObjectIOContext& ctx) {}
	virtual void Write(ByteWriter&, ObjectIOContext& ctx) const {}

	virtual EGeometryID GetTypeID() const = 0;
	virtual const char* GetClassString() const = 0;

	//Note: for now, all geometry volumes are processed in world space!!!
	virtual const Volume& GetVolume() const = 0;
};
