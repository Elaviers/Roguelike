#pragma once
#include "Hashmap.hpp"
#include "IO.hpp"
#include "String.hpp"

template<typename T>
class AssetManager
{
private:
	Buffer<String> _paths;
	Hashmap<String, T*> _map;

	const String _defaultBinaryExtension;
	const String _defaultTextExtension;

protected:
	virtual T* _CreateResource(const String& name, const Buffer<byte>& data) { return nullptr; }
	virtual T* _CreateResource(const String& name, const String& textData) { return nullptr; }

	//Called before deleting a resource
	virtual void _DestroyResource(T* resource) {}

	AssetManager(const String &defaultBinaryExtension = "", const String &defaultTextExtension = ".txt") : 
		_defaultBinaryExtension(defaultBinaryExtension), _defaultTextExtension(defaultTextExtension)
	{ 
		_paths.Add(""); 
	}

	AssetManager(const AssetManager&) = delete;

	virtual ~AssetManager() 
	{
		Buffer<Pair<const String, T*>*> buffer = _map.ToKVBuffer();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			_DestroyResource(buffer[i]->second);
			delete buffer[i]->second;
		}
	}

	T*& _MapValue(const String& name) { return _map[name]; }

public:
	const Hashmap<String, T*>& GetMap() const { return _map; }

	//Only adds if key does not exist, otherwise returns false
	inline bool Add(const String& name, T* value) 
	{
		if (_map.Get(name))
			return false;

		_map[name] = value;
		return true;
	}

	T* Get(const String& name)
	{
		String lowerName = name.ToLower();
		String extension = Utilities::GetExtension(name);
		bool hasExtension = extension.GetLength() != 0;
		
		T** resource = _map.Get(lowerName);
		if (resource) return *resource;

		if (lowerName.GetLength() > 0)
		{
			for (size_t i = 0; i < _paths.GetSize(); ++i)
			{
				String baseName = _paths[i] + name;
				T* resource = nullptr;

				bool textOnly = _defaultTextExtension.GetLength() && extension == _defaultTextExtension;
				bool dataOnly = _defaultBinaryExtension.GetLength() && extension == _defaultBinaryExtension;

				if (!textOnly)
				{
					String filename = (hasExtension ? baseName : (baseName + _defaultBinaryExtension)).GetData();

					if (IO::FileExists(filename.GetData()))
					{
						Buffer<byte> data = IO::ReadFile(filename.GetData());

						resource = _CreateResource(lowerName, data);
					}
				}

				if (!dataOnly && resource == nullptr)
				{
					String filename = (hasExtension ? baseName : (baseName + _defaultTextExtension)).GetData();

					if (IO::FileExists(filename.GetData()))
					{
						String data = IO::ReadFileString(filename.GetData());

						resource = _CreateResource(lowerName, data);
					}
				}

				if (resource)
				{
					_map.Set(lowerName, resource);
					return resource;
				}
			}
		}

		return nullptr;
	}

	inline T* Find(const String& name)
	{
		auto found = _map.Get(name.ToLower());

		return found ? *found : nullptr;
	}

	inline String FindNameOf(const T* resource) const
	{
		String string;

		T *const &dumb = const_cast<T *const>(resource);

		const String* found = _map.FindFirstKey(dumb);
		if (found) string = *found;
		else string = "None";

		return string;
	}

	inline void SetRootPath(const String &root) { _paths[0] = root; }
	inline const String &GetRootPath() { return _paths[0]; }

	inline void AddPath(const String &root) { _paths.Add(root); }

	inline const Buffer<String>& GetPaths() { return _paths; }
};