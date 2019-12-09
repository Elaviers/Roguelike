#pragma once
#include "EntBox.hpp"
#include "BufferIterator.hpp"
#include "Direction2D.hpp"

class EntConnector : public EntBox
{
public:
	Entity_FUNCS(EntConnector, EntityID::LEVEL_CONNECTOR)

	Direction2D direction;
	bool connected;

	EntConnector() : EntBox(FLAG_SAVEABLE), direction(Direction2D::NORTH), connected(false) { }
	virtual ~EntConnector() {}

	virtual void Render(RenderChannels) const;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual Bounds GetBounds() const override {

		return Bounds();

	}

	bool IsCompatibleWith(const EntConnector &other) const;
};
