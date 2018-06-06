#pragma once

class Renderer
{
public:
	Renderer() {}
	virtual ~Renderer() {}

	virtual void Render() const = 0;
};

