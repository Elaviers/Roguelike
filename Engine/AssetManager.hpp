#pragma once
#include "Hashmap.hpp"
#include "IO.hpp"
#include "SharedPointer.hpp"
#include "String.hpp"

template<typename T>
class AssetManager
{
private:
	Buffer<String> _paths;
	Hashmap<String, SharedPointerData<T>> _map;

	const Buffer<String> _textExtensions;
	const Buffer<String> _binaryExtensions;

protected:
	virtual T* _CreateResource(const String& name, const Buffer<byte>& data) { return nullptr; }
	virtual T* _CreateResource(const String& name, const String& textData) { return nullptr; }

	//Called before deleting a resource
	virtual void _DestroyResource(T& resource) {}

	AssetManager(const Buffer<String>& binaryExtensions = { "" }, const Buffer<String>& textExtensions = { ".txt" }) :
		_textExtensions(textExtensions), _binaryExtensions(binaryExtensions)
	{ 
		_paths.Add(""); 
	}

	AssetManager(const AssetManager&) = delete;

	virtual ~AssetManager() 
	{
		_DestroyAll();
	}

	void _DestroyAll()
	{
		Buffer<Pair<const String, SharedPointerData<T>>*> buffer = _map.ToKVBuffer();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			_DestroyResource(*buffer[i]->second.GetPtr());
			delete buffer[i]->second.GetPtr();
			buffer[i]->second.SetPtr(nullptr);
		}
	}

	SharedPointerData<T>& _MapValue(const String& name) 
	{
		SharedPointerData<T>* found = _map.Get(name);

		if (found)
			return *found;

		return _map.Set(name, SharedPointerData<T>(nullptr, 0)); 
	}

public:
	virtual void Initialise() {}

	void UnloadAssets()
	{
		_DestroyAll();
		Initialise();

		//Reload all assets that are still in use
		Buffer<Pair<const String, SharedPointerData<T>>*> buffer = _map.ToKVBuffer();
		for (size_t i = 0; i < buffer.GetSize(); ++i)
		{
			if (buffer[i]->second.GetReferenceCount())
				auto loadNewAsset = Get(buffer[i]->first);
		}
	}

	const Hashmap<String, SharedPointerData<T>>& GetMap() const { return _map; }

	//Only adds if key does not exist, otherwise returns false
	SharedPointer<T> Add(const String& name, T* value) 
	{
		if (_map.Get(name))
			return SharedPointer<T>();

		return _map.Set(name, SharedPointerData<T>(value, 0));
	}

	SharedPointer<T> Get(const String& name)
	{
		if (name.GetLength() == 0)
			return SharedPointer<T>();

		String lowerName = name.ToLower();

		SharedPointerData<T>* existing = _map.Get(lowerName);
			
		if (existing && existing->GetPtr()) 
			return SharedPointer<T>(*existing);

		String extension = Utilities::GetExtension(name);
		bool extIsText = false;
		bool extIsBinary = false;

		if (extension.GetLength())
		{
			for (size_t i = 0; i < _textExtensions.GetSize(); ++i)
			{
				if (extension == _textExtensions[i])
				{
					extIsText = true;
					break;
				}
			}

			for (size_t i = 0; i < _binaryExtensions.GetSize(); ++i)
			{
				if (extension == _binaryExtensions[i])
				{
					extIsBinary = true;
					break;
				}
			}

			if (!extIsText && !extIsBinary)
				return SharedPointer<T>();
		}

		T* resource = nullptr;

		for (size_t i = 0; i < _paths.GetSize(); ++i)
		{
			String baseName = _paths[_paths.GetSize() - 1 - i] + name;

			if (!extIsBinary)
			{
				if (extIsText)
				{
					if (IO::FileExists(baseName.GetData()))
						resource = _CreateResource(lowerName, IO::ReadFileString(baseName.GetData()));
						
					break;
				}

				for (size_t i = 0; i < _textExtensions.GetSize(); ++i)
				{
					String filename = baseName + _textExtensions[i];

					if (IO::FileExists(filename.GetData()))
					{
						resource = _CreateResource(lowerName, IO::ReadFileString(filename.GetData()));
						break;
					}
				}
			}

			if (!extIsText && resource == nullptr)
			{
				if (extIsBinary)
				{
					if (IO::FileExists(baseName.GetData()))
						resource = _CreateResource(lowerName, IO::ReadFile(baseName.GetData()));
						
					break;
				}

				for (size_t i = 0; i < _binaryExtensions.GetSize(); ++i)
				{
					String filename = baseName + _binaryExtensions[i];

					if (IO::FileExists(filename.GetData()))
					{
						resource = _CreateResource(lowerName, IO::ReadFile(filename.GetData()));
						break;
					}
				}
			}
		}

		if (resource)
		{
			if (existing)
			{
				existing->SetPtr(resource);
				return SharedPointer<T>(*existing);
			}
			
			return SharedPointer<T>(_map.Set(lowerName, SharedPointerData<T>(resource, 0)));
		}

		return SharedPointer<T>();
	}

	SharedPointer<T> Find(const String& name)
	{
		SharedPointerData<T>* found = _map.Get(name.ToLower());

		return found ? SharedPointer<T>(*found) : SharedPointer<T>();
	}

	String FindNameOf(const T* resource) const
	{
		Buffer<Pair<const String, const SharedPointerData<T>>*> buffer = _map.ToKVBuffer();

		for (size_t i = 0; i < buffer.GetSize(); ++i)
			if (buffer[i]->second.GetPtr() == resource)
				return buffer[i]->first;

		return "None";
	}

	void SetRootPath(const String &root) { _paths[0] = root; }
	const String &GetRootPath() { return _paths[0]; }

	void AddPath(const String &root) { _paths.Add(root); }

	const Buffer<String>& GetPaths() { return _paths; }

	Buffer<String> GetAllPossibleKeys()
	{
		Buffer<String> results;

		Buffer<const String*> keys = _map.ToKBuffer();

		for (size_t i = 0; i < keys.GetSize(); ++i)
			results.OrderedAdd(*keys[i]);

		for (size_t pathIndex = 0; pathIndex < _paths.GetSize(); ++pathIndex)
		{
			Buffer<String> filenames = IO::FindFilesInDirectory(CSTR(_paths[pathIndex], "*.*"));

			for (size_t filenameIndex = 0; filenameIndex < filenames.GetSize(); ++filenameIndex)
			{
				int extIndex = filenames[filenameIndex].IndexOf('.');
				if (extIndex >= 0)
				{
					String filename = filenames[filenameIndex].ToLower();
					String name = filename.SubString(0, extIndex);
					String ext = filename.SubString(extIndex, filename.GetLength());

					bool noExtension = false;
					for (size_t i = 0; i < _textExtensions.GetSize(); ++i)
						if (ext == _textExtensions[i])
						{
							noExtension = true;
							break;
						}

					if (!noExtension)
					{
						for (size_t i = 0; i < _binaryExtensions.GetSize(); ++i)
							if (ext == _binaryExtensions[i])
							{
								noExtension = true;
								break;
							}
					}

					if (noExtension && results.IndexOf(name) == -1)
						results.OrderedAdd(name);
				}
			}
		}

		return results;
	}
};