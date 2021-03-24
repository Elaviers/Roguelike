#pragma once
#include "EGeometryID.hpp"
#include "Register.hpp"
#include <ELCore/String.hpp>
#include <ELCore/Function.hpp>

class Geometry;
class OGeometryCollection;

struct GeometryType
{
private:
	EGeometryID _id;
	String _name;
	MemberFunction<OGeometryCollection, Geometry*> _create;

public:
	GeometryType(EGeometryID id, const String& name, const MemberFunction<OGeometryCollection, Geometry*>& create) :
		_id(id), _name(name), _create(create) {}

	EGeometryID GetID() const { return _id; }
	const String& GetName() const { return _name; }

	Geometry* New(OGeometryCollection& owner) const { return _create.Call(owner); }
};

class GeometryRegister : public Register<GeometryType, EGeometryID>
{
protected:
	virtual EGeometryID _GetTypeID(const GeometryType& type) const override { return type.GetID(); }
	virtual String _GetTypeName(const GeometryType& type) const override { return type.GetName(); }
};
