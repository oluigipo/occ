# 1 "B:/occ/tests/pp-test.c" 1








# 14 "B:/occ/tests/pp-test.c"





const char* name = "pepe";
void* hello(void)
{
	return "world";
}

int an_int = 0xffffULL;
float a_float = 35.0f;
float wtf = 0x8080.0p+3f;

const char* a_string = "pepe, " "MY_MACRO" " is " "hello";







int a = "HI THERE";
int b = HI_THERE;
int c = "HI THERE";

# 1 "B:/occ/include/stdio.h" 1



# 1 "B:/occ/include/stddef.h" 1



# 7 "B:/occ/include/stddef.h"


# 11 "B:/occ/include/stddef.h"






typedef unsigned long long size_t;
typedef signed long long ptrdiff_t;

# 24 "B:/occ/include/stddef.h"


typedef unsigned short wchar_t;


# 5 "B:/occ/include/stdio.h" 2

# 1 "B:/occ/include/stdarg.h" 1










# 6 "B:/occ/include/stdio.h" 2


typedef struct _FILE FILE;
typedef size_t fpos_t;




















FILE* __acrt_iob_func(unsigned index);
FILE* __acrt_func(void);





int remove(const char* filename);
int rename(const char* old, const char* new); // :)
FILE* tmpfile(void);
char* tmpnam(char* s);

int fclose(FILE* stream);
int fflush(FILE* stream);
FILE* fopen(const char* restrict filename, const char* restrict mode);
FILE* freopen(const char* restrict filename, const char* restrict mode, FILE* restrict stream);
void setbuf(FILE* restrict stream, char* restrict buf);
int setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size);

int fprintf(FILE* restrict stream, const char* restrict format, ...);
int fscanf(FILE* restrict stream, const char* restrict format, ...);
int printf(const char* restrict format, ...);
int scanf(const char* restrict format, ...);
int snprintf(char* restrict s, size_t n, const char* restrict format, ...);
int sprintf(char* restrict s, const char* restrict format, ...);
int sscanf(const char* restrict s, const char* restrict format, ...);
int vfprintf(FILE* restrict stream, const char* restrict format, void* arg);
int vfscanf(FILE* restrict stream, const char* restrict format, void* arg);
int vprintf(const char* restrict format, void* arg);
int vscanf(const char* restrict format, void* arg);
int vsprintf(char* restrict s, size_t n, const char* restrict format, void* arg);
int vsprintf(char* restrict s, const char* restrict format, void* arg);
int vsscanf(const char* restrict s, const char* restrict format, void* arg);

int fgetc(FILE *stream);
char* fgets(char* restrict s, int n, FILE* restrict stream);
int fputc(int c, FILE* stream);
int fputs(const char* restrict s, FILE* restrict stream);
int getc(FILE* stream);
int getchar(void);
char* gets(char* s);
int putc(int c, FILE* stream);
int putchar(int c);
int puts(const char* s);
int ungetc(int c, FILE* stream);

size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);
size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);

int fgetpos(FILE* restrict stream, fpos_t* restrict pos);
int fseek(FILE* stream, long int offset, int whence);
int fsetpos(FILE* stream, const fpos_t* pos);
long int ftell(FILE* stream);
void rewind(FILE* stream);

void clearerr(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);
void perror(const char* s);


# 41 "B:/occ/tests/pp-test.c" 2




int main(int argc, char* argv[])
{
	int typedef a;
	a f(void);
	a v(void);
	
	a (*pp[2])(void) = {
		f, v
	};
	
	printf("address of 'f' is %p\n", pp[0]);
	printf("this is line " "55" "!\n");
}
