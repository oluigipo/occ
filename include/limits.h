#ifndef __LIMITS_H
#define __LIMITS_H

#define CHAR_BIT 8

#define SCHAR_MIN (-128)
#define SCHAR_MAX 127
#define UCHAR_MAX 255
#define CHAR_MIN (-128)
#define CHAR_MAX 127
#define MB_LEN_MAX 5
#define SHRT_MIN (-32768)
#define SHRT_MAX 32767
#define USHRT_MAX 65535
#define INT_MIN (-2147483648)
#define INT_MAX 2147483647
#define UINT_MAX 4294967295u

#if __SIZEOF_LONG == 4
#   define LONG_MIN INT_MIN
#   define LONG_MAX INT_MAX
#   define ULONG_MAX UINT_MAX
#else
#   define LONG_MIN LLONG_MIN
#   define LONG_MAX LLONG_MAX
#   define ULONG_MAX ULLONG_MAX
#endif

#define LLONG_MIN (-9223372036854775808ll)
#define LLONG_MAX 9223372036854775807ll
#define ULLONG_MAX 18446744073709551615ull

#endif //__LIMITS_H