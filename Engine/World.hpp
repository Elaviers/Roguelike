#pragma once
#include <ELCore/Concepts.hpp>
#include "Entity.hpp"
#include "Geometry.hpp"

class World
{
protected:
	Entity _entRoot;
	List<Geometry*> _geometry;

public:
	World() : _entRoot(Entity::EFlags::NONE, "world") {}
	~World();

	Entity& RootEntity() { return _entRoot; }
	const Entity& RootEntity() const { return _entRoot; }

	template <typename T>
	requires Concepts::DerivedFrom<T, Geometry>
	void AddGeometry(const T& geometry) { _geometry.Add(new T(geometry)); }

	void Clear(const Context&);

	void Update(float deltaTime);
	void Render(RenderQueue&, const Frustum& cameraFrustum) const;

	//Will add overlaps to results. Pairs consist of query result and penetration vector.
	void GetOverlaps(List<Pair<EOverlapResult, Vector3>>& results, const Collider& collider, const Transform& transform, const Entity* ignore = nullptr, const Vector3& sweep = Vector3()) const;

	bool Read(const char* filename, const Context& ctx);
	bool Write(const char* filename, const Context& ctx) const;
};
