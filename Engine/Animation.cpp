#include "Animation.hpp"
#include "Skeleton.hpp"

void Animation::Evaluate(Skeleton& outSkeleton, float time) const
{
	for (auto node = outSkeleton.FirstListNode(); node; node = node->next)
	{
		Joint& j = node->obj;

		auto translationTrack = _translationTracks.Find(j.name);
		auto rotationTrack = _rotationTracks.Find(j.name);
		auto scalingTrack = _scalingTracks.Find(j.name);

		if (translationTrack)
		{
			Vector3 value;
			if (translationTrack->Evaluate(value, time))
				j.localTransform.SetPosition(value);
		}

		if (rotationTrack)
		{
			Quaternion value;
			if (rotationTrack->Evaluate(value, time))
				j.localTransform.SetRotation(value);
		}

		if (scalingTrack)
		{
			Vector3 value;
			if (scalingTrack->Evaluate(value, time))
				j.localTransform.SetScale(value);
		}
	}

	outSkeleton.UpdateCache();
}
