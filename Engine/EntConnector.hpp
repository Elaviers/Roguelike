#pragma once
#include "EntBox.hpp"
#include "BufferIterator.hpp"
#include "EDirection2D.hpp"

class EntConnector : public EntBox
{
public:
	Entity_FUNCS(EntConnector, EEntityID::LEVEL_CONNECTOR)

	EDirection2D direction;
	bool connected;

	EntConnector() : EntBox(EFlags::SAVEABLE), direction(EDirection2D::NORTH), connected(false) { }
	virtual ~EntConnector() {}

	virtual void Render(ERenderChannels) const;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual Bounds GetBounds() const override {

		return Bounds();

	}

	bool IsCompatibleWith(const EntConnector &other) const;
};
