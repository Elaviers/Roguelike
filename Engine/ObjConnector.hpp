#pragma once
#include "ObjBox.hpp"
#include "BufferIterator.hpp"
#include "Direction2D.hpp"

class ObjConnector : public ObjBox
{
public:
	GAMEOBJECT_FUNCS(ObjConnector, ObjectIDS::LEVEL_CONNECTOR)

	Direction2D direction;
	bool connected;

	ObjConnector() : ObjBox(FLAG_SAVEABLE), direction(Direction2D::NORTH), connected(false) { }
	virtual ~ObjConnector() {}

	virtual void Render(EnumRenderChannel) const;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual Bounds GetBounds() const override {

		return Bounds();

	}

	bool IsCompatibleWith(const ObjConnector &other) const;
};
