#pragma once
#include "EntBrush.hpp"

class EntBrush2D : public EntBrush<2>
{
protected:
	virtual void _OnTransformChanged() override;

public:
	Entity_FUNCS(EntBrush2D, EntityID::PLANE)

	float level;

	EntBrush2D() : level(0.f) { SetRelativeRotation(Vector3(-90.f, 0.f, 0.f)); }
	virtual ~EntBrush2D() {}
	
	virtual void Render(RenderChannels) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual const PropertyCollection& GetProperties() override;
};
