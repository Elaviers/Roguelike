#include "ObjectProperties.h"

#define SETSTRINGCASE(ENUM, TYPE, VALUE) case ENUM: ((PropertyBase<TYPE>*)property)->Set(VALUE); break
void PropertyPointer::SetByString(const String &value)
{
	switch (type)
	{
		SETSTRINGCASE(PropertyType::STRING, String, value);
		SETSTRINGCASE(PropertyType::FLOAT, float, value.ToFloat());
		SETSTRINGCASE(PropertyType::VECTOR3, Vector3, value.ToVector3());
	}

	//TODO: Add cases for basically everything else...
}

#define GETASTYPE(TYPE) ((PropertyBase<TYPE>*)property)->Get()
String PropertyPointer::GetAsString() const
{
	switch (type)
	{
	case PropertyType::STRING: return GETASTYPE(String);
	case PropertyType::FLOAT: return String::ConvertFloat(GETASTYPE(float));
	case PropertyType::VECTOR3: return String::ConvertVector3(GETASTYPE(Vector3));
	}

	//TODO: You know.

	return "UNSUPPORTED";
}
