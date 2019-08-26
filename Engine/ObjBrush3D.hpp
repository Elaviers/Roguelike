#pragma once
#include "ObjBrush.hpp"

//TODO - Optimise EVERYTHING to do with this class

class ObjBrush3D : public ObjBrush<3>
{
protected:
	virtual void _OnTransformChanged() override;

public:
	GAMEOBJECT_FUNCS(ObjBrush3D, ObjectIDS::BRUSH)

	ObjBrush3D() {}
	virtual ~ObjBrush3D() {}

	virtual void Render(EnumRenderChannel) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual void GetCvars(CvarMap&) override;
};
