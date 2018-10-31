#pragma once
#include "Brush.h"

//TODO - Optimise EVERYTHING to do with this class

class Brush3D : public Brush<3>
{
protected:
	virtual void _OnTransformChanged() override;

	virtual void _UpdateTransform() override;

public:
	Brush3D() {}
	virtual ~Brush3D() {}

	virtual void Render() const override;

	virtual void SaveToFile(BufferIterator<byte> &buffer, const Map<String, uint16> &strings) const override;
	virtual void LoadFromFile(BufferIterator<byte> &buffer, const Map<uint16, String> &strings) override;

	virtual void GetProperties(ObjectProperties&) override;
};
