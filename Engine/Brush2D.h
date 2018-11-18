#pragma once
#include "Brush.h"

class Brush2D : public Brush<2>
{
protected:
	virtual void _OnTransformChanged() override;

	virtual void _UpdateTransform() override;

public:
	float level;

	Brush2D() : level(0.f) { transform.SetRotation(Vector3(-90.f, 0.f, 0.f)); }
	virtual ~Brush2D() {}
	
	virtual void Render() const override;

	virtual void SaveToFile(BufferIterator<byte> &buffer, const Map<String, uint16> &strings) const override;
	virtual void LoadFromFile(BufferIterator<byte> &buffer, const Map<uint16, String> &strings) override;

	virtual void GetProperties(ObjectProperties &) override;
};
