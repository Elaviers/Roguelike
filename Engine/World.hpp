#pragma once
#include <ELCore/Buffer.hpp>
#include <ELCore/Concepts.hpp>
#include <ELCore/Context.hpp>
#include <ELCore/Types.hpp>
#include <ELCore/Handle.hpp>
#include <ELCore/Hashmap.hpp>
#include <ELCore/List.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELPhys/PhysSimulation.hpp>
#include <ELPhys/RaycastHitInformation.hpp>

class Text;

class Context;
class WorldObject;

class RenderQueue;
struct Frustum;

class WorldObject;

//dumb inheritance!
struct RaycastResult : public RaycastHitInformation
{
	WorldObject* object;
};

class World
{
protected:
	List<WorldObject*> _objects;

	uint32 _nextUID;

	PhysSimulation _physics;

	const Context* _context;

	struct AdditionalObjectData
	{
		Buffer<FixedBody*> fixedBodies;
		Buffer<PhysicsBody*> physicsBodies;
		SharedPointerData<WorldObject> spd;

		AdditionalObjectData(WorldObject* object);

		void ObjectDestroyed();
		void TrySelfDelete();
	};

	Hashmap<uint32, AdditionalObjectData*> _additionalData;

	AdditionalObjectData& _GetAdditionalObjectData(WorldObject* object);

public:
	World(const Context* context) : _nextUID(1), _context(context) {}
	~World();

	const List<WorldObject*>& GetObjects() const { return _objects; }
	uint32 TakeNextUID() { return _nextUID++; }
	const PhysSimulation& GetPhysics() const { return _physics; }
	const Context* GetContext() const { return _context; }
	
	void SetContext(const Context* context) { _context = context;}

	template <typename T, typename... ARGS>
	requires Concepts::DerivedFrom<T, WorldObject>
	T* CreateObject(ARGS... args)
	{
		T* object = new T(*this, std::forward(args)...);
		object->_OnCreated();
		_objects.EmplaceBack(object);
		return object;
	}

	WorldObject* CloneObject(const WorldObject& object, bool deep);

	template <typename T>
	T* FindFirstObjectOfType() const
	{
		for (WorldObject* o : _objects)
			if (T* ptr = dynamic_cast<T*>(o))
				return ptr;

		return nullptr;
	}

	WorldObject* FindObject(const Text& name) const;
	WorldObject* FindObject(uint32 uid) const;

	SharedPointer<WorldObject> TrackObject(WorldObject* object)
	{
		return SharedPointer<WorldObject>(_GetAdditionalObjectData(object).spd);
	}

	void Initialise();

	void Clear();

	void Update(float deltaTime);
	void Render(RenderQueue&, const Frustum* cameraFrustum) const;

	//If parent is specified then all objects read will become children of it
	bool ReadObjects(ByteReader& reader, WorldObject* parent = nullptr);
	bool ReadObjects(const char* filename, WorldObject* parent = nullptr);

	//If parent is specified, only children of parent will be written
	bool WriteObjects(ByteWriter& writer, WorldObject* parent = nullptr) const;
	bool WriteObjects(const char* filename, WorldObject* parent = nullptr) const;

	Handle<FixedBody> CreateFixedBody(WorldObject* owner);
	Handle<FixedBody> CreateFixedBody(WorldObject* owner, const FixedBody&);

	Handle<PhysicsBody> CreatePhysicsBody(WorldObject* owner);
	Handle<PhysicsBody> CreatePhysicsBody(WorldObject* owner, const PhysicsBody&);

	Buffer<WorldObject*> FindOverlaps(const Collider& collider, const Transform& transform, bool includeTouching = false) const;
	Buffer<WorldObject*> FindObjectOverlaps(const WorldObject* a, bool includeTouching = false) const;
	Buffer<RaycastResult> Raycast(const Ray& ray, ECollisionChannels channels) const;

	EOverlapResult ObjectsOverlap(const WorldObject& a, const WorldObject& b, Vector3* penetration) const;
	float CalculateClosestPoints(const WorldObject& a, const WorldObject& b, Vector3& pointA, Vector3& pointB) const;

public:
	//Requires red and green channels to be uint32
	void RenderID(RenderQueue&, const Frustum* cameraFrustum);
	
	WorldObject* DecodeIDMapValue(uint32 red, uint32 green) const;

private:
	void _CMD_List(const Array<String>& tokens, const Context&);
	void _CMD_Ent(const Array<String>& tokens, const Context&);

};
