#pragma once
#include "Sound.hpp"

/*
	Sampler

	Audio Sampler
*/

class Sampler
{
	const WaveSound* _sound;

	uint32 _currentFrame;
	bool _playing;
	bool _loop;

public:
	Sampler() : _sound(nullptr), _currentFrame(0), _playing(true), _loop(true) {}
	~Sampler() {}

	void SetSound(const WaveSound& sound) { _sound = &sound; }
	void SetLooping(bool looping) { _loop = looping; }
	void SetPlaying(bool playing) { _playing = playing; }

	bool IsPlaying() const { return _playing; }

	//Reads frames to a sound buffer. Currently only supports 16-bit buffers
	uint32 ReadToSoundBuffer(byte* dest, uint32 sampleCount, uint32 destRate, uint16 destChannelCount, float mix);
};
