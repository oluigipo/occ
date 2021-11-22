#ifndef __STDLIB_H
#define __STDLIB_H

#include <stddef.h>

typedef struct _div_t {
    int quot;
    int rem;
} div_t;

typedef struct _ldiv_t {
    long quot;
    long rem;
} ldiv_t;

typedef struct _lldiv_t {
    long long quot;
    long long rem;
} lldiv_t;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define RAND_MAX 0x7fff

int __cdecl_ ___mb_cur_max_func(void);
#define MB_CUR_MAX ___mb_cur_max_func()

double __cdecl_ atof(const char* nptr);
int __cdecl_ atoi(const char* nptr);
long int __cdecl_ atol(const char* nptr);
long long int __cdecl_ atoll(const char* nptr);
double __cdecl_ strtod(const char* restrict nptr, char** restrict endptr);
float __cdecl_ strtof(const char* restrict nptr, char** restrict endptr);
long double __cdecl_ strtold(const char* restrict nptr, char** restrict endptr);
long int __cdecl_ strtol(const char* restrict nptr, char** restrict endptr, int base);
long long int __cdecl_ strtoll(const char* restrict nptr, char** restrict endptr, int base);
unsigned long int __cdecl_ strtoul(const char* restrict nptr, char** restrict endptr, int base);
unsigned long long int __cdecl_ strtoull(const char* restrict nptr, char** restrict endptr, int base);

int __cdecl_ rand(void);
void __cdecl_ srand(unsigned int seed);
void* __cdecl_ calloc(size_t count, size_t size);
void __cdecl_ free(void* ptr);
void* __cdecl_ malloc(size_t size);
void* __cdecl_ realloc(void* ptr, size_t size);

void __cdecl_ abort(void);
int __cdecl_ atexit(void (*__cdecl_ func)(void));
void __cdecl_ exit(int status);
void __cdecl_ _Exit(int status);

char* __cdecl_ getenv(const char* name);
int __cdecl_ system(const char* string);
void* __cdecl_ bsearch(const void* key, const void* base, size_t count, size_t size, int (*__cdecl_ compar)(const void*, const void*));
void __cdecl_ qsort(void* base, size_t count, size_t size, int (*__cdecl_ compar)(const void*, const void*));

int __cdecl_ abs(int j);
long int __cdecl_ labs(long int j);
long long int __cdecl_ llabs(long long int j);

div_t __cdecl_ div(int numer, int denom);
ldiv_t __cdecl_ div(long int numer, long int denom);
lldiv_t __cdecl_ div(long long int numer, long long int denom);

int __cdecl_ mblen(const char* s, size_t n);
int __cdecl_ mbtowc(wchar_t* restrict pwc, const char* restrict s, size_t n);
int __cdecl_ wctomb(char* s, wchar_t wc);
size_t __cdecl_ mbstowcs(wchar_t* restrict pwcs, const char* restrict s, size_t n);
size_t __cdecl_ wcstombs(char* restrict s, const wchar_t* restrict pwcs, size_t n);

#endif //__STDLIB_H
