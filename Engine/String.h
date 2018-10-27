#pragma once
#include "Buffer.h"
#include "Vector.h"

bool StringsEqual(const char *a, const char *b);
bool StringsInequal(const char *a, const char *b);

class String {
private:
	char *_data;
	unsigned int _length;

public:
	~String();
	String();
	String(const String &string);
	String(String&& string);

	String(const char *string);
	String(char character);
	String(unsigned int length);

	Buffer<String> Split(const char *delimiters) const;

	void SetLength(unsigned int length);
	inline void Clear() { SetLength(0); }

	inline const char* const GetData() const { return _data; }
	inline unsigned int GetLength() const { return _length; }
	inline char& operator[](unsigned int position) const { return _data[position]; }

	String& operator=(const String&);
	String& operator=(String&&);
	String& operator=(const char*);
	String& operator+=(const String&);
	String& operator+=(const char*);
	String& operator+=(char);

	String operator+(const String&) const;

	int Compare(const String &other) const;
	inline bool operator<(const String &other) const	{ return Compare(other) < 0;	}
	inline bool operator>(const String &other) const	{ return Compare(other) > 0;	}

	inline bool operator==(const String &other) const { return StringsEqual(_data, other._data); }
	inline bool operator==(const char *other) const { return StringsEqual(_data, other); }
	inline bool operator!=(const String &other) const { return StringsInequal(_data, other._data); }
	inline bool operator!=(const char *other) const { return StringsInequal(_data, other); }

	String ToLower() const;

	int ToInt() const;
	float ToFloat() const;
	Vector2 ToVector2() const;
	Vector3 ToVector3() const;

	//
	static String Convert(__int64,	unsigned int minimunDigits = 0, byte base = 10);
	static String ConvertFloat(double, unsigned int minimumDigits = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String ConvertVector2(const Vector2&, unsigned int minimum = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String ConvertVector3(const Vector3&, unsigned int minimum = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String ConvertWideString(const wchar_t *string);
};

#define CSTR(BODY) ((String)BODY).GetData()

inline unsigned int StringLength(const char *string) {
	unsigned int length = 0;
	while (string[length] != '\0') length++;
	return length;
}

bool StringContains(const char *string, const char *phrase);
 