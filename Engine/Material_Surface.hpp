#pragma once
#include "Material.hpp"
#include "Texture.hpp"

class MaterialSurface : public Material
{
private:
	const Texture *_diffuse;
	const Texture*_normal;
	const Texture*_specular;
	const Texture*_reflection;

	void _CMD_tex(const Buffer<String>& args);
public:
	MaterialSurface(const Texture*diffuse = nullptr, const Texture*normal = nullptr, const Texture*specular = nullptr, const Texture*reflection = nullptr)
		: Material(RenderChannel::SURFACE), _diffuse(diffuse), _normal(normal), _specular(specular), _reflection(reflection)
	{}
	
	virtual ~MaterialSurface() {}

	virtual const PropertyCollection& GetProperties();

	inline void SetDiffuse(const Texture* texture) { _diffuse = texture; }
	inline void SetNormal(const Texture* texture) { _normal = texture; }
	inline void SetSpecular(const Texture* texture) { _specular = texture; }
	inline void SetReflection(const Texture* texture) { _reflection = texture; }

	virtual void Apply(const RenderParam* param = nullptr) const override;
	
	inline bool operator==(const MaterialSurface &other) { return _diffuse == other._diffuse && _normal == other._normal && _specular == other._specular; }
};
