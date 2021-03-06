#include "LevelSegmentPicker.hpp"
#include <Engine/World.hpp>

bool LevelSegmentPicker::Load(const String& string, const String& rootLevelDir)
{
	SegmentBag* bag = nullptr;
	float bagTotalOverride = 0.f;

	Buffer<String> lines = string.Split("\r\n");
	for (uint32 i = 0; i < lines.GetSize(); ++i)
	{
		Buffer<String> tokens = lines[i].Split(" ");

		if (tokens.GetSize() >= 1)
		{
			if (tokens[0] == "bag" || tokens[0] == "ebag")
			{
				if (tokens[0] == "ebag") // :/
					bag = &_essentialBags.EmplaceBack();
				else
					bag = &_otherBags.EmplaceBack();

				bagTotalOverride = tokens.GetSize() >= 2 ? tokens[1].ToFloat() : 0.f;
				bag->minimumDepth = tokens.GetSize() >= 3 ? tokens[2].ToInt() : 0;
				bag->maximumDepth = tokens.GetSize() >= 4 ? tokens[3].ToInt() : 1000000;
			}
			else if (tokens.GetSize() >= 2 && tokens[0] == "segment" && bag)
			{
				WorldObject* segment = _world.CreateObject<WorldObject>();
				if (_world.ReadObjects(CSTR(rootLevelDir, tokens[1]), segment))
				{
					bag->bag.Add(segment, tokens.GetSize() > 2 ? tokens[2].ToFloat() : 1);
					if (bagTotalOverride)
						bag->bag.SetRemainingWeight(bagTotalOverride);
				}
				else
					segment->Destroy();
			}
		}
	}

	return true;
}

const WorldObject* LevelSegmentPicker::TakeNextSegment(Random& random, unsigned int depth)
{
	SegmentBag* bag = nullptr;

	List<List<SegmentBag>::Iterator> options;

	for (auto it = _essentialBags.begin(); it; ++it)
	{
		if (depth >= it->minimumDepth && depth <= it->maximumDepth)
		{
			options.AddBack(it);
		}
	}

	bool essential = true;

	if (!options.begin())
	{
		essential = false;

		for (auto it = _otherBags.begin(); it; ++it)
		{
			if (depth >= it->minimumDepth && depth <= it->maximumDepth)
			{
				options.AddBack(it);
			}
		}
	}

	size_t numOptions = options.GetSize();
	if (numOptions > 0)
	{
		List<SegmentBag>::Iterator chosen = *IteratorUtils::Offset(options.begin(), random.Next((uint32)numOptions));
		const WorldObject* segment = chosen->bag.TakeNext(random, 1.f);

		if (chosen->bag.GetRemainingWeight() <= 0.f)
			essential ? _essentialBags.Remove(chosen) : _otherBags.Remove(chosen);

		return segment;
	}

	return nullptr;
}

void LevelSegmentPicker::GetAvailableOptions(const Random& random, unsigned int depth, RandomBag<const BagItem> &out_Bag)
{
	out_Bag.Clear();

	Random r = random;
	for (SegmentBag& bag : _essentialBags)
	{
		if (depth >= bag.minimumDepth && depth <= bag.maximumDepth)
		{
			for (size_t i = 0; i < bag.bag.GetEntries().GetSize(); ++i)
			{
				const auto& entry = bag.bag.GetEntries()[i];
				if (entry.weight >= 0.f)
					out_Bag.Emplace(entry.weight, true, this, &bag, entry.item);
			}
		}
	}

	bool essential = true;

	if (out_Bag.GetRemainingWeight() <= 0.f)
	{
		essential = false;

		for (SegmentBag& bag : _otherBags)
		{
			for (size_t i = 0; i < bag.bag.GetEntries().GetSize(); ++i)
			{
				const auto& entry = bag.bag.GetEntries()[i];
				if (entry.weight >= 0.f)
					out_Bag.Emplace(entry.weight, false, this, &bag, entry.item);
			}
		}
	}
}

void LevelSegmentPicker::BagItem::TakeFromRelevantBag(float weight) const
{
	_bag->bag.TakeFrom(segment, weight);

	if (_isEssential)
	{
		for (auto it = _owner->_essentialBags.begin(); it;)
		{
			if (it->bag.GetRemainingWeight() <= 0.f)
				it = _owner->_essentialBags.Remove(it);
			else
				++it;
		}
	}
	else
	{
		for (auto it = _owner->_otherBags.begin(); it;)
		{
			if (it->bag.GetRemainingWeight() <= 0.f)
				it = _owner->_otherBags.Remove(it);
			else
				++it;
		}
	}
}
