#pragma once
#include "Colour.hpp"
#include "MeshManager.hpp"
#include "TextureManager.hpp"
#include "RenderEntry.hpp"
#include <ELCore/Concepts.hpp>
#include <ELCore/Pool.hpp>
#include <ELMaths/Transform.hpp>

class Projection;

struct QueueGroup
{
	const int priority;
	List<const RenderEntry*> staticQueue;
	List<const RenderEntry*> dynamicQueue;

	QueueGroup(
		int priority,
		const NewHandler& staticNew,
		const DeleteHandler& staticDelete,
		const NewHandler& dynamicNew,
		const DeleteHandler& dynamicDelete)
		: priority(priority), staticQueue(staticNew, staticDelete), dynamicQueue(dynamicNew, dynamicDelete)
	{
	}
};

class RenderQueue
{
private:
	typedef MultiPool<byte, sizeof(RenderEntry) * 32> _EntryPoolType;
	typedef MultiPool<byte, sizeof(const RenderEntry*) * 32> _PtrPoolType;

	_EntryPoolType _dynamicEntryPool;
	_EntryPoolType _staticEntryPool;
	_PtrPoolType _dynamicPtrPool;
	_PtrPoolType _staticPtrPool;

	List<RenderEntry> _dynamicEntries;
	List<RenderEntry> _staticEntries;
	List<QueueGroup> _queues;
	
	QueueGroup& _GetQueue(int priority);

public:
	RenderQueue() : _dynamicEntries(NewHandler(&_dynamicEntryPool, &_EntryPoolType::NewArray), DeleteHandler(&_dynamicEntryPool, &_EntryPoolType::DeleteHandler)) {}
	~RenderQueue() {}

	void ClearDynamicQueue();
	void Render(ERenderChannels channels, const MeshManager& meshManager, const TextureManager& textureManager) const;

	void AddStatic(const RenderEntry*, int priority = 0);
	void RemoveStatic(const RenderEntry*, int priority = 0);
	void AddDynamic(const RenderEntry*, int priority = 0);
	RenderEntry& NewDynamicEntry(ERenderChannels renderChannels, int priority = 0);
};
