#include "Property.hpp"
#include "Buffer.hpp"

#define SETSTRINGCASE(ENUM, TYPE, VALUE) case ENUM: ((VariableProperty<TYPE>*)this)->Set(obj, VALUE); break
void Property::SetAsString(void* obj, const String& value) const
{
	switch (GetType())
	{
	case PropertyType::FUNCTION:
	{
		auto func = dynamic_cast<const FunctionPropertyBase*>(this);
		if (func)	func->Call(obj, value.Split(" "));
		else		((VariableProperty<CommandPtr>*)this)->Get(obj)(value.Split(" "));
		break;
	}

		SETSTRINGCASE(PropertyType::STRING, String, value);
		SETSTRINGCASE(PropertyType::FLOAT, float, value.ToFloat());
		SETSTRINGCASE(PropertyType::VECTOR2, Vector2, value.ToVector2());
		SETSTRINGCASE(PropertyType::VECTOR3, Vector3, value.ToVector3());
		SETSTRINGCASE(PropertyType::UINT16, uint16, value.ToInt());
		SETSTRINGCASE(PropertyType::UINT32, uint32, value.ToInt());
		SETSTRINGCASE(PropertyType::UINT64, uint64, value.ToInt());
		SETSTRINGCASE(PropertyType::INT16, int16, value.ToInt());
		SETSTRINGCASE(PropertyType::INT32, int32, value.ToInt());
		SETSTRINGCASE(PropertyType::INT64, int64, value.ToInt());
		SETSTRINGCASE(PropertyType::BOOL, bool, value.ToBool());
	}
}

#define GETASTYPE(TYPE) ((VariableProperty<TYPE>*)this)->Get(obj)
#define GETSTRINGCASE(ENUM, RV) case ENUM: return (RV)
String Property::GetAsString(const void* obj) const
{
	switch (GetType())
	{
		GETSTRINGCASE(PropertyType::FUNCTION, "(COMMAND)");
		GETSTRINGCASE(PropertyType::STRING, GETASTYPE(String));
		GETSTRINGCASE(PropertyType::FLOAT, String::From(GETASTYPE(float)));
		GETSTRINGCASE(PropertyType::VECTOR2, String::From(GETASTYPE(Vector2)));
		GETSTRINGCASE(PropertyType::VECTOR3, String::From(GETASTYPE(Vector3)));
		GETSTRINGCASE(PropertyType::UINT16, String::From(GETASTYPE(uint16)));
		GETSTRINGCASE(PropertyType::UINT32, String::From(GETASTYPE(uint32)));
		GETSTRINGCASE(PropertyType::UINT64, String::From(GETASTYPE(uint64)));
		GETSTRINGCASE(PropertyType::INT16, String::From(GETASTYPE(int16)));
		GETSTRINGCASE(PropertyType::INT32, String::From(GETASTYPE(int32)));
		GETSTRINGCASE(PropertyType::INT64, String::From(GETASTYPE(int64)));
		GETSTRINGCASE(PropertyType::BOOL, String::From(GETASTYPE(bool)));
	}

	return "UNSUPPORTED";
}

#define TRANSFERCASE(ENUM, TYPE) case ENUM: ((VariableProperty<TYPE>*)this)->Set(to, ((VariableProperty<TYPE>*)this)->Get(from)); break
void Property::TransferTo(const void* from, void* to) const
{
	switch (GetType())
	{
		TRANSFERCASE(PropertyType::STRING, String);
		TRANSFERCASE(PropertyType::FLOAT, float);
		TRANSFERCASE(PropertyType::VECTOR2, Vector2);
		TRANSFERCASE(PropertyType::VECTOR3, Vector3);
		TRANSFERCASE(PropertyType::UINT16, uint16);
		TRANSFERCASE(PropertyType::UINT32, uint32);
		TRANSFERCASE(PropertyType::UINT64, uint64);
		TRANSFERCASE(PropertyType::INT16, int16);
		TRANSFERCASE(PropertyType::INT32, int32);
		TRANSFERCASE(PropertyType::INT64, int64);
		TRANSFERCASE(PropertyType::BOOL, bool);
	}
}
