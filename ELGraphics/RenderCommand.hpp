#pragma once
#include "Colour.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include <ELMaths/Matrix4.hpp>
#include <ELSys/GLCubemap.hpp>

class MeshManager;
class TextureManager;

struct RenderContext
{
	const MeshManager* meshManager;
	const TextureManager* textureManager;

	int submittedLightCount;
};

class RenderCommand
{
protected:
	RenderCommand() {}
	
public:
	virtual ~RenderCommand() {}

	virtual void Execute(RenderContext&) const = 0;
};

class RCMDSetBool : public RenderCommand
{
public:
	enum class Type { FONT };

protected:
	Type _type;
	bool _value;

public:
	RCMDSetBool(Type type, bool value) : _type(type), _value(value) {}
	virtual ~RCMDSetBool() {}

	void Set(Type type, bool value)
	{
		_type = type;
		_value = value;
	}

	virtual void Execute(RenderContext&) const override;
};

class RCMDSetFloat : public RenderCommand
{
public:
	enum class Type { LINE_WIDTH };

protected:
	Type _type;
	float _value;

public:
	RCMDSetFloat(Type type, float value) : _type(type), _value(value) {}
	virtual ~RCMDSetFloat() {}

	void Set(Type type, float value)
	{
		_type = type;
		_value = value;
	}

	virtual void Execute(RenderContext&) const override;
};

class RCMDSetTransform : public RenderCommand
{
protected:
	Matrix4 _matrix;

public:
	RCMDSetTransform(const Matrix4& matrix) : _matrix(matrix) {}
	RCMDSetTransform(Matrix4&& matrix) : _matrix(matrix) {}
	virtual ~RCMDSetTransform() {}

	void Set(const Matrix4& matrix) { _matrix = matrix; }
	void Set(Matrix4&& matrix) { _matrix = matrix; }

	virtual void Execute(RenderContext&) const override;
};

class RCMDSetUVOffset : public RenderCommand
{
protected:
	Vector2 _uvOffset;

public:
	RCMDSetUVOffset(const Vector2& uvOffset = Vector2()) : _uvOffset(uvOffset) {}
	RCMDSetUVOffset(Vector2&& uvOffset) : _uvOffset(uvOffset) {}
	virtual ~RCMDSetUVOffset() {}

	void Set(const Vector2& uvOffset) { _uvOffset = uvOffset; }
	void Set(Vector2&& uvOffset) { _uvOffset = uvOffset; }

	virtual void Execute(RenderContext&) const override;

	static const RCMDSetUVOffset& Default() { static RCMDSetUVOffset uvOffset; return uvOffset; }
};

class RCMDSetUVScale : public RenderCommand
{
protected:
	Vector2 _uvScale;

public:
	RCMDSetUVScale(const Vector2& uvScale = Vector2(1.f, 1.f)) : _uvScale(uvScale) {}
	RCMDSetUVScale(Vector2&& uvScale) : _uvScale(uvScale) {}
	virtual ~RCMDSetUVScale() {}

	void Set(const Vector2& uvScale) { _uvScale = uvScale; }
	void Set(Vector2&& uvScale) { _uvScale = uvScale; }

	virtual void Execute(RenderContext&) const override;

	static const RCMDSetUVScale& Default() { static RCMDSetUVScale uvScale; return uvScale; }
};

class RCMDSetColour : public RenderCommand
{
protected:
	Colour _colour;
	Colour _secondaryColour;
	bool _bBlend;

public:
	RCMDSetColour(const Colour& colour = Colour::White) : _colour(colour), _bBlend(false) {}
	RCMDSetColour(const Colour& primary, const Colour& secondary) : _colour(primary), _secondaryColour(secondary), _bBlend(true) {}
	virtual ~RCMDSetColour() {}

	void Set(const Colour& colour) { _colour = colour; _bBlend = false; }
	void Set(const Colour& primary, const Colour& secondary) { _colour = primary; _secondaryColour = secondary; _bBlend = false; }

	virtual void Execute(RenderContext&) const override;
};

