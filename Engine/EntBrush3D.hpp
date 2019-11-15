#pragma once
#include "EntBrush.hpp"

class EntBrush3D : public EntBrush<3>
{
protected:
	virtual void _OnTransformChanged() override;

public:
	Entity_FUNCS(EntBrush3D, EntityIDS::BRUSH)

	EntBrush3D() {}
	virtual ~EntBrush3D() {}

	virtual void Render(EnumRenderChannel) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual const PropertyCollection& GetProperties() override;
};
