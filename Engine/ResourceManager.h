#pragma once
#include "Map.h"
#include "String.h"

template<typename T>
class ResourceManager
{
protected:
	String _rootPath;
	Map<String, T> _map;

	ResourceManager() {}
	virtual ~ResourceManager() {}
public:
	inline const T* Find(const String &name) { return _map.Find(name.ToLower()); };

	inline String FindNameOf(const T& resource) const
	{
		String string;

		const String* found = _map.FindFirstKey(resource);
		if (found) string = *found;
		else string = "None";

		return string;
	}

	inline void SetRootPath(const char *root) { _rootPath = root; }
	inline const String &GetRootPath() { return _rootPath; }
};