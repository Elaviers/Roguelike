#pragma once
#include <ELCore/NumberedSet.hpp>
#include <ELMaths/Bounds.hpp>
#include <ELPhys/Collider.hpp>

class RenderQueue;

//Represents static geometry
class Geometry
{
protected:
	static uint32 _TakeNextUID()
	{
		static uint32 uid = 1;
		return uid++;
	}

	const uint32 _uid;
	
	Geometry() : _uid(_TakeNextUID()) {}
	Geometry(const Geometry& other) : _uid(_TakeNextUID()) {}
	Geometry(Geometry&& other) noexcept : _uid(other._uid) {}

	Geometry& operator=(const Geometry& other) { return *this; }
	Geometry& operator=(Geometry&& other) noexcept { const_cast<uint32&>(_uid) = other._uid; return *this; }
	
public:
	virtual ~Geometry() {}

	uint32 GetUID() const { return _uid; }

	virtual void Render(RenderQueue& q) const {}

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const {}
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) {}

	virtual byte GetTypeID() = 0;

	virtual const Bounds& GetBounds() const { static Bounds bounds; return bounds; }
	virtual EOverlapResult Overlaps(const Collider& collider, const Transform& transform, const Vector3& sweep, Vector3* penOut) const { return EOverlapResult::SEPERATE; }
};
