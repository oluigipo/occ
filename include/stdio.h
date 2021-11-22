#ifndef __STDIO_H
#define __STDIO_H

#include <stddef.h>
#include <stdarg.h>

typedef struct _FILE FILE;
typedef size_t fpos_t;

#define _IOFBF 0x0000
#define _IOLBF 0x0040
#define _IONBF 0x0004

#define BUFSIZ 512

#define EOF (-1)

#define FILENAME_MAX 260
#define FOPEN_MAX 20

#define L_tmpnam 14

#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0

#define TMP_MAX 32767

FILE* __cdecl_ __acrt_iob_func(unsigned index);
FILE* __cdecl_ __acrt_func(void);

#define stdin (__acrt_iob_func(0))
#define stdout (__acrt_iob_func(1))
#define stderr (__acrt_iob_func(2))

int __cdecl_ remove(const char* filename);
int __cdecl_ rename(const char* old, const char* new); // :)
FILE* __cdecl_ tmpfile(void);
char* __cdecl_ tmpnam(char* s);

int __cdecl_ fclose(FILE* stream);
int __cdecl_ fflush(FILE* stream);
FILE* __cdecl_ fopen(const char* restrict filename, const char* restrict mode);
FILE* __cdecl_ freopen(const char* restrict filename, const char* restrict mode, FILE* restrict stream);
void __cdecl_ setbuf(FILE* restrict stream, char* restrict buf);
int __cdecl_ setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size);

int __cdecl_ fprintf(FILE* restrict stream, const char* restrict format, ...);
int __cdecl_ fscanf(FILE* restrict stream, const char* restrict format, ...);
int __cdecl_ printf(const char* restrict format, ...);
int __cdecl_ scanf(const char* restrict format, ...);
int __cdecl_ snprintf(char* restrict s, size_t n, const char* restrict format, ...);
int __cdecl_ sprintf(char* restrict s, const char* restrict format, ...);
int __cdecl_ sscanf(const char* restrict s, const char* restrict format, ...);
int __cdecl_ vfprintf(FILE* restrict stream, const char* restrict format, va_list arg);
int __cdecl_ vfscanf(FILE* restrict stream, const char* restrict format, va_list arg);
int __cdecl_ vprintf(const char* restrict format, va_list arg);
int __cdecl_ vscanf(const char* restrict format, va_list arg);
int __cdecl_ vsnprintf(char* restrict s, size_t n, const char* restrict format, va_list arg);
int __cdecl_ vsprintf(char* restrict s, const char* restrict format, va_list arg);
int __cdecl_ vsscanf(const char* restrict s, const char* restrict format, va_list arg);

int __cdecl_ fgetc(FILE *stream);
char* __cdecl_ fgets(char* restrict s, int n, FILE* restrict stream);
int __cdecl_ fputc(int c, FILE* stream);
int __cdecl_ fputs(const char* restrict s, FILE* restrict stream);
int __cdecl_ getc(FILE* stream);
int __cdecl_ getchar(void);
char* __cdecl_ gets(char* s);
int __cdecl_ putc(int c, FILE* stream);
int __cdecl_ putchar(int c);
int __cdecl_ puts(const char* s);
int __cdecl_ ungetc(int c, FILE* stream);

size_t __cdecl_ fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);
size_t __cdecl_ fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);

int __cdecl_ fgetpos(FILE* restrict stream, fpos_t* restrict pos);
int __cdecl_ fseek(FILE* stream, long int offset, int whence);
int __cdecl_ fsetpos(FILE* stream, const fpos_t* pos);
long int __cdecl_ ftell(FILE* stream);
void __cdecl_ rewind(FILE* stream);

void __cdecl_ clearerr(FILE* stream);
int __cdecl_ feof(FILE* stream);
int __cdecl_ ferror(FILE* stream);
void __cdecl_ perror(const char* s);

#endif //__STDIO_H
