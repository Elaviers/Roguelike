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
	//If there r
	virtual T* _CreateResource(const String& name, const Buffer<byte>& data) { return nullptr; }
	virtual T* _CreateResource(const String& name, const String& textData) { return nullptr; }

	//Called before deleting a resource
	virtual void _DestroyResource(T& resource) {}

	AssetManager(const String &defaultBinaryExtension = "", const String &defaultTextExtension = ".txt") : 
		_defaultBinaryExtension(defaultBinaryExtension), _defaultTextExtension(defaultTextExtension)
	{ 
		_paths.Add(""); 
	}

	AssetManager(const AssetManager&) = delete;

	virtual ~AssetManager() 
	{
		auto buffer = _map.ToBuffer();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			auto keys = buffer[i];

			for (auto node = keys->First(); node; node = node->next)
			{
				_DestroyResource(*node->obj.second);
				delete node->obj.second;
			}
		}
	}

public:

	T* Get(const String& name)
	{
		String lowerName = name.ToLower();
		bool hasExtension = Utilities::GetExtension(name).GetLength() != 0;
		
		T** resource = _map.Get(lowerName);
		if (resource) return *resource;

		if (lowerName.GetLength() > 0)
		{
			for (size_t i = 0; i < _paths.GetSize(); ++i)
			{
				String baseName = _paths[i] + name;
				T* resource = nullptr;

				if (hasExtension || _defaultTextExtension.GetLength() != 0)
				{
					String filename = (hasExtension ? baseName : (baseName + _defaultBinaryExtension)).GetData();

					if (IO::FileExists(filename.GetData()))
					{
						Buffer<byte> data = IO::ReadFile(filename.GetData());

						resource = _CreateResource(lowerName, data);
					}
				}

				if (resource == nullptr && (hasExtension || _defaultTextExtension.GetLength() != 0))
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