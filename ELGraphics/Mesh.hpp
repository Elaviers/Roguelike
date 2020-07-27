#pragma once
#include <ELCore/Asset.hpp>
#include "MeshRenderer.hpp"
#include <ELMaths/Bounds.hpp>

class Mesh : public Asset
{
protected:
	Mesh() { }

	MeshRenderer _renderer;

public:
	Bounds bounds;

	virtual ~Mesh() {}

	virtual bool IsValid() const { return false; }

	virtual void UpdateRenderer() {}

	void SetRenderMode(ERenderMode mode) { _renderer.SetRenderMode(mode); }
	void Render() const { _renderer.Render(); }

	static Mesh* FromData(const Buffer<byte>&);
};
