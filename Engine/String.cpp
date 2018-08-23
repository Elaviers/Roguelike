#include "String.h"
#include "Types.h"

String::~String()
{
	delete[] _data;
}

String::String() : _length(0)
{
	_data = new char[1];
	_data[0] = '\0';
}

//Copy constructor

String::String(const String &other) : _length(other._length)
{
	_data = new char[_length + 1];
	_data[_length] = '\0';

	for (unsigned int i = 0; i < _length; ++i)
		_data[i] = other._data[i];
}

//Move constructor

String::String(String &&string) : _data(string._data), _length(string._length)
{
	string._data = nullptr;
}

////Conversion constructors

String::String(const char *string) : _length(0)
{
	while (string[_length] != '\0')
		++_length;

	_data = new char[_length + 1];
	_data[_length] = '\0';

	for (unsigned int i = 0; i < _length; ++i)
		_data[i] = string[i];
}

String::String(char character) : _length(1)
{
	_data = new char[2];
	_data[0] = character;
	_data[1] = '\0';
}

////

String::String(unsigned int length)
{
	_length = length;
	_data = new char[_length + 1]();
}

////

inline bool IsPartOfToken(char c, const char *delimiters, unsigned int delimiterCount)
{
	if (c == '\0')
		return false;

	for (unsigned int i = 0; i < delimiterCount; ++i)
		if (c == delimiters[i])
			return false;

	return true;
}

Buffer<String> String::Split(const char *delimiters) const
{
	unsigned int delimiterCount = StringLength(delimiters);

	Buffer<String> result;

	uint32 tokenCount = 0;

	for (unsigned int i = 0; i < _length;)
	{
		if (IsPartOfToken(_data[i], delimiters, delimiterCount))
		{
			tokenCount++;
			while (IsPartOfToken(_data[++i], delimiters, delimiterCount));
		}
		else i++;
	}

	if (tokenCount)
	{
		result.SetSize(tokenCount);

		unsigned int token = 0;
		for (unsigned int i = 0; i < _length;) {
			if (IsPartOfToken(_data[i], delimiters, delimiterCount)) {
				unsigned int length = 0;
				for (unsigned int j = i; IsPartOfToken(_data[j], delimiters, delimiterCount); ++j)
					++length;

				result[token].SetLength(length);

				for (unsigned int j = 0; j < length; ++j)
					result[token][j] = _data[i + j];

				token++;
				while (IsPartOfToken(_data[++i], delimiters, delimiterCount));
			}
			else i++;
		}
	}

	return result;
}

////

void String::SetLength(unsigned int length)
{
	if (length < _length)
		_data[length] = '\0';

	_length = length;
	delete[] _data;
	_data = new char[_length + 1]();
}

////

const String& String::operator=(const String &other)
{
	_length = other._length;

	delete[] _data;
	_data = new char[_length + 1];

	for (unsigned int i = 0; i < _length; ++i)
		_data[i] = other._data[i];

	_data[_length] = '\0';
	return *this;
}

const String& String::operator=(String &&other)
{
	delete[] _data;

	_length = other._length;
	_data = other._data;
	other._data = nullptr;

	return *this;
}

const String& String::operator=(const char* string)
{
	_length = StringLength(string);

	delete[] _data;
	_data = new char[_length + 1];

	for (unsigned int i = 0; i < _length; ++i)
		_data[i] = string[i];

	_data[_length] = '\0';
	return *this;
}

const String& String::operator+=(const String &other)
{
	char *new_data = new char[_length + other._length + 1];

	for (unsigned int i = 0; i < _length; ++i)
		new_data[i] = _data[i];

	for (unsigned int i = 0; i < other._length; ++i)
		new_data[_length + i] = other._data[i];

	_length += other._length;
	new_data[_length] = '\0';

	delete[] _data;
	_data = new_data;
	return *this;
}

const String& String::operator+=(const char *string)
{
	unsigned int length = StringLength(string);
	char *new_data = new char[_length + length + 1];

	for (unsigned int i = 0; i < _length; ++i)
		new_data[i] = _data[i];

	for (unsigned int i = 0; i < length; ++i)
		new_data[_length + i] = string[i];

	_length += length;
	new_data[_length] = '\0';
	_data = new_data;
	return *this;
}

