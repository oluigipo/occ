#ifndef __STRING_H
#define __STRING_H

#include <stddef.h>

void* __cdecl_ memcpy(void* restrict s1, const void* restrict s2, size_t n);
void* __cdecl_ memmove(void* s1, const void* s2, size_t n);
char* __cdecl_ strcpy(char* restrict s1, const char* restrict s2);
char* __cdecl_ strncpy(char* restrict s1, const char* restrict s2, size_t n);

char* __cdecl_ strcat(char* restrict s1, const char* restrict s2);
char* __cdecl_ strncat(char* restrict s1, const char* restrict s2, size_t n);

int __cdecl_ memcmp(const void* s1, const void* s2, size_t n);
int __cdecl_ strcmp(const char* s1, const char* s2);
int __cdecl_ strcoll(const char* s1, const char* s2);
int __cdecl_ strncmp(const char* s1, const char* s2, size_t n);
size_t __cdecl_ strxfrm(char* restrict s1, const char* restrict s2, size_t n);

void* __cdecl_ memchr(const void* s, int c, size_t n);
char* __cdecl_ strchr(const char* s, int c);
size_t __cdecl_ strcspn(const char* s1, const char* s2);
char* __cdecl_ strpbrk(const char* s1, const char* s2);
char* __cdecl_ strrchr(const char* s, int c);
size_t __cdecl_ strspn(const char* s1, const char* s2);
char* __cdecl_ strstr(const char* s1, const char* s2);
char* __cdecl_ strtok(char* restrict s1, const char* restrict s2);

void* __cdecl_ memset(void* s, int c, size_t n);
char* __cdecl_ strerror(int errnum);
size_t __cdecl_ strlen(const char* s);

#endif //__STRING_H
