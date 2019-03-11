#include "AudioManager.h"
#include "Debug.h"
#include "IO.h"
#include "Utilities.h"

constexpr const char* extension = ".txt";

#define TRYRELEASE(noob) if (noob != NULL) noob->Release();

AudioManager::~AudioManager()
{
	if (_audioClient) _audioClient->Stop();

	TRYRELEASE(_renderClient);
	TRYRELEASE(_audioClient);
	TRYRELEASE(_audioDevice);
	TRYRELEASE(_enumerator);
}

bool AudioManager::_CreateResource(WaveSound& sound, const String& name, const String& data)
{
	String filename;
	float volume = 1.f;
	SoundCategory category = SoundCategory::GENERIC;

	Buffer<String> lines = data.ToLower().Split("\r\n");

	for (size_t i = 0; i < lines.GetSize(); ++i)
	{
		Buffer<String> tokens = lines[i].Split(" ");

		if (tokens[0] == "filename")
			filename = tokens[1];
		else if (tokens[0] == "volume")
			volume = tokens[1].ToFloat();
		else if (tokens[0] == "category")
		{
			if (tokens[1] == "music")
				category = SoundCategory::MUSIC;
		}
	}

	sound = IO::ReadWaveFile((this->GetRootPath() + filename).GetData());
	sound.volume = volume;
	sound.category = category;
	return true;
}

void AudioManager::_DestroyResource(WaveSound& sound)
{
	sound.Destroy();
}

#define CHECK(result) if (!SUCCEEDED(result)) goto Finished

void AudioManager::Initialise()
{
	WAVEFORMATEX* descriptor = NULL;

	CoInitialize(NULL);

	HRESULT result = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**) &_enumerator);
	CHECK(result);

	result = _enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &_audioDevice);
	CHECK(result);

	result = _audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&_audioClient);
	CHECK(result);

	REFERENCE_TIME defaultDevicePeriod, minimumDevicePeriod;

	result = _audioClient->GetDevicePeriod(&defaultDevicePeriod, &minimumDevicePeriod);
	CHECK(result);

	{
		const uint32 minimumBufferDurationMillis = 100;
		const REFERENCE_TIME buffer_duration = Utilities::Max((REFERENCE_TIME)(minimumBufferDurationMillis * 10000), minimumDevicePeriod);

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

		ZeroMemory(buffer, availableFrames * _waveFormat.nBlockAlign);

		List<Sampler>::Node* node = _playingSounds.First();

		while (node)
		{
			List<Sampler>::Node* next = node->next;

			uint32 framesWritten = node->obj.ReadToSoundBuffer(buffer, availableFrames, _waveFormat.nSamplesPerSec, _waveFormat.nChannels, 1.f);

			destFramesWritten = Utilities::Max(destFramesWritten, framesWritten);

			if (framesWritten == 0)
				_playingSounds.Remove(node);

			node = next;
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
	List<Sampler>::Node *node = _playingSounds.New();

	node->obj.SetSound(sound);
	node->obj.SetLooping(false);
}

void AudioManager::CMD_play(const Buffer<String> &args)
{
	if (args.GetSize() > 0)
		this->PlaySound(args[0]);
}
