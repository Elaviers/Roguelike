#include "Animation.hpp"
#include "Skeleton.hpp"

Animation* Animation::FromData(const Buffer<byte>& data)
{
	if (data[0] == ASSET_ANIMATION)
	{
		Animation* animation = new Animation();
		ByteReader reader(data);
		animation->Read(reader);
		return animation;
	}

	return nullptr;
}

void Animation::Evaluate(Buffer<Matrix4>& skinningMatrices, const Skeleton& skeleton, float time) const
{
	skinningMatrices.SetSize(skeleton.GetJointCount());

	for (auto it = skeleton.FirstListElement(); it.IsValid(); ++it)
	{
		const Joint& j = *it;

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

		skinningMatrices[j.GetID()] = Matrix4::Transformation(translation, rotation, scale);
	}
}

void Animation::Read(ByteReader& iterator)
{
	iterator.Read_byte();

	uint32 tTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < tTrackCount; ++i)
	{
		AnimationTrack<Vector3>& track = _translationTracks[iterator.Read<String>()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Vector3 value = iterator.Read<Vector3>();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}

	uint32 rTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < rTrackCount; ++i)
	{
		AnimationTrack<Quaternion>& track = _rotationTracks[iterator.Read<String>()];

		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Quaternion value = iterator.Read<Vector3>();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}

	uint32 sTrackCount = iterator.Read_uint32();
	for (uint32 i = 0; i < sTrackCount; ++i)
	{
		AnimationTrack<Vector3>& track = _scalingTracks[iterator.Read<String>()];
		
		uint32 keyframeCount = iterator.Read_uint32();
		for (uint32 j = 0; j < keyframeCount; ++j)
		{
			float time = iterator.Read_float();
			Vector3 value = iterator.Read<Vector3>();
			byte interp = iterator.Read_byte();
			track.AddKey(time, value, interp);
		}
	}
}

void Animation::Write(ByteWriter &iterator) const
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

		iterator.Write_cstr(tTrackBuffer[i]->first.GetData());
		
		auto keyframes = tTrackBuffer[i]->second.GetKeyframes();
		
		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value));

			iterator.Write_float(keyframes[j].time);
			keyframes[j].value.Write(iterator);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}\n\nROTATION TRACKS:\n{");

	iterator.Write_uint32((uint32)rTrackBuffer.GetSize());
	for (uint32 i = 0; i < rTrackBuffer.GetSize(); ++i)
	{
		Debug::PrintLine(CSTR("\t\"", tTrackBuffer[i]->first, "\":\n\t{"));

		iterator.Write_cstr(rTrackBuffer[i]->first.GetData());

		auto keyframes = rTrackBuffer[i]->second.GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value.ToEulerYXZ()));

			iterator.Write_float(keyframes[j].time);
			keyframes[j].value.ToEulerYXZ().Write(iterator);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}\n\nSCALING TRACKS:\n{");

	iterator.Write_uint32((uint32)sTrackBuffer.GetSize());
	for (uint32 i = 0; i < sTrackBuffer.GetSize(); ++i)
	{
		Debug::PrintLine(CSTR("\t\"", tTrackBuffer[i]->first, "\":\n\t{"));

		iterator.Write_cstr(sTrackBuffer[i]->first.GetData());

		auto keyframes = sTrackBuffer[i]->second.GetKeyframes();

		iterator.Write_uint32((uint32)keyframes.GetSize());
		for (uint32 j = 0; j < keyframes.GetSize(); ++j)
		{
			Debug::PrintLine(CSTR("\t\t", keyframes[j].time, "\t", keyframes[j].value));

			iterator.Write_float(keyframes[j].time);
			keyframes[j].value.Write(iterator);
			iterator.Write_byte(keyframes[j].interpolation);
		}

		Debug::PrintLine("\t}\n");
	}

	Debug::PrintLine("}");
}
