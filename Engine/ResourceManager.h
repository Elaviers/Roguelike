#pragma once
#include "Map.h"
#include "IO.h"
#include "String.h"

template<typename T, bool RAW = false>
class ResourceManager
{
private:
	Buffer<String> _paths;
	Map<String, T> _map;
	
protected:
	//If raw, then data is the filepath
	virtual bool _CreateResource(T &resource, const String &name, const String &data) = 0;

	virtual void _DestroyResource(T& resource) {}

	ResourceManager() { _paths.SetSize(1); }
	virtual ~ResourceManager() {}

public:

	T* Get(const String& name)
	{
		String lowerName = name.ToLower();
		
		T* resource = _map.Find(lowerName);
		if (resource) return resource;

		if (lowerName.GetLength() > 0)
		{
			for (size_t i = 0; i < _paths.GetSize(); ++i)
			{
				T resource;

				String data = RAW ? (_paths[i] + name) : IO::ReadFileString((_paths[i] + name + ".txt").GetData());

				if (_CreateResource(resource, lowerName, data))
					return &_map.Set(lowerName, std::move(resource));
			}
		}

		return nullptr;
	}

	inline T* Find(const String &name) { return _map.Find(name.ToLower()); };

	inline String FindNameOf(const T& resource) const
	{
		String string;

		const String* found = _map.FindFirstKey(resource);
		if (found) string = *found;
		else string = "None";

		return string;
	}

	inline void SetRootPath(const String &root) { _paths[0] = root; }
	inline const String &GetRootPath() { return _paths[0]; }

	inline void AddPath(const String &root) { _paths.Add(root); }

	inline const Buffer<String>& GetPaths() { return _paths; }
};