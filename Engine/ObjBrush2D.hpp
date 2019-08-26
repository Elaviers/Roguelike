#pragma once
#include "ObjBrush.hpp"

class ObjBrush2D : public ObjBrush<2>
{
protected:
	virtual void _OnTransformChanged() override;

public:
	GAMEOBJECT_FUNCS(ObjBrush2D, ObjectIDS::PLANE)

	float level;

	ObjBrush2D() : level(0.f) { SetRelativeRotation(Vector3(-90.f, 0.f, 0.f)); }
	virtual ~ObjBrush2D() {}
	
	virtual void Render(EnumRenderChannel) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual void GetCvars(CvarMap &) override;
};
