#pragma once
#include "Entity.hpp"
#include <ELGraphics/ERenderChannels.hpp>
#include <ELMaths/Projection.hpp>

class EntCamera : public Entity
{
private:
	Projection _projection;

public:
	Entity_FUNCS(EntCamera, EEntityID::CAMERA)

	EntCamera() { onTransformChanged += Callback(_projection, &Projection::UpdateMatrix); }
	~EntCamera() {}

	void Use(class RenderQueue&, ERenderChannels channels = ERenderChannels::ALL) const;
	void Use(class RenderQueue&, int vpX, int vpY, ERenderChannels channels = ERenderChannels::ALL) const;

	Projection& GetProjection() { return _projection; }
	const Projection& GetProjection() const { return _projection; }
	
	virtual const PropertyCollection& GetProperties() override;

	//File IO
	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;
};
