#pragma once
#include "Buffer.h"

inline bool StringsEqual(const char *a, const char *b);

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

	Buffer<String> Split(const char *delimiters);

	void SetLength(unsigned int length);
	inline void Clear() { SetLength(0); }

	inline const char* const GetData() const { return _data; }
	inline unsigned int GetLength() const { return _length; }
	inline char& operator[](unsigned int position) const { return _data[position]; }

	const String& operator=(const String&);
	const String& operator=(const char*);
	const String& operator+=(const String&);
	const String& operator+=(const char*);
	const String& operator+=(char);
	
	String operator+(const String&);

	int Compare(const String &other) const;
	bool operator==(const String &other) const			{ return StringsEqual(_data, other._data);	}
	inline bool operator<(const String &other) const	{ return Compare(other) < 0;	}
	inline bool operator>(const String &other) const	{ return Compare(other) > 0;	}

	bool operator==(const char*) const;

	int ToInt();
	float ToFloat();

	//
	static String Convert(__int64,	unsigned int minimun = 0, byte base = 10);
	static String ConvertFloat(double, unsigned int minimum = 0, byte base = 10);
};

#define CSTR(BODY) ((String)BODY).GetData()

inline unsigned int StringLength(const char *string) {
	unsigned int length = 0;
	while (string[++length] != '\0');
	return length;
}

inline bool StringsEqual(const char *a, const char *b)
{
	unsigned int i = 0;

	while (1)
	{
		if (a[i] != b[i])
			return false;

		if (a[i] == '\0')
			return true;

		i++;
	}
}

bool StringContains(const char *string, const char *phrase);
