#pragma once

#define DO_ONCE_BEGIN { static bool __first = true; if (__first) { __first = false; 

#define DO_ONCE_END }}

#define DO_ONCE(BODY) DO_ONCE_BEGIN BODY; DO_ONCE_END

#define DEFINE_BITMASK_FUNCS(ENUMCLASS)								\
inline ENUMCLASS operator|(const ENUMCLASS& a, const ENUMCLASS& b)	\
{																	\
	return ENUMCLASS(int(a) | int(b));								\
}																	\
																	\
inline ENUMCLASS operator|=(ENUMCLASS& a, const ENUMCLASS& b)		\
{																	\
	return a = a | b;												\
}																	\
																	\
inline ENUMCLASS operator~(const ENUMCLASS& rc)						\
{																	\
	return ENUMCLASS(~int(rc));										\
}																	\
																	\
inline bool operator&(const ENUMCLASS& a, const ENUMCLASS& b)		\
{																	\
	return int(a) & int(b);											\
}

