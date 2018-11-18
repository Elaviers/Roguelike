#include "ObjectProperties.h"

Property::Property(const Property &other) : _type(other._type), _flags(other._flags)
{
	


}

#define SETSTRINGCASE(ENUM, TYPE, VALUE) case ENUM: ((PropertyBase<TYPE>*)this)->Set(VALUE); break
void Property::SetByString(const String &value)
{
	switch (_type)
	{
		SETSTRINGCASE(PropertyType::STRING, String, value);
		SETSTRINGCASE(PropertyType::FLOAT, float, value.ToFloat());
		SETSTRINGCASE(PropertyType::VECTOR2, Vector2, value.ToVector2());
		SETSTRINGCASE(PropertyType::VECTOR3, Vector3, value.ToVector3());
	}

	//TODO: Add cases for basically everything else...
}

#define GETASTYPE(TYPE) ((PropertyBase<TYPE>*)this)->Get()
String Property::GetAsString() const
{
	switch (_type)
	{
	case PropertyType::STRING: return GETASTYPE(String);
	case PropertyType::FLOAT: return String::ConvertFloat(GETASTYPE(float));
	case PropertyType::VECTOR2: return String::ConvertVector2(GETASTYPE(Vector2));
	case PropertyType::VECTOR3: return String::ConvertVector3(GETASTYPE(Vector3));
	}

	//TODO: You know.

	return "UNSUPPORTED";
}

#define TRANSFERCASE(ENUM, TYPE) case ENUM: ((PropertyBase<TYPE>*)this)->Set(((PropertyBase<TYPE>*)this)->Get(), otherBase); break
void Property::Transfer(void *otherBase)
{
	switch (_type)
	{
		TRANSFERCASE(PropertyType::STRING, String);
		TRANSFERCASE(PropertyType::FLOAT, float);
		TRANSFERCASE(PropertyType::VECTOR2, Vector2);
		TRANSFERCASE(PropertyType::VECTOR3, Vector3);
	}
}
