#pragma once
#include "ResourceManager.hpp"
#include "FunctionPointer.hpp"
#include "List.hpp"
#include "Sampler.hpp"
#include <Windows.h>
#include <Audioclient.h>
#include <mmdeviceapi.h>

#undef PlaySound

class AudioManager : public ResourceManager<WaveSound>
{
	IMMDeviceEnumerator* _enumerator;
	IMMDevice* _audioDevice;
	IAudioClient* _audioClient;
	IAudioRenderClient* _renderClient;

	WAVEFORMATEX _waveFormat;

	uint32 _bufferFrameCount;

	List<Sampler> _playingSounds;

	virtual WaveSound* _CreateResource(const String&, const String&) override;

	virtual void _DestroyResource(WaveSound& sound) override;

public:
	AudioManager() : _waveFormat{0} {}
	~AudioManager();

	void PlaySound(const WaveSound& sound);
	inline void PlaySound(const String& name) 
	{
		WaveSound *found = this->Get(name);
		if (found)
			PlaySound(*found);
	}

	void Initialise(uint32 minimumBufferDurationMillis = 100);

	void FillBuffer();
	
	void CMD_play(const Buffer<String> &args);
};