#ifndef __STDINT_H
#define __STDINT_H

#include <stddef.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

#ifdef _WIN64

typedef unsigned long long uint64_t;
typedef signed long long int64_t;

#elif defined(_WIN32)

typedef unsigned long long uint64_t;
typedef signed long long int64_t;

#elif defined(__linux__)

#ifdef __x86_64__
typedef unsigned long uint64_t;
typedef signed long int64_t;
#else //__x86_64
typedef unsigned long long uint64_t;
typedef signed long long int64_t;
#endif //__x86_64

#endif //_WIN64

typedef size_t uintptr_t;
typedef ptrdiff_t intptr_t;

typedef uint8_t uint_least8__t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef int8_t int_least8__t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_fast8__t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef int8_t int_fast8__t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

typedef unsigned short wint_t;
typedef unsigned short wctype_t;

#define INT8_MIN -0x80
#define INT8_MAX 0x7f
#define UINT8_MAX 0xffU

#define INT16_MIN -0x8000
#define INT16_MAX 0x7fff
#define UINT16_MAX 0xffffU

#define INT32_MIN -0x80000000
#define INT32_MAX 0x7fffffff
#define UINT32_MAX 0xffffffffU

#define INT64_MIN -0x8000000000000000LL
#define INT64_MAX 0x7fffffffffffffffLL
#define UINT64_MAX 0xffffffffffffffffULL

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST8_MAX INT8_MAX
#define UINT_LEAST8_MAX UINT8_MAX

#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST16_MAX INT16_MAX
#define UINT_LEAST16_MAX UINT16_MAX

#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST32_MAX INT32_MAX
#define UINT_LEAST32_MAX UINT32_MAX

#define INT_LEAST64_MIN INT64_MIN
#define INT_LEAST64_MAX INT64_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MIN INT8_MIN
#define INT_FAST8_MAX INT8_MAX
#define UINT_FAST8_MAX UINT8_MAX

#define INT_FAST16_MIN INT16_MIN
#define INT_FAST16_MAX INT16_MAX
#define UINT_FAST16_MAX UINT16_MAX

#define INT_FAST32_MIN INT32_MIN
#define INT_FAST32_MAX INT32_MAX
#define UINT_FAST32_MAX UINT32_MAX

#define INT_FAST64_MIN INT64_MIN
#define INT_FAST64_MAX INT64_MAX
#define UINT_FAST64_MAX UINT64_MAX

#ifdef __x86_64__
#   define PTRDIFF_MIN INT64_MIN
#   define PTRDIFF_MAX INT64_MAX
#   define INTPTR_MIN INT64_MIN
#   define INTPTR_MAX INT64_MAX
#   define UINTPTR_MAX UINT64_MAX
#   define SIZE_MAX UINT64_MAX
#else
#   define PTRDIFF_MIN INT32_MIN
#   define PTRDIFF_MAX INT32_MAX
#   define INTPTR_MIN INT32_MIN
#   define INTPTR_MAX INT32_MAX
#   define UINTPTR_MAX UINT32_MAX
#   define SIZE_MAX UINT32_MAX
#endif

#define INTMAX_MAX INT64_MAX
#define INTMAX_MIN INT64_MIN
#define UINTMAX_MAX UINT64_MAX

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#define WCHAR_MIN 0U
#define WCHAR_MAX 0xffffU

#define INT8_C(x) (x)
#define INT16_C(x) (x)
#define INT32_C(x) (x)
#define INT64_C(x) (x##LL)

#define UINT8_C(x) (x##U)
#define UINT16_C(x) (x##U)
#define UINT32_C(x) (x##U)
#define UINT64_C(x) (x##ULL)

#define INTMAX_C(x) (x##ULL)
#define UINTMAX_C(x) (x##ULL)

#endif //__STDINT_H
