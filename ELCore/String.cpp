#include "String.hpp"
#include "Buffer.hpp"
#include "ByteReader.hpp"
#include "ByteWriter.hpp"
#include "Types.hpp"

String::~String()
{
	delete[] _data;
}

String::String() : _length(0)
{
	_data = new char[1];
	_data[0] = '\0';
}

String::String(char c, size_t length) : _length(length)
{
	if (c == '\0') c = '0'; //Null characters are not allowed

	_data = new char[_length + 1];
	_data[_length] = '\0';
	
	for (int i = 0; i < _length; ++i)
		_data[i] = c;
}

//Copy constructor

String::String(const String &other) : _length(other._length)
{
	_data = new char[_length + 1];
	_data[_length] = '\0';

	for (size_t i = 0; i < _length; ++i)
		_data[i] = other._data[i];
}

//Move constructor

String::String(String &&string) noexcept : _data(string._data), _length(string._length)
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

	for (size_t i = 0; i < _length; ++i)
		_data[i] = string[i];
}

String::String(char character) : _length(1)
{
	_data = new char[2];
	_data[0] = character;
	_data[1] = '\0';
}

////

inline bool IsPartOfToken(char c, const char *delimiters, size_t delimiterCount)
{
	if (c == '\0')
		return false;

	for (size_t i = 0; i < delimiterCount; ++i)
		if (c == delimiters[i])
			return false;

	return true;
}

Buffer<String> String::Split(const char *delimiters) const
{
	size_t delimiterCount = StringLength(delimiters);

	Buffer<String> result;

	size_t tokenCount = 0;

	for (size_t i = 0; i < _length;)
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

		size_t token = 0;
		for (size_t i = 0; i < _length;) {
			if (IsPartOfToken(_data[i], delimiters, delimiterCount)) {
				size_t length = 0;
				for (size_t j = i; IsPartOfToken(_data[j], delimiters, delimiterCount); ++j)
					++length;

				result[token]._SetLength(length);

				for (size_t j = 0; j < length; ++j)
					result[token][j] = _data[i + j];

				token++;
				while (IsPartOfToken(_data[++i], delimiters, delimiterCount));
			}
			else i++;
		}
	}

	return result;
}

/*
	Returns substring with all elements greater than or equal to the start and less than the end
*/
String String::SubString(size_t start, size_t end) const
{
	if (start > _length)
		return "";

	if (end > _length)
		end = _length;

	String string;
	string._SetLength(end - start);
	for (size_t i = start; i < end; ++i)
		string._data[i - start] = _data[i];

	return string;
}

int String::IndexOf(char c) const
{
	for (size_t i = 0; i < _length; ++i)
		if (_data[i] == c)
			return (int)i;

	return -1;
}

int String::IndexOfAny(const char* chars) const
{
	for (size_t i = 0; i < _length; ++i)
		for (const char* c = chars; *c != '\0'; ++c)
			if (_data[i] == *c)
				return (int)i;

	return -1;
}

////

void String::_SetLength(size_t length)
{
	size_t minLength = length < _length ? length : _length;

	char *new_data = new char[length + 1]();

	for (size_t i = 0; i < minLength; ++i)
		new_data[i] = _data[i];

	delete[] _data;
	_length = length;
	_data = new_data;
}
////

char& String::Insert(char c, size_t index)
{
	if (index > _length)
		index = _length;

	char* new_data = new char[_length + 2];

	for (size_t i = 0; i < index; ++i)
		new_data[i] = _data[i];

	new_data[index] = c;

	for (size_t i = index; i < _length; ++i)
		new_data[i + 1] = _data[i];

	delete[] _data;
	_data = new_data;
	_length++;
	_data[_length] = '\0';

	return _data[index];
}

////

void String::Lower()
{
	for (int i = 0; i < _length; ++i)
		_data[i] = LowerChar(_data[i]);
}

////

String& String::operator=(const String &other)
{
	_length = other._length;

	delete[] _data;
	_data = new char[_length + 1];

	for (size_t i = 0; i < _length; ++i)
		_data[i] = other._data[i];

	_data[_length] = '\0';
	return *this;
}

String& String::operator=(String &&other) noexcept
{
	delete[] _data;

	_length = other._length;
	_data = other._data;
	other._data = nullptr;

	return *this;
}

