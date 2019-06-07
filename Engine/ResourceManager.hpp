#pragma once
#include "Map.hpp"
#include "IO.hpp"
#include "String.hpp"

template<typename T, bool RAW = false>
class ResourceManager
{
private:
	Buffer<String> _paths;
	Map<String, T*> _map;
	
protected:
	//If raw, then data is the filepath
	virtual T* _CreateResource(const String &name, const String &data) = 0;

	virtual void _DestroyResource(T& resource) {}

	ResourceManager() { _paths.SetSize(1); }
	virtual ~ResourceManager() {}

public:

	T* Get(const String& name)
	{
		String lowerName = name.ToLower();
		
		T** resource = _map.Find(lowerName);
		if (resource) return *resource;

		if (lowerName.GetLength() > 0)
		{
			for (size_t i = 0; i < _paths.GetSize(); ++i)
			{
				String data = RAW ? (_paths[i] + name) : IO::ReadFileString((_paths[i] + name + ".txt").GetData());

				T* resource = _CreateResource(lowerName, data);

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
		auto found = _map.Find(name.ToLower());

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