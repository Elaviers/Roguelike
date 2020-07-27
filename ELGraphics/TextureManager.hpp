#pragma once
#include <ELSys/AssetManager.hpp>
#include "Texture.hpp"

class TextureManager : public AssetManager<Texture>
{
private:
	struct _TextureManagerColours
	{
		Texture *black = nullptr;
		Texture *white = nullptr;
		Texture *grey = nullptr;
		Texture *normalDefault = nullptr;

		SharedPointer<const Texture> tBlack, tWhite, tGrey, tNormalDefault;
	} _colours;

	virtual Texture* _CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context&) override;
	virtual Texture* _CreateResource(const String& textData, const String& name, const String& extension, const Context&) override;

	byte _maxAnisotropy;
	byte _maxMipLevels;

public:
	TextureManager() : AssetManager({".png", ".tex"}), _maxAnisotropy(0), _maxMipLevels(0) {}
	
	virtual ~TextureManager()
	{
	}

	void Initialise();

	const SharedPointer<const Texture>& Black() const { return _colours.tBlack; }
	const SharedPointer<const Texture>& White() const { return _colours.tWhite; }
	const SharedPointer<const Texture>& Grey() const { return _colours.tGrey; }
	const SharedPointer<const Texture>& UnitNormal() const { return _colours.tNormalDefault; }

	byte GetMaxAnisotropy() const { return _maxAnisotropy; }
	byte GetMaxMipLevels() const { return _maxMipLevels; }

	void SetMaxAnisotropy(byte maxAnisotropy) { _maxAnisotropy = maxAnisotropy; }
	void SetMaxMipLevels(byte maxMips) { _maxMipLevels = maxMips; }
};
