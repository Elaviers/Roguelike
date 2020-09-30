#pragma once
#include <ELCore/Asset.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/Material.hpp>
#include <ELPhys/Collider.hpp>

class Tile : public Asset
{
public:
	enum class ECollisionType { NONE, BLOCK, WALL_L, WALL_R, FLOOR };

protected:
	ECollisionType _collisionType;

	SharedPointer<const Material> _material;
	SharedPointer<const Mesh> _mesh;

	Vector2 _renderSize;
	Vector2 _size;
	Vector2 _extra;

	void _UpdateRenderSize();

	void _SetSize(const Vector2& size) { _size = size; _UpdateRenderSize(); }
	void _SetExtra(const Vector2& extra) { _extra = extra; _UpdateRenderSize(); }
	const Vector2& _GetSize() const { return _size; }
	const Vector2& _GetExtra() const { return _extra; }

	void _SetCollisionTypeStr(const String&);
	String _GetCollisionTypeStr() const;

	void _SetMaterialStr(const String&, const Context&);
	String _GetMaterialStr(const Context&);

	void _SetMeshStr(const String&, const Context&);
	String _GetMeshStr(const Context&);
public:
	Tile() : _collisionType(ECollisionType::NONE), _size(1.f, 1.f) {}
	~Tile() {}

	virtual const PropertyCollection& GetProperties() override;

	ECollisionType GetCollisionType() const { return _collisionType; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	const SharedPointer<const Mesh>& GetMesh() const { return _mesh; }
	const Vector2& GetSize() const { return _size; }
	const Vector2& GetRenderSize() const { return _renderSize; }

	void SetMaterial(const SharedPointer<const Material>& material) { _material = material; _UpdateRenderSize(); }

	static Tile FromText(const String& str, const Context& ctx)
	{
		Tile t;
		t.ReadText(str, ctx);
		return t;
	}

	const Collider& GetCollider() const;
};
