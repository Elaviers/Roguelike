#pragma once
#include "String.hpp"
#include "Types.hpp"
#include "Vector4.hpp"

class Colour
{
	Vector4 _data;

public:
	float& r = _data[0];
	float& g = _data[1];
	float& b = _data[2];
	float& a = _data[3];

	Colour() : _data(0, 0, 0, 1) {}
	Colour(const Vector4& data) : _data(data) {}
	Colour(float r, float g, float b, float a = 1.f) : _data(r, g, b, a) {}
	Colour(byte r, byte g, byte b, byte a = 255) : _data(r / 255.f, g / 255.f, b / 255.f, a / 255.f) {}
	
	Colour(const Colour& other) : _data(other._data) {}
	
	~Colour() {}

	const Vector4& GetData() { return _data; }

	operator Vector4() const { return _data; }

	Colour& operator=(const Colour& other)
	{
		_data = other._data;
		return *this;
	}

	bool operator==(const Colour& other) const { return _data == other._data; }

	String ToColourCode() const;

	static Colour FromColourCode(const char* colourCode);

	static Colour Black, Grey, White, Red, Green, Blue, Yellow, Pink, Cyan;
};