String& String::operator=(const char* string)
{
	_length = StringLength(string);

	delete[] _data;
	_data = new char[_length + 1];

	for (size_t i = 0; i < _length; ++i)
		_data[i] = string[i];

	_data[_length] = '\0';
	return *this;
}

String& String::operator+=(const String &other)
{
	char *new_data = new char[_length + other._length + 1];

	for (size_t i = 0; i < _length; ++i)
		new_data[i] = _data[i];

	for (size_t i = 0; i < other._length; ++i)
		new_data[_length + i] = other._data[i];

	_length += other._length;
	new_data[_length] = '\0';

	delete[] _data;
	_data = new_data;
	return *this;
}

String& String::operator+=(const char *string)
{
	size_t length = StringLength(string);
	char *new_data = new char[_length + length + 1];

	for (size_t i = 0; i < _length; ++i)
		new_data[i] = _data[i];

	for (size_t i = 0; i < length; ++i)
		new_data[_length + i] = string[i];

	_length += length;
	new_data[_length] = '\0';
	_data = new_data;
	return *this;
}

String& String::operator+=(char character)
{
	char *new_data = new char[_length + 2];

	for (size_t i = 0; i < _length; ++i)
		new_data[i] = _data[i];

	new_data[_length] = character;
	++_length;
	new_data[_length] = '\0';

	delete[] _data;
	_data = new_data;
	return *this;
}

////

String String::operator+(const String& other) const
{
	String product;
	product._SetLength(_length + other._length);

	for (size_t i = 0; i < _length; ++i)
		product[i] = _data[i];

	for (size_t i = 0; i < other._length; ++i)
		product[_length + i] = other._data[i];

	return product;
}

////

int String::Compare(const String &other) const
{
	size_t minLength = _length < other._length ? _length : other._length;

	for (size_t i = 0; i < minLength; ++i)
		if (_data[i] != other[i])
			return _data[i] < other[i] ? -1 : 1;

	if (_length != other._length)
		return _length < other._length ? -1 : 1;

	return 0;
}

////

String String::ToLower() const
{
	String string(*this);
	string.Lower();
	return string;
}

////

void String::Read(ByteReader& reader)
{
	//todo- optimise
	byte next;

	while (next = reader.Read_byte())
		*this += next;
}

void String::Write(ByteWriter& writer) const
{
	writer.EnsureSpace(_length + 1);

	byte* const dest = writer.Ptr();
	dest[_length] = '\0';

	for (unsigned int i = 0; i < _length; ++i)
		dest[i] = _data[i];

	writer.IncrementIndex(_length + 1);
}


////

#include <stdlib.h>

bool String::ToBool() const
{
	String lower = ToLower();

	if (lower == "t" || lower == "true" || ToInt() != 0)
		return true;

	return false;
}

int String::ToInt() const
{
	return atoi(_data);
}

float String::ToFloat() const
{
	return (float)atof(_data);
}

/*
Vector2 String::ToVector2() const
{
	Buffer<String> tokens = Split(",");
	Vector2 result;

	if (tokens.GetSize() >= 1)
		result.x = tokens[0].ToFloat();
	if (tokens.GetSize() >= 2)
		result.y = tokens[1].ToFloat();

	return result;
}

Vector3 String::ToVector3() const
{
	Buffer<String> tokens = Split(",");
	Vector3 result;

	if (tokens.GetSize() >= 1)
		result.x = tokens[0].ToFloat();
	if (tokens.GetSize() >= 2)
		result.y = tokens[1].ToFloat();
	if (tokens.GetSize() >= 3)
		result.z = tokens[2].ToFloat();

	return result;
}

Vector4 String::ToVector4() const
{
	Buffer<String> tokens = Split(",");
	Vector4 result;

	if (tokens.GetSize() >= 1)
		result.x = tokens[0].ToFloat();
	if (tokens.GetSize() >= 2)
		result.y = tokens[1].ToFloat();
	if (tokens.GetSize() >= 3)
		result.z = tokens[2].ToFloat();
	if (tokens.GetSize() >= 4)
		result.w = tokens[3].ToFloat();

	return result;
}
*/

////Conversion

const char* ALPHA = "0123456789ABCDEFGHIJKLMNOP";

