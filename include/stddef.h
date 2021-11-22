#ifndef __STDDEF_H
#define __STDDEF_H

#if !defined(_WIN64) && !defined(_WIN32) && !defined(__linux__)
#   error This header only supports WindowsNT and Linux.
#endif

#if defined(_M_X64)
#   define __x86_64__ 1
#endif

#define NULL 0
#define offsetof(T, field) (&((T*)0)->field)

#ifdef _WIN64
typedef unsigned long long size_t;
typedef signed long long ptrdiff_t;

#elif defined(_WIN32) || defined(__linux__)
typedef unsigned long size_t;
typedef signed long ptrdiff_t;

#endif // _WIN64

typedef unsigned short wchar_t;

#if defined(_WIN64) || defined(__linux__)
#   define __cdecl_
#else
#   define __cdecl_ __cdecl
#endif

#if __STDC_VERSION__ < 199901L
#   define restrict
#endif

#endif //__STDDEF_H
