#pragma once
#include "WorldObject.hpp"
#include <ELGraphics/ERenderChannels.hpp>
#include <ELMaths/Projection.hpp>

class OCamera : public WorldObject
{
private:
	Projection _projection;

protected:
	OCamera(World& world) : WorldObject(world) {}

public:
	WORLDOBJECT_VFUNCS(OCamera, EObjectID::CAMERA)

	virtual ~OCamera() {}

	void Use(class RenderQueue&, ERenderChannels channels = ERenderChannels::ALL) const;
	void Use(class RenderQueue&, int vpX, int vpY, ERenderChannels channels = ERenderChannels::ALL) const;

	Projection& GetProjection() { return _projection; }
	const Projection& GetProjection() const { return _projection; }
	
	virtual const PropertyCollection& GetProperties() override;

	//File IO
	virtual void Read(ByteReader& buffer, ObjectIOContext& ctx) override;
	virtual void Write(ByteWriter& buffer, ObjectIOContext& ctx) const override;
};