String String::FromInt(int64 number, unsigned int minimum, byte base)
{
	size_t digit_count = 1;

	for (int64 n = number / base; n; n /= base)
		++digit_count;

	if (digit_count < minimum)
		digit_count = minimum;

	bool neg = false;
	if (number < 0)
	{
		number *= -1;
		neg = true;
	}

	String string;
	string._SetLength(digit_count + (neg ? 1 : 0));

	for (size_t i = 0; i < digit_count; ++i) {
		string[digit_count - 1 - i + (neg ? 1 : 0)] = ALPHA[number % base];
		number /= base;
	}

	if (neg)
		string[0] = '-';

	return string;
}

#include <math.h>

String String::FromFloat(double number, unsigned int minimum, unsigned int maxDecimal, byte base)
{
	if (number == INFINITY)
		return "infinity";

#pragma warning(push)
#pragma warning(disable : 26451)
	if (number == NAN)
#pragma warning(pop)
		return "NAN";

	int64 whole_number = (int64)number;
	double fraction = number - whole_number;

	String whole_string;

	//This is here because negative numbers with a whole part of zero would return as if positive otherwise
	if (number < 0.0)
		whole_string = String('-') + FromInt(-whole_number, minimum, base);
	else
		whole_string = FromInt(whole_number, minimum, base);

	size_t fraction_digit_count = 0;
	for (double d = fraction; d != 0.f && fraction_digit_count < maxDecimal;) {
		d *= base;
		d -= (int)d;
		++fraction_digit_count;
	}

	String fraction_string;
	fraction_string._SetLength(fraction_digit_count);
	byte digit;

	if (fraction < 0.0) fraction *= -1.0;

	for (size_t i = 0; i < fraction_digit_count; ++i) {
		fraction *= base;

		digit = (byte)fraction;
		fraction_string[i] = ALPHA[digit];
		fraction -= digit;
	}

	if (fraction_string.GetLength() > 0)
		return whole_string + '.' + fraction_string;

	return whole_string;
}

/*
String String::From(const Vector2 &vector, unsigned int minimum, unsigned int maxDecimal, byte base)
{
	const char *seperator = ", ";
	return From(vector.x, minimum, maxDecimal, base) + seperator + From(vector.y, minimum, maxDecimal, base);
}

String String::From(const Vector3 &vector, unsigned int minimum, unsigned int maxDecimal, byte base)
{
	const char *seperator = ", ";
	return From(vector.x, minimum, maxDecimal, base) + seperator + From(vector.y, minimum, maxDecimal, base) + seperator + From(vector.z, minimum, maxDecimal, base);
}

String String::From(const Vector4& vector, unsigned int minimum, unsigned int maxDecimal, byte base)
{
	const char* seperator = ", ";
	return From(vector.x, minimum, maxDecimal, base) + seperator + From(vector.y, minimum, maxDecimal, base) + seperator + From(vector.z, minimum, maxDecimal, base) + seperator + From(vector.w, minimum, maxDecimal, base);
}

String String::From(const Matrix4& matrix, unsigned minimum, unsigned int maxDecimal, byte base)
{
	String result;
	const char* rowSeperator = ", ";
	const char* columnSeperator = "\n";
	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
		{
			result += From(matrix[r][c], minimum, maxDecimal, base);
			if (r < 3)
				result += rowSeperator;
			else if (c < 3)
				result += columnSeperator;
		}

	return result;
}
*/

String String::FromWide(const wchar_t *string)
{
	size_t i = 0;

	while (string[++i] != 0x0000);
	String result;
	result._SetLength(i);

	i = 0;
	do
	{
		if (string[i] <= 0xFF)
			result[i] = (char)string[i];
		else
			result[i] = '?';

	} while (string[++i] != '\0');

	return result;
}

//Other

bool StringsInequal(const char *a, const char *b)
{
	for (;; ++a, ++b)
	{
		if (*a != *b)
			return true;

		if (*a == '\0')
			return false;
	}
}

bool StringsEqual(const char *a, const char *b)
{
	return !StringsInequal(a, b);
}

bool StringsEqual_CaseInsensitive(const char* a, const char* b)
{
	for (; ; ++a, ++b)
	{
		if (LowerChar(*a) != LowerChar(*b))
			return false;
		
		if (*a == '\0')
			return true;
	}
}

bool StringContains(const char *string, const char *phrase)
{
	for (; string[0] != '\0'; ++string)
	{
		for (size_t i = 0; string[i] != '\0'; ++i)
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
