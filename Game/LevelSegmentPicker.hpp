#pragma once
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
	
public:
	LevelSegmentPicker() {}
	~LevelSegmentPicker() {}

	static LevelSegmentPicker FromString(const String& string, const String& rootLevelDir, World& world);

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
