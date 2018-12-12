#pragma once
#include "Box.h"
#include "BufferIterator.h"
#include "Direction2D.h"

class Connector : public Box
{
public:
	Connector() : direction(Direction2D::NORTH) {}
	virtual ~Connector() {}

	Direction2D direction;

	virtual void Render() const;

	void SaveToFile(BufferIterator<byte>&) const;
	void LoadFromFile(BufferIterator<byte>&);

	bool IsCompatibleWith(const Connector &other) const;
};
