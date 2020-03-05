#pragma once
#include "EntBrush.hpp"

class EntBrush2D : public EntBrush<2>
{
protected:
	void _OnTransformChanged();
	virtual void _OnPointChanged() override;

	bool _updatingTransform;

public:
	Entity_FUNCS(EntBrush2D, EEntityID::PLANE)

	float level;

	EntBrush2D() : _updatingTransform(false), level(0.f) 
	{ 
		onTransformChanged += FunctionPointer<void>(this, &EntBrush2D::_OnTransformChanged);
		SetRelativeRotation(Vector3(-90.f, 0.f, 0.f));
	}

	EntBrush2D(const EntBrush2D& other) : EntBrush<2>(other), _updatingTransform(false), level(other.level)
	{
		onTransformChanged += FunctionPointer<void>(this, &EntBrush2D::_OnTransformChanged);
	}

	virtual ~EntBrush2D() {}
	
	virtual void Render(ERenderChannels) const override;

	virtual void WriteData(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const override;
	virtual void ReadData(BufferReader<byte> &buffer, const NumberedSet<String> &strings) override;

	virtual const PropertyCollection& GetProperties() override;
};
