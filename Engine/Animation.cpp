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

void Animation::Evaluate(Buffer<Mat4>& skinningMatrices, const Skeleton& skeleton, float time) const
{
	skinningMatrices.SetSize(skeleton.GetJointCount());

	for (auto it = skeleton.FirstListElement(); it; ++it)
	{
		Joint& j = *it;

		auto translationTrack = _translationTracks.Get(j.name);
		auto rotationTrack = _rotationTracks.Get(j.name);
		auto scalingTrack = _scalingTracks.Get(j.name);

		Vector3 translation, scale;
		Quaternion rotation;

		if (translationTrack)
			translationTrack->Evaluate(translation, time);

		if (rotationTrack)
			rotationTrack->Evaluate(rotation, time);

		if (scalingTrack)
			scalingTrack->Evaluate(scale, time);

		skinningMatrices[j.GetID()] = Matrix::Transformation(translation, rotation, scale);
	}
}

void Animation::_ReadData(BufferReader<byte> &iterator)
{
	iterator.Read_byte();

	uint32 tTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < tTrackCount; ++i)
	{
		AnimationTrack<Vector3>& track = _translationTracks[iterator.Read_string()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Vector3 value = iterator.Read_vector3();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}

	uint32 rTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < rTrackCount; ++i)
	{
		AnimationTrack<Quaternion>& track = _rotationTracks[iterator.Read_string()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Quaternion value = iterator.Read_vector3();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}

	uint32 sTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < sTrackCount; ++i)
	{
		AnimationTrack<Vector3>& track = _scalingTracks[iterator.Read_string()];
		
		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Vector3 value = iterator.Read_vector3();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}
}

void Animation::_WriteData(BufferWriter<byte> &iterator) const
{
	Debug::PrintLine("_________________\nSaving animation...");

	iterator.Write_byte(ASSET_ANIMATION);

	auto tTrackBuffer = _translationTracks.ToKVBuffer();
	auto rTrackBuffer = _rotationTracks.ToKVBuffer();
	auto sTrackBuffer = _scalingTracks.ToKVBuffer();

	Debug::PrintLine("TRANSLATION TRACKS:\n{");

	iterator.Write_uint32((uint32)tTrackBuffer.GetSize());
	for (uint32 i = 0; i < tTrackBuffer.GetSize(); ++i)
	{
		Debug::PrintLine(CSTR("\t\"", tTrackBuffer[i]->first, "\":\n\t{"));

		iterator.Write_string(tTrackBuffer[i]->first.GetData());
		
		auto keyframes = tTrackBuffer[i]->second.GetKeyframes();
		
		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value));

			iterator.Write_float(keyframes[j].time);
			iterator.Write_vector3(keyframes[j].value);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}\n\nROTATION TRACKS:\n{");

	iterator.Write_uint32((uint32)rTrackBuffer.GetSize());
	for (uint32 i = 0; i < rTrackBuffer.GetSize(); ++i)
	{
		Debug::PrintLine(CSTR("\t\"", tTrackBuffer[i]->first, "\":\n\t{"));

		iterator.Write_string(rTrackBuffer[i]->first.GetData());

		auto keyframes = rTrackBuffer[i]->second.GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value.ToEuler()));

			iterator.Write_float(keyframes[j].time);
			iterator.Write_vector3(keyframes[j].value.ToEuler());
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}\n\nSCALING TRACKS:\n{");

	iterator.Write_uint32((uint32)sTrackBuffer.GetSize());
	for (uint32 i = 0; i < sTrackBuffer.GetSize(); ++i)
	{
		Debug::PrintLine(CSTR("\t\"", tTrackBuffer[i]->first, "\":\n\t{"));

		iterator.Write_string(sTrackBuffer[i]->first.GetData());

		auto keyframes = sTrackBuffer[i]->second.GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value));

			iterator.Write_float(keyframes[j].time);
			iterator.Write_vector3(keyframes[j].value);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}");
}
