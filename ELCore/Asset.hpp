#pragma once
#include "Buffer.hpp"
#include "String.hpp"
#include "PropertyCollection.hpp"

class ByteReader;
class ByteWriter;

enum EAssetID
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

	virtual void _ReadText(const String& string, const Context& ctx)
	{
		Buffer<String> lines = string.ToLower().Split("\r\n");
		for (const String& line : lines)
			String unused = GetProperties().HandleCommand(this, line, ctx);
	}

	virtual String _WriteText() const { return ""; }
public:
	virtual ~Asset() {}

	virtual const PropertyCollection& GetProperties() 
	{ 
		static PropertyCollection properties; 
		return properties;
	}
	
	//T Must be derived from Asset
	template<typename T>
	static T* FromText(const String &string, const Context& ctx)
	{
		Asset* ass = new T();
		ass->_ReadText(string, ctx);
		return (T*)ass;
	}

	String GetAsText() const { return _WriteText();}

	Buffer<byte> GetAsData() const;

	virtual void Read(ByteReader&) {}
	virtual void Write(ByteWriter&) const {}
};
