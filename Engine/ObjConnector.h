#pragma once
#include "ObjBox.h"
#include "BufferIterator.h"
#include "Direction2D.h"

class ObjConnector : public ObjBox
{
public:
	GAMEOBJECT_FUNCS(ObjConnector)

	Direction2D direction;
	bool connected;

	ObjConnector() : ObjBox(FLAG_SAVEABLE), direction(Direction2D::NORTH), connected(false) {}
	virtual ~ObjConnector() {}

	virtual void Render() const;

	virtual void WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings) override;

	bool IsCompatibleWith(const ObjConnector &other) const;
};
