#include "AudioManager.hpp"
#include "AudioUtilities.hpp"
#include "IO.hpp"
#include <ELCore/Utilities.hpp>
#include <ELMaths/Maths.hpp>
#include <ELSys/Debug.hpp>

#define TRYRELEASE(noob) if (noob != NULL) noob->Release();

AudioManager::~AudioManager()
{
	if (_audioClient) _audioClient->Stop();

	TRYRELEASE(_renderClient);
	TRYRELEASE(_audioClient);
	TRYRELEASE(_audioDevice);
	TRYRELEASE(_enumerator);
}

WaveSound* AudioManager::_CreateResource(const String& data, const String& name, const String& ext, const Context& ctx)
{
	String filename;
	float volume = 1.f;
	ESoundCategory category = ESoundCategory::GENERIC;

	Buffer<String> lines = data.ToLower().Split("\r\n");

	for (const String& line : lines)
	{
		Buffer<String> tokens = line.Split(" ");

		if (tokens[0] == "filename")
			filename = tokens[1];
		else if (tokens[0] == "volume")
			volume = tokens[1].ToFloat();
		else if (tokens[0] == "category")
		{
			if (tokens[1] == "music")
				category = ESoundCategory::MUSIC;
		}
	}

	WaveSound* sound = IO::ReadWaveFile((this->GetRootPath() + filename).GetData());
	sound->volume = volume;
	sound->category = category;

	if (sound->sampleRate != _waveFormat.nSamplesPerSec)
	{
		byte* originalData = new byte[sound->dataSize];
		Utilities::CopyBytes(sound->data, originalData, sound->dataSize);

		uint32 cutoff = (uint32)(Maths::Min<uint32>(sound->sampleRate, _waveFormat.nSamplesPerSec) / 2.f);

		AudioUtilities::LowPassFilter(
			(int16*)originalData,
			(int16*)sound->data,
			sound->channelCount,
			sound->dataSize / sound->FrameSize,
			sound->sampleRate,
			cutoff);

		delete[] originalData;

		//sound.Resample(_waveFormat.nSamplesPerSec);
	}

	return sound;
}

void AudioManager::_DestroyResource(WaveSound& sound)
{
	sound.Destroy();
}

#define CHECK(result) if (!SUCCEEDED(result)) goto Finished

void AudioManager::Initialise(uint32 minimumBufferDurationMillis)
{
	WAVEFORMATEX* descriptor = NULL;

	HRESULT result = CoInitialize(NULL);

	result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**) &_enumerator);
	CHECK(result);

	result = _enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &_audioDevice);
	CHECK(result);

	result = _audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&_audioClient);
	CHECK(result);

	REFERENCE_TIME defaultDevicePeriod, minimumDevicePeriod;

	result = _audioClient->GetDevicePeriod(&defaultDevicePeriod, &minimumDevicePeriod);
	CHECK(result);

	{
		const REFERENCE_TIME buffer_duration = Maths::Max((REFERENCE_TIME)minimumBufferDurationMillis * 10000, minimumDevicePeriod);

		result = _audioClient->GetMixFormat(&descriptor);
		CHECK(result);

		_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		_waveFormat.wBitsPerSample = 16;
		_waveFormat.nChannels = 2;
		_waveFormat.nSamplesPerSec = descriptor->nSamplesPerSec;
		_waveFormat.nBlockAlign = (_waveFormat.wBitsPerSample * _waveFormat.nChannels) / 8;
		_waveFormat.nAvgBytesPerSec = _waveFormat.nSamplesPerSec * _waveFormat.nBlockAlign;

		result = _audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, buffer_duration, 0 /*shared*/, &_waveFormat, NULL);
		CHECK(result);

		result = _audioClient->GetBufferSize(&_bufferFrameCount);
		CHECK(result);

		result = _audioClient->GetService(__uuidof(IAudioRenderClient), (void**)&_renderClient);
		CHECK(result);

		result = _audioClient->Start();
		CHECK(result);
	}

	CoTaskMemFree(descriptor);
	return;

Finished:
	CoTaskMemFree(descriptor);
	Debug::Error("Could not initialise the audio manager (this is probably a WASAPI problem...)");
}

void AudioManager::FillBuffer()
{
	UINT32 padding = 0;
	HRESULT result = _audioClient->GetCurrentPadding(&padding);

	uint32 availableFrames = _bufferFrameCount - padding;
	uint32 destFramesWritten = 0;

	CHECK(result);

	if (availableFrames)
	{
		byte* buffer;
		result = _renderClient->GetBuffer(availableFrames, &buffer);
		CHECK(result);

		ZeroMemory(buffer, (size_t)availableFrames * _waveFormat.nBlockAlign);

		List<Sampler>::Iterator it = _playingSounds.begin();

		while (it.IsValid())
		{
			List<Sampler>::Iterator next = it.Next();

			uint32 framesWritten = it->ReadToSoundBuffer(buffer, availableFrames, _waveFormat.nSamplesPerSec, _waveFormat.nChannels, .5f);

			destFramesWritten = Maths::Max(destFramesWritten, framesWritten);

			if (framesWritten == 0)
				_playingSounds.Remove(it);

			it = next;
		}

		result = _renderClient->ReleaseBuffer(destFramesWritten, 0);
		CHECK(result);
	}

	return;

Finished:
	Debug::Error("WASAPI buffer error");
}

void AudioManager::PlaySound(const WaveSound& sound)
{
	List<Sampler>::Iterator it = _playingSounds.Add(Sampler());

	it->SetSound(sound);
	it->SetLooping(false);
}

void AudioManager::CMD_play(const Buffer<String> &args, const Context& ctx)
{
	if (args.GetSize() > 0)
		this->PlaySound(args[0]);
}
