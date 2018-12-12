#pragma once
#include "GLTexture.h"
#include "Material.h"

class MaterialSurface : public Material
{
private:
	const GLTexture *_diffuse;
	const GLTexture *_normal;
	const GLTexture *_specular;
	const GLTexture *_reflection;

public:
	MaterialSurface(const GLTexture *diffuse = nullptr, const GLTexture *normal = nullptr, const GLTexture *specular = nullptr, const GLTexture *reflection = nullptr)
		: _diffuse(diffuse), _normal(normal), _specular(specular), _reflection(reflection) {}
	
	virtual ~MaterialSurface() {}

	inline void SetDiffuse(const GLTexture* texture) { _diffuse = texture; }
	inline void SetNormal(const GLTexture* texture) { _normal = texture; }
	inline void SetSpecular(const GLTexture* texture) { _specular = texture; }
	inline void SetReflection(const GLTexture* texture) { _reflection = texture; }

	virtual void FromString(const String &string) override;

	virtual void Apply(const RenderParam *param = nullptr) const override { BindTextures(); }
	virtual void BindTextures() const override;

	inline bool operator==(const MaterialSurface &other) { return _diffuse == other._diffuse && _normal == other._normal && _specular == other._specular; }
};
