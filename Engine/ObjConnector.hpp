#pragma once
#include "ObjBox.hpp"
#include "BufferIterator.hpp"
#include "Direction2D.hpp"

class ObjConnector : public ObjBox
{
public:
	GAMEOBJECT_FUNCS(ObjConnector)

	Direction2D direction;
	bool connected;

	ObjConnector() : ObjBox(FLAG_SAVEABLE), direction(Direction2D::NORTH), connected(false) { }
	virtual ~ObjConnector() {}

	virtual void Render() const;

	virtual void WriteToFile(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadFromFile(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual Bounds GetBounds() const override {

		return Bounds();

	}

	bool IsCompatibleWith(const ObjConnector &other) const;
};
