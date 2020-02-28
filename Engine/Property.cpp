#include "Property.hpp"
#include "Buffer.hpp"

#define TYPESTRINGCASE(ENUM, STRING) case ENUM: { static String str = STRING; return str; }
const String& Property::GetTypeString() const
{
	switch (GetType())
	{
		TYPESTRINGCASE(EPropertyType::FUNCTION,	"Command");
		TYPESTRINGCASE(EPropertyType::UINT16,	"uint16");
		TYPESTRINGCASE(EPropertyType::UINT32,	"uint32");
		TYPESTRINGCASE(EPropertyType::UINT64,	"uint64");
		TYPESTRINGCASE(EPropertyType::INT16,	"int16");
		TYPESTRINGCASE(EPropertyType::INT32,	"int32");
		TYPESTRINGCASE(EPropertyType::INT64,	"int64");
		TYPESTRINGCASE(EPropertyType::BYTE,		"byte");
		TYPESTRINGCASE(EPropertyType::FLOAT,	"float");
		TYPESTRINGCASE(EPropertyType::VECTOR2,	"Vector2");
		TYPESTRINGCASE(EPropertyType::VECTOR3,	"Vector3");
		TYPESTRINGCASE(EPropertyType::VECTOR4,	"Vector4");
		TYPESTRINGCASE(EPropertyType::STRING,	"String");
	}

	static String unknown = "???";
	return unknown;
}

#define SETSTRINGCASE(ENUM, TYPE, VALUE) case ENUM: ((VariableProperty<TYPE>*)this)->Set(obj, VALUE); break
void Property::SetAsString(void* obj, const String& value) const
{
	switch (GetType())
	{
	case EPropertyType::FUNCTION:
	{
		auto func = dynamic_cast<const FunctionPropertyBase*>(this);
		if (func)	func->Call(obj, value.Split(" "));
		else		((VariableProperty<CommandPtr>*)this)->Get(obj)(value.Split(" "));
		break;
	}

		SETSTRINGCASE(EPropertyType::STRING, String, value);
		SETSTRINGCASE(EPropertyType::FLOAT, float, value.ToFloat());
		SETSTRINGCASE(EPropertyType::VECTOR2, Vector2, value.ToVector2());
		SETSTRINGCASE(EPropertyType::VECTOR3, Vector3, value.ToVector3());
		SETSTRINGCASE(EPropertyType::VECTOR4, Vector4, value.ToVector4());
		SETSTRINGCASE(EPropertyType::BYTE, byte, value.ToInt());
		SETSTRINGCASE(EPropertyType::UINT16, uint16, value.ToInt());
		SETSTRINGCASE(EPropertyType::UINT32, uint32, value.ToInt());
		SETSTRINGCASE(EPropertyType::UINT64, uint64, value.ToInt());
		SETSTRINGCASE(EPropertyType::INT16, int16, value.ToInt());
		SETSTRINGCASE(EPropertyType::INT32, int32, value.ToInt());
		SETSTRINGCASE(EPropertyType::INT64, int64, value.ToInt());
		SETSTRINGCASE(EPropertyType::BOOL, bool, value.ToBool());
	}
}

#define GETASTYPE(TYPE) ((VariableProperty<TYPE>*)this)->Get(obj)
#define GETSTRINGCASE(ENUM, RV) case ENUM: return (RV)
String Property::GetAsString(const void* obj) const
{
	switch (GetType())
	{
		GETSTRINGCASE(EPropertyType::FUNCTION, "(COMMAND)");
		GETSTRINGCASE(EPropertyType::STRING, GETASTYPE(String));
		GETSTRINGCASE(EPropertyType::FLOAT, String::From(GETASTYPE(float)));
		GETSTRINGCASE(EPropertyType::VECTOR2, String::From(GETASTYPE(Vector2)));
		GETSTRINGCASE(EPropertyType::VECTOR3, String::From(GETASTYPE(Vector3)));
		GETSTRINGCASE(EPropertyType::VECTOR4, String::From(GETASTYPE(Vector4)));
		GETSTRINGCASE(EPropertyType::BYTE, String::From((int)GETASTYPE(byte)));
		GETSTRINGCASE(EPropertyType::UINT16, String::From(GETASTYPE(uint16)));
		GETSTRINGCASE(EPropertyType::UINT32, String::From(GETASTYPE(uint32)));
		GETSTRINGCASE(EPropertyType::UINT64, String::From(GETASTYPE(uint64)));
		GETSTRINGCASE(EPropertyType::INT16, String::From(GETASTYPE(int16)));
		GETSTRINGCASE(EPropertyType::INT32, String::From(GETASTYPE(int32)));
		GETSTRINGCASE(EPropertyType::INT64, String::From(GETASTYPE(int64)));
		GETSTRINGCASE(EPropertyType::BOOL, String::From(GETASTYPE(bool)));
	}

	return "UNSUPPORTED";
}

#define TRANSFERCASE(ENUM, TYPE) case ENUM: ((VariableProperty<TYPE>*)this)->Set(to, ((VariableProperty<TYPE>*)this)->Get(from)); break
void Property::TransferTo(const void* from, void* to) const
{
	if (GetFlags() & PropertyFlags::READONLY)
		return;

	switch (GetType())
	{
		TRANSFERCASE(EPropertyType::STRING, String);
		TRANSFERCASE(EPropertyType::FLOAT, float);
		TRANSFERCASE(EPropertyType::VECTOR2, Vector2);
		TRANSFERCASE(EPropertyType::VECTOR3, Vector3);
		TRANSFERCASE(EPropertyType::VECTOR4, Vector4);
		TRANSFERCASE(EPropertyType::BYTE, byte);
		TRANSFERCASE(EPropertyType::UINT16, uint16);
		TRANSFERCASE(EPropertyType::UINT32, uint32);
		TRANSFERCASE(EPropertyType::UINT64, uint64);
		TRANSFERCASE(EPropertyType::INT16, int16);
		TRANSFERCASE(EPropertyType::INT32, int32);
		TRANSFERCASE(EPropertyType::INT64, int64);
		TRANSFERCASE(EPropertyType::BOOL, bool);
	}
}
