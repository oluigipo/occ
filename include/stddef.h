#ifndef __STDDEF_H
#define __STDDEF_H

#if !defined(_WIN64) || !defined(_WIN32) || !defined(__linux__)
#   error This header only supports WindowsNT and Linux.
#endif

#if defined(_M_X64)
#   define __x86_64__ 1
#endif

#define NULL 0
#define offsetof(T, field) __builtin_offsetof(T, field)

#ifdef _WIN64
typedef unsigned long long size_t;
typedef signed long long ptrdiff_t;

#elif defined(_WIN32) || defined(__linux__)
typedef unsigned long size_t;
typedef signed long ptrdiff_t;

#endif // _WIN64

typedef unsigned short wchar_t;

#endif //__STDDEF_H
