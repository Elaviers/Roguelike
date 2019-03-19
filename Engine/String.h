#pragma once
#include "Types.h"
#include "Vector.h"

template<typename T>
class Buffer;

bool StringsEqual(const char *a, const char *b);
bool StringsInequal(const char *a, const char *b);

class String {
private:
	char *_data;
	size_t _length;

public:
	~String();
	String();
	String(const String &string);
	String(String&& string) noexcept;

	String(const char *string);
	String(char character);
	String(size_t length);

	Buffer<String> Split(const char *delimiters) const;
	String SubString(size_t start, size_t end) const;
	inline String SubString(size_t start) const { return SubString(start, _length); }
	int IndexOf(char) const;
	int IndexOfAny(const char* possibleChars) const;

	void SetLength(size_t length);
	inline void Clear() { SetLength(0); }

	void Trim();

	inline const char* const GetData() const { return _data; }
	inline size_t GetLength() const { return _length; }
	inline char& operator[](size_t position) const { return _data[position]; }

	String& operator=(const String&);
	String& operator=(String&&) noexcept;
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
	static String FromInt(int64,	unsigned int minimunDigits = 0, byte base = 10);
	static String FromFloat(double, unsigned int minimumDigits = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String FromVector2(const Vector2&, unsigned int minimum = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String FromVector3(const Vector3&, unsigned int minimum = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String FromWideString(const wchar_t *string);
};

#define CSTR(BODY) ((String)BODY).GetData()

inline size_t StringLength(const char *string) {
	size_t length = 0;
	while (string[length] != '\0') length++;
	return length;
}

bool StringContains(const char *string, const char *phrase);
 