const String& String::operator+=(char character)
{
	char *new_data = new char[_length + 2];

	for (unsigned int i = 0; i < _length; ++i)
		new_data[i] = _data[i];

	new_data[_length] = character;
	++_length;
	new_data[_length] = '\0';

	delete[] _data;
	_data = new_data;
	return *this;
}

////

String String::operator+(const String& other)
{
	String product(_length + other._length);

	for (unsigned int i = 0; i < _length; ++i)
		product[i] = _data[i];
	
	for (unsigned int i = 0; i < other._length; ++i)
		product[_length + i] = other._data[i];

	return product;
}

////

int String::Compare(const String &other) const
{
	auto minLength = _length < other._length ? _length : other._length;

	for (unsigned int i = 0; i < minLength; ++i)
		if (_data[i] != other[i])
			return _data[i] < other[i] ? -1 : 1;

	if (_length != other._length)
		return _length < other._length ? -1 : 1;

	return 0;
}

////

bool String::operator==(const char *string) const
{
	for (unsigned int i = 0; i < _length; ++i)
		if (string[i] == '\0' || _data[i] != string[i])
			return false;

	return true;
}

////
#include <stdlib.h>

int String::ToInt() const
{
	return atoi(GetData());
}

float String::ToFloat() const
{
	return (float)atof(GetData());
}

Vector3 String::ToVector3() const
{
	Buffer<String> tokens = Split(",");
	Vector3 result;

	if (tokens.GetSize() >= 1)
		result[0] = tokens[0].ToFloat();
	if (tokens.GetSize() >= 2)
		result[1] = tokens[1].ToFloat();
	if (tokens.GetSize() >= 3)
		result[2] = tokens[2].ToFloat();

	return result;
}

////Conversion

String String::Convert(__int64 number, unsigned int minimum, byte base)
{
	unsigned int digit_count = 1;

	for (int64 n = number / 10; n; n /= 10)
		++digit_count;

	if (digit_count < minimum)
		digit_count = minimum;

	bool neg = false;
	if (number < 0)
	{
		number *= -1;
		neg = true;
	}

	String string(digit_count + (neg ? 1 : 0));

	for (unsigned int i = 0; i < digit_count; ++i) {
		string[digit_count - 1 - i + (neg ? 1 : 0)] = '0' + (number % base);
		number /= base;
	}

	if (neg)
		string[0] = '-';

	return string;
}

#include <math.h>

String String::ConvertFloat(double number, unsigned int minimum, byte base)
{
	if (number == INFINITY)
		return "infinity";
	if (number == NAN)
		return "NAN";

	
	int64 whole_number = (int64)number;
	double fraction = number - whole_number;

	String whole_string = Convert(whole_number, minimum, base);

	unsigned int fraction_digit_count = 0;
	for (double d = fraction; d;) {
		d *= base;
		d -= (int)d;
		++fraction_digit_count;
	}

	String fraction_string(fraction_digit_count);
	int digit;

	for (unsigned int i = 0; i < fraction_digit_count; ++i) {
		fraction *= base;

		digit = (int)fraction;
		fraction_string[i] = '0' + digit;
		fraction -= digit;
	}

	if (fraction_string.GetLength() > 0)
		return whole_string + '.' + fraction_string;

	return whole_string;
}

String String::ConvertVector3(const Vector3 &vector, unsigned int minimum, byte base)
{
	const char *seperator = ", ";
	return ConvertFloat(vector[0], minimum, base) + seperator + ConvertFloat(vector[1], minimum, base) + seperator + ConvertFloat(vector[2], minimum, base);
}

//Other

bool StringContains(const char *string, const char *phrase)
{
	for (; string[0] != '\0'; ++string)
	{
		for (unsigned int i = 0; string[i] != '\0'; ++i)
		{
			if (phrase[i] == '\0')
				return true;

			if (string[i] != phrase[i])
			{
				string += i;
				break;
			}
		}
	}

	return false;
}
