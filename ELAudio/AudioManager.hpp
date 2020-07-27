#pragma once
#include <ELSys/AssetManager.hpp>
#include "Sampler.hpp"
#include <ELCore/FunctionPointer.hpp>
#include <ELCore/List.hpp>
#include <Windows.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>

#undef PlaySound

class AudioManager : public AssetManager<WaveSound>
{
	IMMDeviceEnumerator* _enumerator;
	IMMDevice* _audioDevice;
	IAudioClient* _audioClient;
	IAudioRenderClient* _renderClient;

	WAVEFORMATEX _waveFormat;

	uint32 _bufferFrameCount;

	List<Sampler> _playingSounds;

	virtual WaveSound* _CreateResource(const String&, const String&, const String&, const Context&) override;

	virtual void _DestroyResource(WaveSound& sound) override;

public:
	AudioManager() : AssetManager(), _waveFormat{0} {}
	~AudioManager();

	void PlaySound(const WaveSound& sound);
	void PlaySound(const String& name) 
	{
		WaveSound *found = this->Get(name, Context::Empty()).Ptr();
		if (found)
			PlaySound(*found);
	}

	void Initialise(uint32 minimumBufferDurationMillis = 100);

	void FillBuffer();
	
	void CMD_play(const Buffer<String> &args, const Context&);
};
