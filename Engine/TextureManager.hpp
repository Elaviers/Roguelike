#pragma once
#include "AssetManager.hpp"
#include "Texture.hpp"

class TextureManager : public AssetManager<Texture>
{
private:
	struct _TextureManagerColours
	{
		Texture *white = nullptr;
		Texture *grey = nullptr;
		Texture *normalDefault = nullptr;

		SharedPointer<const Texture> tWhite, tGrey, tNormalDefault;
	} _colours;

	virtual Texture* _CreateResource(const String&, const Buffer<byte>&);
	virtual Texture* _CreateResource(const String&, const String&);

	byte _maxAnisotropy;
	byte _maxMipLevels;

public:
	TextureManager() : AssetManager({".png", ".tex"}), _maxAnisotropy(0), _maxMipLevels(0) {}
	
	virtual ~TextureManager()
	{
	}

	void Initialise();

	void CMD_config(const Buffer<String>& args);

	const SharedPointer<const Texture>& White() const { return _colours.tWhite; }
	const SharedPointer<const Texture>& Grey() const { return _colours.tGrey; }
	const SharedPointer<const Texture>& NormalDefault() const { return _colours.tNormalDefault; }

	byte GetMaxAnisotropy() const { return _maxAnisotropy; }
	byte GetMaxMipLevels() const { return _maxMipLevels; }
};
