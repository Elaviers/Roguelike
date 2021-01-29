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

	const List<Geometry*>& GetGeometry() const { return _geometry; }

	template <typename T>
	requires Concepts::DerivedFrom<T, Geometry>
	void AddGeometry(const T& geometry) { _geometry.Add(new T(geometry)); }

	void RemoveGeometry(uint32 uid)
	{
		for (List<Geometry*>::Iterator g = _geometry.begin(); g.IsValid(); ++g)
			if ((*g)->GetUID() == uid)
			{
				delete *g;
				_geometry.Remove(g);
				return;
			}
	}

	void Initialise(const Context&);

	void Clear(const Context&);

	void Update(float deltaTime);
	void Render(RenderQueue&, const Frustum& cameraFrustum) const;

	//Will add overlaps to results. Pairs consist of query result and penetration vector.
	void GetOverlaps(List<Pair<EOverlapResult, Vector3>>& results, const Collider& collider, const Transform& transform, const Entity* ignore = nullptr, const Vector3& sweep = Vector3()) const;

	bool Read(const char* filename, const Context& ctx);
	bool Write(const char* filename, const Context& ctx) const;

	//Renders scene where the colour uniform variable is unique per entity / geometry
	void RenderIDMap(RenderQueue&, const Frustum& cameraFrustum) const;

	struct IDMapResultConst
	{
		union
		{
			const Geometry* geometry;
			const Entity* entity;
		};

		bool isEntity;
	};

	struct IDMapResult
	{
		union
		{
			Geometry* geometry;
			Entity* entity;
		};

		bool isEntity;
	};

	IDMapResult DecodeIDMapValue(byte r, byte g, byte b);
	IDMapResultConst DecodeIDMapValue(byte r, byte g, byte b) const 
	{ 
		IDMapResult result = const_cast<World*>(this)->DecodeIDMapValue(r, g, b);
		return *reinterpret_cast<IDMapResultConst*>(&result); 
	}
};
