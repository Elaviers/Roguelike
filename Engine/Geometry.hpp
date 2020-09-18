#pragma once
#include <ELCore/NumberedSet.hpp>
#include <ELMaths/Bounds.hpp>
#include <ELPhys/Collider.hpp>

class RenderQueue;

//Represents static geometry
class Geometry
{
protected:
	Geometry() {}

public:
	virtual ~Geometry() {}

	virtual void Render(RenderQueue& q) const {}

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const {}
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) {}

	virtual byte GetTypeID() = 0;

	virtual const Bounds& GetBounds() const { static Bounds bounds; return bounds; }
	virtual EOverlapResult Overlaps(const Collider& collider, const Transform& transform, const Vector3& sweep, Vector3* penOut) const { return EOverlapResult::SEPERATE; }
};
