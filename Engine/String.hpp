#pragma once
#include "Types.hpp"
#include "Vector.hpp"

template<typename T>
class Buffer;

bool StringsEqual(const char *a, const char *b);
bool StringsInequal(const char *a, const char *b);

class String {
private:
	char *_data;
	size_t _length;

	void _SetLength(size_t length);
public:
	~String();
	String();

	//Creates an array of length characters of c
	String(char c, size_t length);

	String(const String &string);
	String(String&& string) noexcept;

	String(const char *string);
	String(char character);


	inline const char* const GetData() const { return _data; }
	inline size_t GetLength() const { return _length; }
	inline char& operator[](size_t position) const { return _data[position]; }

	Buffer<String> Split(const char *delimiters) const;
	String SubString(size_t start, size_t end) const;
	inline String SubString(size_t start) const { return SubString(start, _length); }
	size_t IndexOf(char) const;
	size_t IndexOfAny(const char* possibleChars) const;

	inline void Clear()					{ _SetLength(0); }
	inline void Shrink(size_t amount)	{ _SetLength(_length - amount); }
	inline void ShrinkTo(size_t length) { if (length < _length) _SetLength(length); }

	char& Insert(char c, size_t position);

	String& operator=(const String&);
	String& operator=(String&&) noexcept;
	String& operator=(const char*);
	String& operator+=(const String&);
	String& operator+=(const char*);
	String& operator+=(char);

	String operator+(const String&) const;

	size_t Compare(const String &other) const;
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
	template<typename T>
	static inline String Concat(T first) { return From(first); }

	//Concatenates each argument interpreted as a string
	template<typename T, typename ...Args>
	static inline String Concat(T first, Args... args) { return From(first) + Concat(args...); }

	//
	static String From(int64,			unsigned int minimumDigits = 0, byte base = 10);
	static String From(double,			unsigned int minimumDigits = 0, unsigned int maxDecimalDigits = 3, byte base = 10);
	static String From(const Vector2&,	unsigned int minimumDigits = 0,	unsigned int maxDecimalDigits = 3, byte base = 10);
	static String From(const Vector3&,	unsigned int minimumDigits = 0,	unsigned int maxDecimalDigits = 3, byte base = 10);
	static String From(const wchar_t *string);

	inline static String From(const String& string) { return string; }
	inline static String From(char c) { return String(c); }
	inline static String From(unsigned char c) { return String(c); }
	
#define LINKFROM(FROMTYPE, TOTYPE) inline static String From(FROMTYPE x) { return From((TOTYPE)x);}
	LINKFROM(short, int64);
	LINKFROM(int, int64);
	LINKFROM(long, int64);
	LINKFROM(unsigned short, int64);
	LINKFROM(unsigned int, int64);
	LINKFROM(unsigned long, int64);
	LINKFROM(unsigned long long, int64);
	LINKFROM(float, double);
};

//Concatenates args and returns a char pointer (which is deleted before the next statement)
#define CSTR(...) (String::Concat(__VA_ARGS__)).GetData()

inline size_t StringLength(const char *string) {
	size_t length = 0;
	while (string[length] != '\0') length++;
	return length;
}

bool StringContains(const char *string, const char *phrase);
 