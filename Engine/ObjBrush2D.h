#pragma once
#include "ObjBrush.h"

class ObjBrush2D : public ObjBrush<2>
{
protected:
	virtual void _UpdateTransform() override;

public:
	GAMEOBJECT_FUNCS(ObjBrush2D)

	float level;

	ObjBrush2D() : level(0.f) { SetRelativeRotation(Vector3(-90.f, 0.f, 0.f)); }
	virtual ~ObjBrush2D() {}
	
	virtual void Render() const override;

	virtual void WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual void GetCvars(CvarMap &) override;
};