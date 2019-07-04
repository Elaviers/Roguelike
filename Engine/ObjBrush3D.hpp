#pragma once
#include "ObjBrush.hpp"

//TODO - Optimise EVERYTHING to do with this class

class ObjBrush3D : public ObjBrush<3>
{
protected:
	virtual void _UpdateTransform() override;

public:
	GAMEOBJECT_FUNCS(ObjBrush3D)

	ObjBrush3D() {}
	virtual ~ObjBrush3D() {}

	virtual void Render() const override;

	virtual void WriteToFile(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadFromFile(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual void GetCvars(CvarMap&) override;
};
