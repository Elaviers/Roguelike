#pragma once
#include <ELCore/Hashmap.hpp>
#include <ELCore/IteratorUtils.hpp>

template <typename T, typename ID_TYPE>
class Register
{
	List<T> _types;
	Buffer<T*> _lut;

protected:
	virtual ID_TYPE _GetTypeID(const T& type) const = 0;
	virtual String _GetTypeName(const T& type) const = 0;

public:
	Register()
	{
		if (sizeof(ID_TYPE) <= 8)
			_lut.SetSize(256);
	}

	Register(const Register&) = delete; //this would mess up the lookup table and is unnecessary
	Register(Register&&) = delete;

	void RegisterType(const T& type)
	{
		ID_TYPE id = _GetTypeID(type);
		T* ptr = GetType(id);

		if (ptr)
		{
			Debug::Error(CSTR("Type \"", _GetTypeName(type), "\" attempted to replace an existing registry entity!"));
			return;
		}

		ptr = &_types.EmplaceBack(type);
		if (_lut.GetSize()) _lut[(int)id] = ptr;
	}

	T* GetType(ID_TYPE id)
	{
		T* ptr = nullptr;
		if (_lut.GetSize()) ptr = _lut[(int)id];
		else
		{
			auto find = IteratorUtils::FirstWhere(_types.begin(), _types.end(), [this, id](const auto& it) { return _GetTypeID(*it) == id; });
			if (find != _types.end())
				ptr = &*find;
		}

		return ptr; 
	}

	const T* GetType(ID_TYPE id) const { return const_cast<Register<T, ID_TYPE>*>(this)->GetType(id); }

	const List<T>& GetTypes() const { return _types; }
};
