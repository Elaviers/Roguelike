#pragma once
#include "Buffer.hpp"
#include "BufferIterator.hpp"
#include "String.hpp"
#include "PropertyCollection.hpp"

enum AssetID
{
	ASSET_TEXTURE			= 1,
	ASSET_MATERIAL_SURFACE	= 10,
	ASSET_MATERIAL_GRID		= 11,
	ASSET_MATERIAL_FONT		= 12,
	ASSET_MESH_STATIC		= 20,
	ASSET_MESH_SKELETAL		= 21,
	ASSET_MODEL				= 30,
	ASSET_ANIMATION			= 40
};

class Asset
{
protected:
	Asset() {}

	virtual void _ReadText(const String& string)
	{
		Buffer<String> lines = string.ToLower().Split("\r\n");
		for (size_t i = 0; i < lines.GetSize(); ++i)
			String unused = GetProperties().HandleCommand(this, lines[i]);
	}

	virtual void _ReadData(BufferReader<byte>&) {}

	virtual String _WriteText() const { return ""; }
	virtual void _WriteData(BufferWriter<byte>&) const {}
public:
	virtual ~Asset() {}

	virtual const PropertyCollection& GetProperties() 
	{ 
		static PropertyCollection properties; 
		return properties;
	}
	
	//T Must be derived from Asset
	template<typename T>
	static T* FromText(const String &string)
	{
		Asset* ass = new T();
		ass->_ReadText(string);
		return (T*)ass;
	}

	String GetAsText() const { return _WriteText();}

	Buffer<byte> GetAsData() const
	{
		Buffer<byte> data;
		BufferWriter<byte> writer(data);
		_WriteData(writer);
		return data;
	}
};
