#pragma once
#include <ELCore/Asset.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/Material.hpp>
#include <ELPhys/Collider.hpp>

class Tile : public Asset
{
public:
	enum class ECollisionType { NONE, BLOCK, WALL_L, WALL_R };

protected:
	ECollisionType _collisionType;

	SharedPointer<const Material> _material;
	SharedPointer<const Mesh> _mesh;

	Vector2 _size;

	void _SetCollisionTypeStr(const String&);
	String _GetCollisionTypeStr() const;

	void _SetMaterialStr(const String&, const Context&);
	String _GetMaterialStr(const Context&);

	void _SetMeshStr(const String&, const Context&);
	String _GetMeshStr(const Context&);
public:
	Tile() : _collisionType(ECollisionType::NONE) {}
	~Tile() {}

	virtual const PropertyCollection& GetProperties() override;

	ECollisionType GetCollisionType() const { return _collisionType; }
	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	const SharedPointer<const Mesh>& GetMesh() const { return _mesh; }
	const Vector2& GetSize() const { return _size; }

	static Tile FromText(const String& str, const Context& ctx)
	{
		Tile t;
		t.ReadText(str, ctx);
		return t;
	}

	const Collider& GetCollider() const;
};
