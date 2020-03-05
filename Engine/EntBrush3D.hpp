#pragma once
#include "EntBrush.hpp"

class EntBrush3D : public EntBrush<3>
{
protected:
	void _OnTransformChanged();
	virtual void _OnPointChanged() override;

	bool _updatingTransform;

public:
	Entity_FUNCS(EntBrush3D, EEntityID::BRUSH);

	EntBrush3D() : _updatingTransform(false)
	{ 
		onTransformChanged += FunctionPointer<void>(this, &EntBrush3D::_OnTransformChanged);
	}

	EntBrush3D(const EntBrush3D& other) : EntBrush<3>(other), _updatingTransform(false)
	{
		onTransformChanged += FunctionPointer<void>(this, &EntBrush3D::_OnTransformChanged);
	}

	virtual ~EntBrush3D() {}

	virtual void Render(ERenderChannels) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual const PropertyCollection& GetProperties() override;
};
