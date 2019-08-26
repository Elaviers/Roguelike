#include "Animation.hpp"
#include "Skeleton.hpp"

Animation* Animation::FromData(const Buffer<byte>& data)
{
	if (data[0] == ASSET_ANIMATION)
	{
		Animation* animation = new Animation();
		BufferReader<byte> reader(data);
		animation->_ReadData(reader);
		return animation;
	}

	return nullptr;
}

void Animation::Evaluate(Skeleton& outSkeleton, float time) const
{
	for (auto it = outSkeleton.FirstListElement(); it; ++it)
	{
		Joint& j = *it;

		auto translationTrack = _translationTracks.Get(j.name);
		auto rotationTrack = _rotationTracks.Get(j.name);
		auto scalingTrack = _scalingTracks.Get(j.name);

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

void Animation::_ReadData(BufferReader<byte> &iterator)
{
	iterator.Read_byte();

	uint32 tTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < tTrackCount; ++i)
	{
		auto track = _translationTracks[iterator.Read_string()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 i = 0; i < keyframeCount; ++i)
			track.AddKey(iterator.Read_float(), iterator.Read_vector3(), iterator.Read_byte());
	}

	uint32 rTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < rTrackCount; ++i)
	{
		auto track = _rotationTracks[iterator.Read_string()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 i = 0; i < keyframeCount; ++i)
			track.AddKey(iterator.Read_float(), Quaternion(iterator.Read_vector3()), iterator.Read_byte());
	}

	uint32 sTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < sTrackCount; ++i)
	{
		auto track = _scalingTracks[iterator.Read_string()];
		
		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 i = 0; i < keyframeCount; ++i)
			track.AddKey(iterator.Read_float(), iterator.Read_vector3(), iterator.Read_byte());
	}
}

void Animation::_WriteData(BufferWriter<byte> &iterator) const
{
	iterator.Write_byte(ASSET_ANIMATION);

	auto tTrackBuffer = _translationTracks.ToKVBuffer();
	auto rTrackBuffer = _rotationTracks.ToKVBuffer();
	auto sTrackBuffer = _scalingTracks.ToKVBuffer();

	iterator.Write_uint32((uint32)tTrackBuffer.GetSize());
	for (uint32 i = 0; i < tTrackBuffer.GetSize(); ++i)
	{
		iterator.Write_string(tTrackBuffer[i].first->GetData());
		
		auto keyframes = tTrackBuffer[i].second->GetKeyframes();
		
		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 i = 0; i < keyframes.GetSize(); ++i)
		{
			iterator.Write_float(keyframes[i].time);
			iterator.Write_vector3(keyframes[i].value);
			iterator.Write_byte(keyframes[i].interpolation);
		}
	}

	iterator.Write_uint32((uint32)rTrackBuffer.GetSize());
	for (uint32 i = 0; i < rTrackBuffer.GetSize(); ++i)
	{
		iterator.Write_string(rTrackBuffer[i].first->GetData());

		auto keyframes = rTrackBuffer[i].second->GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 i = 0; i < keyframes.GetSize(); ++i)
		{
			iterator.Write_float(keyframes[i].time);
			iterator.Write_vector3(keyframes[i].value.ToEuler());
			iterator.Write_byte(keyframes[i].interpolation);
		}
	}

	iterator.Write_uint32((uint32)sTrackBuffer.GetSize());
	for (uint32 i = 0; i < sTrackBuffer.GetSize(); ++i)
	{
		iterator.Write_string(sTrackBuffer[i].first->GetData());

		auto keyframes = sTrackBuffer[i].second->GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 i = 0; i < keyframes.GetSize(); ++i)
		{
			iterator.Write_float(keyframes[i].time);
			iterator.Write_vector3(keyframes[i].value);
			iterator.Write_byte(keyframes[i].interpolation);
		}
	}
}
