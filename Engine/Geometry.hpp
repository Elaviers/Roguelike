#pragma once

class RenderQueue;

//Represents static geometry
class Geometry
{
protected:
	Geometry() {}

public:
	virtual ~Geometry() {}

	virtual void Render(RenderQueue& q) const {}
};