class RCMDSetTexture : public RenderCommand
{
public:
	enum class Type : byte
	{
		WHITE = 1, BLACK = 2, GREY = 3, UNIT_NORMAL = 4 
	};

protected:
	byte _type;
	GLuint _tex;
	uint16 _unit;

public:
	RCMDSetTexture(GLuint tex, uint16 unit) : _type(0), _tex(tex), _unit(unit) {}
	RCMDSetTexture(Type type, uint16 unit) : _type((byte)type), _unit(unit) {}
	RCMDSetTexture(const Texture& texture, uint16 unit) : _type(0), _tex(texture.GL().GetID()), _unit(unit) {}
	virtual ~RCMDSetTexture() {}

	void Set(GLuint tex, uint16 unit) { _type = 0; _tex = tex; _unit = unit; }
	void Set(Type type, uint16 unit) { _type = (byte)type; _unit = unit; }
	void Set(const Texture* texture, uint16 unit) { _type = 0; _tex = texture->GL().GetID(); _unit = unit; }

	virtual void Execute(RenderContext&) const override;
};

class RCMDSetCubemap : public RenderCommand
{
protected:
	GLuint _tex;
	uint16 _unit;

public:
	RCMDSetCubemap(GLuint tex, uint16 unit) : _tex(tex), _unit(unit) {}
	RCMDSetCubemap(const GLCubemap& cubemap, uint16 unit) : _tex(cubemap.getGLId()), _unit(unit) {}
	virtual ~RCMDSetCubemap() {}

	void Set(GLuint tex, uint16 unit) { _tex = tex; _unit = unit; }
	void Set(const Texture* texture, uint16 unit) { _tex = texture->GL().GetID(); _unit = unit; }

	virtual void Execute(RenderContext& ctx) const;
};

class RCMDRenderMesh : public RenderCommand
{
protected:
	byte _type;
	const Mesh* _mesh;

	enum class _Type : byte
	{
		LINE = 1, PLANE = 2, CUBE = 3, INV_CUBE = 4
	};

	RCMDRenderMesh(_Type type) : _type((byte)type) {}

public:
	RCMDRenderMesh(const Mesh& mesh) : _type(0), _mesh(&mesh) {}
	virtual ~RCMDRenderMesh() {}

	void Set(const Mesh& mesh) { _mesh = &mesh; }

	virtual void Execute(RenderContext&) const override;

	static const RCMDRenderMesh LINE;
	static const RCMDRenderMesh PLANE;
	static const RCMDRenderMesh CUBE;
	static const RCMDRenderMesh INV_CUBE;
};

class RCMDSetSkinningMatrices : public RenderCommand
{
protected:
	Buffer<Matrix4> _matrices;
	const Buffer<Matrix4>* _ref;

public:
	RCMDSetSkinningMatrices(const Buffer<Matrix4>& matrices) : _matrices(matrices), _ref(nullptr) {}
	RCMDSetSkinningMatrices(const Buffer<Matrix4>* matrices) : _ref(matrices) {}
	virtual ~RCMDSetSkinningMatrices() {}

	void Set(const Buffer<Matrix4>& matrices) { _ref = nullptr; _matrices = matrices; }
	void Set(const Buffer<Matrix4>* matrices) { _ref = matrices; }

	virtual void Execute(RenderContext&) const override;
};

class RCMDLight : public RenderCommand
{
protected:
	Vector3 _position;
	Vector3 _colour;
	float _radius;

public:
	RCMDLight(const Vector3& position, const Vector3& colour, float radius) : _position(position), _colour(colour), _radius(radius) {}
	virtual ~RCMDLight() {}

	void SetPosition(const Vector3& position) { _position = position; }
	void SetColour(const Vector3& colour) { _colour = colour; }
	void SetRadius(float radius) { _radius = radius; }

	virtual void Execute(RenderContext&) const override;
};

class RCMDSetDepthFunc : public RenderCommand
{
protected:
	GLenum _depthFunc;

	RCMDSetDepthFunc(GLenum df) : _depthFunc(df) {}
public:
	virtual ~RCMDSetDepthFunc() {}

	virtual void Execute(RenderContext&) const override;

	static const RCMDSetDepthFunc NEVER;
	static const RCMDSetDepthFunc LESS;
	static const RCMDSetDepthFunc EQUAL;
	static const RCMDSetDepthFunc LEQUAL;
	static const RCMDSetDepthFunc GREATER;
	static const RCMDSetDepthFunc NOTEQUAL;
	static const RCMDSetDepthFunc GEQUAL;
	static const RCMDSetDepthFunc ALWAYS;
};
