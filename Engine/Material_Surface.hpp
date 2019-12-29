#pragma once
#include "Material.hpp"
#include "Texture.hpp"

class MaterialSurface : public Material
{
private:
	SharedPointer<const Texture> _diffuse, _normal, _specular, _reflection;
	
	void _CMD_tex(const Buffer<String>& args);
public:
	MaterialSurface(
		const SharedPointer<const Texture>& diffuse = SharedPointer<const Texture>(), 
		const SharedPointer<const Texture>& normal = SharedPointer<const Texture>(), 
		const SharedPointer<const Texture>& specular = SharedPointer<const Texture>(), 
		const SharedPointer<const Texture>& reflection = SharedPointer<const Texture>())
		: Material(RenderChannels::SURFACE), _diffuse(diffuse), _normal(normal), _specular(specular), _reflection(reflection)
	{}
	
	virtual ~MaterialSurface() {}

	virtual const PropertyCollection& GetProperties() override;

	void SetDiffuse(const SharedPointer<const Texture>& texture) { _diffuse = texture; }
	void SetNormal(const SharedPointer<const Texture>& texture) { _normal = texture; }
	void SetSpecular(const SharedPointer<const Texture>& texture) { _specular = texture; }
	void SetReflection(const SharedPointer<const Texture>& texture) { _reflection = texture; }

	virtual void Apply(const RenderParam* param = nullptr) const override;
	
	bool operator==(const MaterialSurface &other) { return _diffuse == other._diffuse && _normal == other._normal && _specular == other._specular; }
};
