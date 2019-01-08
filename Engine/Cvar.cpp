#include "CVar.h"

#define SETSTRINGCASE(ENUM, TYPE, VALUE) case ENUM: ((TypedCvar<TYPE>*)this)->Set(VALUE); break
void Cvar::SetByString(const String &value)
{
	switch (_type)
	{
		SETSTRINGCASE(CvarType::STRING, String, value);
		SETSTRINGCASE(CvarType::FLOAT, float, value.ToFloat());
		SETSTRINGCASE(CvarType::VECTOR2, Vector2, value.ToVector2());
		SETSTRINGCASE(CvarType::VECTOR3, Vector3, value.ToVector3());
		SETSTRINGCASE(CvarType::UINT16, uint16, value.ToInt());
		SETSTRINGCASE(CvarType::UINT32, uint32, value.ToInt());
		SETSTRINGCASE(CvarType::UINT64, uint64, value.ToInt());
		SETSTRINGCASE(CvarType::INT16, int16, value.ToInt());
		SETSTRINGCASE(CvarType::INT32, int32, value.ToInt());
		SETSTRINGCASE(CvarType::INT64, int64, value.ToInt());
	}
}

#define GETASTYPE(TYPE) ((TypedCvar<TYPE>*)this)->Get()
String Cvar::GetAsString() const
{
	switch (_type)
	{
	case CvarType::STRING:	return GETASTYPE(String);
	case CvarType::FLOAT:	return String::FromFloat(GETASTYPE(float));
	case CvarType::VECTOR2: return String::FromVector2(GETASTYPE(Vector2));
	case CvarType::VECTOR3: return String::FromVector3(GETASTYPE(Vector3));
	case CvarType::UINT16:	return String::FromInt(GETASTYPE(uint16));
	case CvarType::UINT32:	return String::FromInt(GETASTYPE(uint32));
	case CvarType::UINT64:	return String::FromInt(GETASTYPE(uint64));
	case CvarType::INT16:	return String::FromInt(GETASTYPE(int16));
	case CvarType::INT32:	return String::FromInt(GETASTYPE(int32));
	case CvarType::INT64:	return String::FromInt(GETASTYPE(int64));
	}

	return "UNSUPPORTED";
}

#define TRANSFERCASE(ENUM, TYPE) case ENUM: ((TypedCvar<TYPE>*)&other)->Set(((TypedCvar<TYPE>*)this)->Get()); break
void Cvar::TransferTo(Cvar &other)
{
	switch (_type)
	{
		TRANSFERCASE(CvarType::STRING, String);
		TRANSFERCASE(CvarType::FLOAT, float);
		TRANSFERCASE(CvarType::VECTOR2, Vector2);
		TRANSFERCASE(CvarType::VECTOR3, Vector3);
	}
}
