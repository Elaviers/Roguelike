#pragma once
#include <ELCore/NumberedSet.hpp>

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
};
