#pragma once
#include "Entity.hpp"
#include <ELMaths/Projection.hpp>

class EntCamera : public Entity
{
private:
	static const EntCamera* _currentCamera;

	Projection _projection;
public:
	Entity_FUNCS(EntCamera, EEntityID::CAMERA)

	EntCamera() { onTransformChanged += Callback(&_projection, &Projection::UpdateMatrix); }
	~EntCamera() {}

	void Use() const;
	void Use(int vpX, int vpY) const;
	static const EntCamera* Current() { return _currentCamera; }

	Projection& GetProjection() { return _projection; }
	const Projection& GetProjection() const { return _projection; }
	
	virtual const PropertyCollection& GetProperties() override;

	//File IO
	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;
};
