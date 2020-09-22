#pragma once
#include <Engine/Entity.hpp>
#include "RandomBag.hpp"

class LevelSegmentPicker
{
	struct SegmentBag
	{
		RandomBag<const Entity*> bag;
		unsigned int minimumDepth = 0;
		unsigned int maximumDepth = 0;
	};

	List<SegmentBag> _essentialBags;
	List<SegmentBag> _otherBags;
	
public:
	LevelSegmentPicker() {}
	~LevelSegmentPicker() {}

	static LevelSegmentPicker FromString(const String& string, const String& rootLevelDir, const Context&);

	const Entity* TakeNextSegment(Random& random, unsigned int depth);
	
	struct BagItem
	{
	private:
		bool _isEssential;
		LevelSegmentPicker* _owner;
		SegmentBag* _bag;

	public:
		BagItem() : _isEssential(false), _owner(nullptr), _bag(nullptr), segment(nullptr) {}
		BagItem(bool essential, LevelSegmentPicker* owner, SegmentBag* bag, const Entity* segment) : _isEssential(essential), _owner(owner), _bag(bag), segment(segment) {}
		BagItem(const BagItem&) = delete;
		BagItem(BagItem&&) = delete;

		const Entity* segment;

		void TakeFromRelevantBag(float weight) const;
	};

	void GetAvailableOptions(const Random& random, unsigned int depth, RandomBag<const BagItem>& out_Bag);
};
