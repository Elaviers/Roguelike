#pragma once
#include <Engine/World.hpp>
#include <Engine/WorldObject.hpp>
#include "RandomBag.hpp"

class LevelSegmentPicker
{
	struct SegmentBag
	{
		RandomBag<const WorldObject*> bag;
		unsigned int minimumDepth = 0;
		unsigned int maximumDepth = 0;
	};

	List<SegmentBag> _essentialBags;
	List<SegmentBag> _otherBags;
	
	World _world;

public:
	LevelSegmentPicker(const Context& context) : _world(&context) {}
	~LevelSegmentPicker() {}

	bool Load(const String& string, const String& rootLevelDir);

	const WorldObject* TakeNextSegment(Random& random, unsigned int depth);
	
	struct BagItem
	{
	private:
		bool _isEssential;
		LevelSegmentPicker* _owner;
		SegmentBag* _bag;

	public:
		BagItem() : _isEssential(false), _owner(nullptr), _bag(nullptr), segment(nullptr) {}
		BagItem(bool essential, LevelSegmentPicker* owner, SegmentBag* bag, const WorldObject* segment) : _isEssential(essential), _owner(owner), _bag(bag), segment(segment) {}

		const WorldObject* segment;

		void TakeFromRelevantBag(float weight) const;
	};

	void GetAvailableOptions(const Random& random, unsigned int depth, RandomBag<const BagItem>& out_Bag);
};
