#pragma once
#include "GLTexture.h"

class Material
{
private:
	const GLTexture *_diffuse;
	const GLTexture *_normal;
	const GLTexture *_specular;
	const GLTexture *_reflection;

public:
	Material(const GLTexture *diffuse = nullptr, const GLTexture *normal = nullptr, const GLTexture *specular = nullptr, const GLTexture *reflection = nullptr)
		: _diffuse(diffuse), _normal(normal), _specular(specular), _reflection(reflection) {}
	
	~Material() {}

	inline void SetDiffuse(const GLTexture* texture) { _diffuse = texture; }
	inline void SetNormal(const GLTexture* texture) { _normal = texture; }
	inline void SetSpecular(const GLTexture* texture) { _specular = texture; }
	inline void SetReflection(const GLTexture* texture) { _reflection = texture; }

	inline void Apply() const
	{
		if (_diffuse)
			_diffuse->Bind(0);
		
		if (_normal)
			_normal->Bind(1);
		
		if (_specular)
			_specular->Bind(2);

		if (_reflection)
			_reflection->Bind(3);
	}

	inline bool operator==(const Material &other) { return _diffuse == other._diffuse && _normal == other._normal && _specular == other._specular; }
};

