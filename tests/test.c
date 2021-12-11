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

# 1 "B:/occ/_include-mingw/stdio.h" 1




# 1 "B:/occ/_include-mingw/crtdefs.h" 1





# 1 "B:/occ/_include-mingw/corecrt.h" 1





# 1 "B:/occ/_include-mingw/_mingw.h" 1





# 1 "B:/occ/_include-mingw/_mingw_mac.h" 1












/* This macro holds an monotonic increasing value, which indicates
   a specific fix/patch is present on trunk.  This value isn't related to
   minor/major version-macros.  It is increased on demand, if a big
   fix was applied to trunk.  This macro gets just increased on trunk.  For
   other branches its value won't be modified.  */







/* mingw.org's version macros: these make gcc to define
   MINGW32_SUPPORTS_MT_EH and to use the _CRT_MT global
   and the __mingwthr_key_dtor() function from the MinGW
   CRT in its private gthr-win32.h header. */



/* Set VC specific compiler target macros.  */
# 46 "B:/occ/_include-mingw/_mingw_mac.h"


# 59 "B:/occ/_include-mingw/_mingw_mac.h"







# 70 "B:/occ/_include-mingw/_mingw_mac.h"


# 83 "B:/occ/_include-mingw/_mingw_mac.h"


# 90 "B:/occ/_include-mingw/_mingw_mac.h"






# 109 "B:/occ/_include-mingw/_mingw_mac.h"




# 117 "B:/occ/_include-mingw/_mingw_mac.h"







# 129 "B:/occ/_include-mingw/_mingw_mac.h"








# 139 "B:/occ/_include-mingw/_mingw_mac.h"






# 147 "B:/occ/_include-mingw/_mingw_mac.h"













# 162 "B:/occ/_include-mingw/_mingw_mac.h"

# 165 "B:/occ/_include-mingw/_mingw_mac.h"





/* Special case nameless struct/union.  */























/* MinGW-w64 has some additional C99 printf/scanf feature support.
   So we add some helper macros to ease recognition of them.  */





# 203 "B:/occ/_include-mingw/_mingw_mac.h"







# 213 "B:/occ/_include-mingw/_mingw_mac.h"



# 220 "B:/occ/_include-mingw/_mingw_mac.h"




# 228 "B:/occ/_include-mingw/_mingw_mac.h"






# 237 "B:/occ/_include-mingw/_mingw_mac.h"


# 247 "B:/occ/_include-mingw/_mingw_mac.h"










# 262 "B:/occ/_include-mingw/_mingw_mac.h"




# 269 "B:/occ/_include-mingw/_mingw_mac.h"













# 293 "B:/occ/_include-mingw/_mingw_mac.h"
# 293 "B:/occ/_include-mingw/_mingw_mac.h"





# 301 "B:/occ/_include-mingw/_mingw_mac.h"




# 311 "B:/occ/_include-mingw/_mingw_mac.h"




# 341 "B:/occ/_include-mingw/_mingw_mac.h"




/* If _FORTIFY_SOURCE is enabled, some inline functions may use
   __builtin_va_arg_pack().  GCC may report an error if the address
   of such a function is used.  Set _FORTIFY_VA_ARG=0 in this case.  */
# 351 "B:/occ/_include-mingw/_mingw_mac.h"




/* Enable workaround for ABI incompatibility on affected platforms */

# 359 "B:/occ/_include-mingw/_mingw_mac.h"




# 11 "B:/occ/_include-mingw/_mingw.h" 2

# 1 "B:/occ/_include-mingw/_mingw_secapi.h" 1

 



/* http://msdn.microsoft.com/en-us/library/ms175759%28v=VS.100%29.aspx */
# 28 "B:/occ/_include-mingw/_mingw_secapi.h"
















# 53 "B:/occ/_include-mingw/_mingw_secapi.h"


/* https://blogs.msdn.com/b/sdl/archive/2010/02/16/vc-2010-and-memcpy.aspx?Redirected=true */
/* fallback on default implementation if we can't know the size of the destination */
# 69 "B:/occ/_include-mingw/_mingw_secapi.h"





# 12 "B:/occ/_include-mingw/_mingw.h" 2


/* Include _cygwin.h if we're building a Cygwin application. */
# 17 "B:/occ/_include-mingw/_mingw.h"


/* Target specific macro replacement for type "long".  In the Windows API,
   the type long is always 32 bit, even if the target is 64 bit (LLP64).
   On 64 bit Cygwin, the type long is 64 bit (LP64).  So, to get the right
   sized definitions and declarations, all usage of type long in the Windows
   headers have to be replaced by the below defined macro __LONG32. */


# 28 "B:/occ/_include-mingw/_mingw.h"


/* C/C++ specific language defines.  */

# 34 "B:/occ/_include-mingw/_mingw.h"













# 73 "B:/occ/_include-mingw/_mingw.h"




# 79 "B:/occ/_include-mingw/_mingw.h"


# 83 "B:/occ/_include-mingw/_mingw.h"


# 87 "B:/occ/_include-mingw/_mingw.h"


# 96 "B:/occ/_include-mingw/_mingw.h"


# 100 "B:/occ/_include-mingw/_mingw.h"





# 108 "B:/occ/_include-mingw/_mingw.h"



# 113 "B:/occ/_include-mingw/_mingw.h"

# 116 "B:/occ/_include-mingw/_mingw.h"








# 126 "B:/occ/_include-mingw/_mingw.h"



# 131 "B:/occ/_include-mingw/_mingw.h"


# 143 "B:/occ/_include-mingw/_mingw.h"


# 151 "B:/occ/_include-mingw/_mingw.h"
# 151 "B:/occ/_include-mingw/_mingw.h"





# 162 "B:/occ/_include-mingw/_mingw.h"
# 162 "B:/occ/_include-mingw/_mingw.h"





/* Attribute `nonnull' was valid as of gcc 3.3.  We don't use GCC's
   variadiac macro facility, because variadic macros cause syntax
   errors with  --traditional-cpp.  */
# 172 "B:/occ/_include-mingw/_mingw.h"




# 178 "B:/occ/_include-mingw/_mingw.h"




# 191 "B:/occ/_include-mingw/_mingw.h"
# 191 "B:/occ/_include-mingw/_mingw.h"









# 204 "B:/occ/_include-mingw/_mingw.h"
# 204 "B:/occ/_include-mingw/_mingw.h"




# 210 "B:/occ/_include-mingw/_mingw.h"




# 218 "B:/occ/_include-mingw/_mingw.h"
# 218 "B:/occ/_include-mingw/_mingw.h"










# 232 "B:/occ/_include-mingw/_mingw.h"










# 256 "B:/occ/_include-mingw/_mingw.h"



# 273 "B:/occ/_include-mingw/_mingw.h"


# 277 "B:/occ/_include-mingw/_mingw.h"



# 282 "B:/occ/_include-mingw/_mingw.h"





# 1 "B:/occ/_include-mingw/vadefs.h" 1




# 1 "B:/occ/_include-mingw/_mingw.h" 1


# 598 "B:/occ/_include-mingw/_mingw.h"





/* for backward compatibility */








# 1 "B:/occ/_include-mingw/sdks/_mingw_directx.h" 1









# 17 "B:/occ/_include-mingw/sdks/_mingw_directx.h"

# 612 "B:/occ/_include-mingw/_mingw.h" 2

# 1 "B:/occ/_include-mingw/sdks/_mingw_ddk.h" 1






# 613 "B:/occ/_include-mingw/_mingw.h" 2




# 10 "B:/occ/_include-mingw/vadefs.h" 2





#pragma pack(push,8)


# 20 "B:/occ/_include-mingw/vadefs.h"


# 27 "B:/occ/_include-mingw/vadefs.h"




# 33 "B:/occ/_include-mingw/vadefs.h"

typedef char *  va_list;
# 37 "B:/occ/_include-mingw/vadefs.h"



# 42 "B:/occ/_include-mingw/vadefs.h"




# 54 "B:/occ/_include-mingw/vadefs.h"



# 63 "B:/occ/_include-mingw/vadefs.h"





# 76 "B:/occ/_include-mingw/vadefs.h"









# 95 "B:/occ/_include-mingw/vadefs.h"




# 101 "B:/occ/_include-mingw/vadefs.h"



#pragma pack(pop)




# 287 "B:/occ/_include-mingw/_mingw.h" 2

















# 306 "B:/occ/_include-mingw/_mingw.h"


















/* We have to define _DLL for gcc based mingw version. This define is set
   by VC, when DLL-based runtime is used. So, gcc based runtime just have
   DLL-base runtime, therefore this define has to be set.
   As our headers are possibly used by windows compiler having a static
   C-runtime, we make this definition gnu compiler specific here.  */
# 331 "B:/occ/_include-mingw/_mingw.h"











































# 378 "B:/occ/_include-mingw/_mingw.h"









# 389 "B:/occ/_include-mingw/_mingw.h"






# 397 "B:/occ/_include-mingw/_mingw.h"



















/* MSVC defines _NATIVE_NULLPTR_SUPPORTED when nullptr is supported. We emulate it here for GCC. */
# 421 "B:/occ/_include-mingw/_mingw.h"


/* We are activating __USE_MINGW_ANSI_STDIO for various define indicators.
   Note that we enable it also for _GNU_SOURCE in C++, but not for C case. */
# 434 "B:/occ/_include-mingw/_mingw.h"


/* We are defining __USE_MINGW_ANSI_STDIO as 0 or 1 */


# 443 "B:/occ/_include-mingw/_mingw.h"


/* _dowildcard is an int that controls the globbing of the command line.
 * The MinGW32 (mingw.org) runtime calls it _CRT_glob, so we are adding
 * a compatibility definition here:  you can use either of _CRT_glob or
 * _dowildcard .
 * If _dowildcard is non-zero, the command line will be globbed:  *.*
 * will be expanded to be all files in the startup directory.
 * In the mingw-w64 library a _dowildcard variable is defined as being
 * 0, therefore command line globbing is DISABLED by default. To turn it
 * on and to leave wildcard command line processing MS's globbing code,
 * include a line in one of your source modules defining _dowildcard and
 * setting it to -1, like so:
 * int _dowildcard = -1;
 */







# 468 "B:/occ/_include-mingw/_mingw.h"









# 481 "B:/occ/_include-mingw/_mingw.h"


















# 510 "B:/occ/_include-mingw/_mingw.h"











# 528 "B:/occ/_include-mingw/_mingw.h"




/* Macros for __uuidof template-based emulation */
# 565 "B:/occ/_include-mingw/_mingw.h"






# 573 "B:/occ/_include-mingw/_mingw.h"




# 579 "B:/occ/_include-mingw/_mingw.h"




void __cdecl __debugbreak(void);
void __cdecl __debugbreak(void)
{
  __asm__ volatile("int {$}3":);
}



/* mingw-w64 specific functions: */
const char *__mingw_get_crt_info (void);

# 596 "B:/occ/_include-mingw/_mingw.h"




# 616 "B:/occ/_include-mingw/_mingw.h"

# 11 "B:/occ/_include-mingw/corecrt.h" 2





#pragma pack(push,8)


# 22 "B:/occ/_include-mingw/corecrt.h"


# 27 "B:/occ/_include-mingw/corecrt.h"














typedef unsigned long long size_t;
# 44 "B:/occ/_include-mingw/corecrt.h"







typedef long long ssize_t;
# 54 "B:/occ/_include-mingw/corecrt.h"




typedef size_t rsize_t;









typedef long long intptr_t;
# 71 "B:/occ/_include-mingw/corecrt.h"










typedef unsigned long long uintptr_t;
# 84 "B:/occ/_include-mingw/corecrt.h"










typedef long long ptrdiff_t;
# 97 "B:/occ/_include-mingw/corecrt.h"







typedef unsigned short wchar_t;







typedef unsigned short wint_t;
typedef unsigned short wctype_t;





typedef int errno_t;




typedef long __time32_t;




typedef long long __time64_t;


# 137 "B:/occ/_include-mingw/corecrt.h"




# 143 "B:/occ/_include-mingw/corecrt.h"

typedef __time64_t time_t;







# 158 "B:/occ/_include-mingw/corecrt.h"


# 266 "B:/occ/_include-mingw/corecrt.h"























# 426 "B:/occ/_include-mingw/corecrt.h"










struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;
struct __lc_time_data;

typedef struct localeinfo_struct {
  pthreadlocinfo locinfo;
  pthreadmbcinfo mbcinfo;
} _locale_tstruct,*_locale_t;



typedef struct tagLC_ID {
  unsigned short wLanguage;
  unsigned short wCountry;
  unsigned short wCodePage;
} LC_ID,*LPLC_ID;




typedef struct threadlocaleinfostruct {
# 463 "B:/occ/_include-mingw/corecrt.h"

int refcount;
  unsigned int lc_codepage;
  unsigned int lc_collate_cp;
  unsigned long lc_handle[6];
  LC_ID lc_id[6];
  struct {
    char *locale;
    wchar_t *wlocale;
    int *refcount;
    int *wrefcount;
  } lc_category[6];
  int lc_clike;
  int mb_cur_max;
  int *lconv_intl_refcount;
  int *lconv_num_refcount;
  int *lconv_mon_refcount;
  struct lconv *lconv;
  int *ctype1_refcount;
  unsigned short *ctype1;
  const unsigned short *pctype;
  const unsigned char *pclmap;
  const unsigned char *pcumap;
  struct __lc_time_data *lc_time_curr;

} threadlocinfo;







#pragma pack(pop)



# 11 "B:/occ/_include-mingw/crtdefs.h" 2



# 10 "B:/occ/_include-mingw/stdio.h" 2


#pragma pack(push,8)

# 16 "B:/occ/_include-mingw/stdio.h"









struct _iobuf {
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
  };
  typedef struct _iobuf FILE;



# 42 "B:/occ/_include-mingw/stdio.h"







# 52 "B:/occ/_include-mingw/stdio.h"
















# 74 "B:/occ/_include-mingw/stdio.h"





# 1 "B:/occ/_include-mingw/_mingw_off_t.h" 1




typedef long _off_t;

typedef long off32_t;





typedef long long _off64_t;

typedef long long off64_t;







# 26 "B:/occ/_include-mingw/_mingw_off_t.h"

typedef off32_t off_t;






# 79 "B:/occ/_include-mingw/stdio.h" 2


__declspec(dllimport) FILE *__cdecl __acrt_iob_func(unsigned index);


__declspec(dllimport) FILE *__cdecl __iob_func(void);

# 95 "B:/occ/_include-mingw/stdio.h"







# 105 "B:/occ/_include-mingw/stdio.h"

typedef long long fpos_t;

























# 134 "B:/occ/_include-mingw/stdio.h"




# 159 "B:/occ/_include-mingw/stdio.h"


# 163 "B:/occ/_include-mingw/stdio.h"



extern
  
  int __cdecl __mingw_sscanf(const char * restrict _Src,const char * restrict _Format,...);
extern
  
  int __cdecl __mingw_vsscanf (const char * restrict _Str,const char * restrict Format,va_list argp);
extern
  
  int __cdecl __mingw_scanf(const char * restrict _Format,...);
extern
  
  int __cdecl __mingw_vscanf(const char * restrict Format, va_list argp);
extern
  
  int __cdecl __mingw_fscanf(FILE * restrict _File,const char * restrict _Format,...);
extern
  
  int __cdecl __mingw_vfscanf (FILE * restrict fp, const char * restrict Format,va_list argp);

extern
  
  int __cdecl __mingw_vsnprintf(char * restrict _DstBuf,size_t _MaxCount,const char * restrict _Format,
                               va_list _ArgList);
extern
  
  int __cdecl __mingw_snprintf(char * restrict s, size_t n, const char * restrict  format, ...);
extern
  
  int __cdecl __mingw_printf(const char * restrict , ... ) ;
extern
  
  int __cdecl __mingw_vprintf (const char * restrict , va_list) ;
extern
  
  int __cdecl __mingw_fprintf (FILE * restrict , const char * restrict , ...) ;
extern
  
  int __cdecl __mingw_vfprintf (FILE * restrict , const char * restrict , va_list) ;
extern
  
  int __cdecl __mingw_sprintf (char * restrict , const char * restrict , ...) ;
extern
  
  int __cdecl __mingw_vsprintf (char * restrict , const char * restrict , va_list) ;
extern
  
  int __cdecl __mingw_asprintf(char ** restrict , const char * restrict , ...) ;
extern
  
  int __cdecl __mingw_vasprintf(char ** restrict , const char * restrict , va_list) ;

# 222 "B:/occ/_include-mingw/stdio.h"





# 233 "B:/occ/_include-mingw/stdio.h"
# 233 "B:/occ/_include-mingw/stdio.h"





# 463 "B:/occ/_include-mingw/stdio.h"





/*
 * Default configuration: simply direct all calls to MSVCRT...
 */
# 554 "B:/occ/_include-mingw/stdio.h"


  int __cdecl fprintf(FILE * restrict _File,const char * restrict _Format,...);
  
  int __cdecl printf(const char * restrict _Format,...);
  
  int __cdecl sprintf(char * restrict _Dest,const char * restrict _Format,...) ;

  
  int __cdecl vfprintf(FILE * restrict _File,const char * restrict _Format,va_list _ArgList);
  
  int __cdecl vprintf(const char * restrict _Format,va_list _ArgList);
  
  int __cdecl vsprintf(char * restrict _Dest,const char * restrict _Format,va_list _Args) ;

  
  int __cdecl fscanf(FILE * restrict _File,const char * restrict _Format,...) ;
  
  int __cdecl scanf(const char * restrict _Format,...) ;
  
  int __cdecl sscanf(const char * restrict _Src,const char * restrict _Format,...) ;
# 578 "B:/occ/_include-mingw/stdio.h"


# 583 "B:/occ/_include-mingw/stdio.h"


  
  int __cdecl __ms_vscanf(const char * restrict Format, va_list argp);
  
  int __cdecl __ms_vfscanf (FILE * restrict fp, const char * restrict Format,va_list argp);
  
  int __cdecl __ms_vsscanf (const char * restrict _Str,const char * restrict Format,va_list argp);

  static __cdecl
  
  int vfscanf (FILE *__stream,  const char *__format, void* __local_argv)
  {
    return __ms_vfscanf (__stream, __format, __local_argv);
  }

  static __cdecl
  
  int vsscanf (const char * restrict __source, const char * restrict __format, void* __local_argv)
  {
    return __ms_vsscanf( __source, __format, __local_argv );
  }
  static __cdecl
  
  int vscanf(const char *__format,  void* __local_argv)
  {
    return __ms_vscanf (__format, __local_argv);
  }

# 614 "B:/occ/_include-mingw/stdio.h"






  __declspec(dllimport) int __cdecl _filbuf(FILE *_File);
  __declspec(dllimport) int __cdecl _flsbuf(int _Ch,FILE *_File);
# 624 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) FILE *__cdecl _fsopen(const char *_Filename,const char *_Mode,int _ShFlag);

void __cdecl clearerr(FILE *_File);
  int __cdecl fclose(FILE *_File);
  __declspec(dllimport) int __cdecl _fcloseall(void);
# 632 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) FILE *__cdecl _fdopen(int _FileHandle,const char *_Mode);

int __cdecl feof(FILE *_File);
  int __cdecl ferror(FILE *_File);
  int __cdecl fflush(FILE *_File);
  int __cdecl fgetc(FILE *_File);
  __declspec(dllimport) int __cdecl _fgetchar(void);
  int __cdecl fgetpos(FILE * restrict _File ,fpos_t * restrict _Pos); /* 64bit only, no 32bit version */
  int __cdecl fgetpos64(FILE * restrict _File ,fpos_t * restrict _Pos); /* fgetpos already 64bit */
  char *__cdecl fgets(char * restrict _Buf,int _MaxCount,FILE * restrict _File);
  __declspec(dllimport) int __cdecl _fileno(FILE *_File);
# 646 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) char *__cdecl _tempnam(const char *_DirName,const char *_FilePrefix);
  __declspec(dllimport) int __cdecl _flushall(void);
  FILE *__cdecl fopen(const char * restrict _Filename,const char * restrict _Mode) ;
  FILE *fopen64(const char * restrict filename,const char * restrict  mode);
  int __cdecl fputc(int _Ch,FILE *_File);
  __declspec(dllimport) int __cdecl _fputchar(int _Ch);
  int __cdecl fputs(const char * restrict _Str,FILE * restrict _File);
  size_t __cdecl fread(void * restrict _DstBuf,size_t _ElementSize,size_t _Count,FILE * restrict _File);
  FILE *__cdecl freopen(const char * restrict _Filename,const char * restrict _Mode,FILE * restrict _File) ;
  int __cdecl fsetpos(FILE *_File,const fpos_t *_Pos);
  int __cdecl fsetpos64(FILE *_File,const fpos_t *_Pos); /* fsetpos already 64bit */
  int __cdecl fseek(FILE *_File,long _Offset,int _Origin);
  long __cdecl ftell(FILE *_File);

  /* Shouldn't be any fseeko32 in glibc, 32bit to 64bit casting should be fine */
  /* int fseeko32(FILE* stream, _off_t offset, int whence);*/ /* fseeko32 redirects to fseeko64 */
  __declspec(dllimport) int __cdecl _fseeki64(FILE *_File,long long _Offset,int _Origin);
  __declspec(dllimport) long long __cdecl _ftelli64(FILE *_File);
# 678 "B:/occ/_include-mingw/stdio.h"

int fseeko64(FILE* stream, _off64_t offset, int whence);
  int fseeko(FILE* stream, _off_t offset, int whence);
  /* Returns truncated 64bit off_t */
  _off_t ftello(FILE * stream);
  _off64_t ftello64(FILE * stream);




# 690 "B:/occ/_include-mingw/stdio.h"





# 697 "B:/occ/_include-mingw/stdio.h"



  size_t __cdecl fwrite(const void * restrict _Str,size_t _Size,size_t _Count,FILE * restrict _File);
  int __cdecl getc(FILE *_File);
  int __cdecl getchar(void);
  __declspec(dllimport) int __cdecl _getmaxstdio(void);
  char *__cdecl gets(char *_Buffer) ;
  int __cdecl _getw(FILE *_File);


void __cdecl perror(const char *_ErrMsg);

__declspec(dllimport) int __cdecl _pclose(FILE *_File);
  __declspec(dllimport) FILE *__cdecl _popen(const char *_Command,const char *_Mode);




int __cdecl putc(int _Ch,FILE *_File);
  int __cdecl putchar(int _Ch);
  int __cdecl puts(const char *_Str);
  __declspec(dllimport) int __cdecl _putw(int _Word,FILE *_File);


int __cdecl remove(const char *_Filename);
  int __cdecl rename(const char *_OldFilename,const char *_NewFilename);
  __declspec(dllimport) int __cdecl _unlink(const char *_Filename);

int __cdecl unlink(const char *_Filename) ;


void __cdecl rewind(FILE *_File);
  __declspec(dllimport) int __cdecl _rmtmp(void);
  void __cdecl setbuf(FILE * restrict _File,char * restrict _Buffer) ;
  __declspec(dllimport) int __cdecl _setmaxstdio(int _Max);
  __declspec(dllimport) unsigned int __cdecl _set_output_format(unsigned int _Format);
  __declspec(dllimport) unsigned int __cdecl _get_output_format(void);
  int __cdecl setvbuf(FILE * restrict _File,char * restrict _Buf,int _Mode,size_t _Size);
# 757 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) int __cdecl _scprintf(const char * restrict _Format,...);
  __declspec(dllimport) int __cdecl _snscanf(const char * restrict _Src,size_t _MaxCount,const char * restrict _Format,...) ;

FILE *__cdecl tmpfile(void) ;
  char *__cdecl tmpnam(char *_Buffer);
  int __cdecl ungetc(int _Ch,FILE *_File);

# 779 "B:/occ/_include-mingw/stdio.h"


  __declspec(dllimport) int __cdecl _snprintf(char * restrict _Dest,size_t _Count,const char * restrict _Format,...) ;
  
  __declspec(dllimport) int __cdecl _vsnprintf(char * restrict _Dest,size_t _Count,const char * restrict _Format,va_list _Args) ;


# 824 "B:/occ/_include-mingw/stdio.h"




# 841 "B:/occ/_include-mingw/stdio.h"


/* this is here to deal with software defining
 * vsnprintf as _vsnprintf, eg. libxml2.  */

# 849 "B:/occ/_include-mingw/stdio.h"


#pragma push_macro("snprintf")
#pragma push_macro("vsnprintf")



  int __cdecl __ms_vsnprintf(char * restrict d,size_t n,const char * restrict format,va_list arg)
    ;

  static __cdecl
  
  int vsnprintf (char * restrict __stream, size_t __n, const char * restrict __format, va_list __local_argv)
  {
# 865 "B:/occ/_include-mingw/stdio.h"

return __ms_vsnprintf (__stream, __n, __format, __local_argv);
  }

  
  int __cdecl __ms_snprintf(char * restrict s, size_t n, const char * restrict  format, ...);


# 883 "B:/occ/_include-mingw/stdio.h"


static __cdecl

int snprintf (char * restrict __stream, size_t __n, const char * restrict __format, ...)
{
  int __retval;
  void* __local_argv; __builtin_va_start( __local_argv, __format );
  __retval = __ms_vsnprintf (__stream, __n, __format, __local_argv);
  __builtin_va_end( __local_argv );
  return __retval;
}




# 916 "B:/occ/_include-mingw/stdio.h"


# 935 "B:/occ/_include-mingw/stdio.h"


#pragma pop_macro ("vsnprintf")
#pragma pop_macro ("snprintf")
# 941 "B:/occ/_include-mingw/stdio.h"




# 951 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) int __cdecl _vscprintf(const char * restrict _Format,va_list _ArgList);


  __declspec(dllimport) int __cdecl _set_printf_count_output(int _Value);
  __declspec(dllimport) int __cdecl _get_printf_count_output(void);




/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ 
  int __cdecl __mingw_swscanf(const wchar_t * restrict _Src,const wchar_t * restrict _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ 
  int __cdecl __mingw_vswscanf (const wchar_t * restrict _Str,const wchar_t * restrict Format,va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 1, 2))) */ 
  int __cdecl __mingw_wscanf(const wchar_t * restrict _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 1, 0))) */ 
  int __cdecl __mingw_vwscanf(const wchar_t * restrict Format, va_list argp);
/* __attribute__((__format__ (gnu_wscanf, 2, 3))) */ 
  int __cdecl __mingw_fwscanf(FILE * restrict _File,const wchar_t * restrict _Format,...);
/* __attribute__((__format__ (gnu_wscanf, 2, 0))) */ 
  int __cdecl __mingw_vfwscanf (FILE * restrict fp, const wchar_t * restrict Format,va_list argp);

/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ 
  int __cdecl __mingw_fwprintf(FILE * restrict _File,const wchar_t * restrict _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 1, 2))) */ 
  int __cdecl __mingw_wprintf(const wchar_t * restrict _Format,...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */
  int __cdecl __mingw_vfwprintf(FILE * restrict _File,const wchar_t * restrict _Format,va_list _ArgList);
/*__attribute__((__format__ (gnu_wprintf, 1, 0))) */ 
  int __cdecl __mingw_vwprintf(const wchar_t * restrict _Format,va_list _ArgList);
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ 
  int __cdecl __mingw_snwprintf (wchar_t * restrict s, size_t n, const wchar_t * restrict format, ...);
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ 
  int __cdecl __mingw_vsnwprintf (wchar_t * restrict , size_t, const wchar_t * restrict , va_list);
/* __attribute__((__format__ (gnu_wprintf, 2, 3))) */ 
  int __cdecl __mingw_swprintf(wchar_t * restrict , const wchar_t * restrict , ...);
/* __attribute__((__format__ (gnu_wprintf, 2, 0))) */ 
  int __cdecl __mingw_vswprintf(wchar_t * restrict , const wchar_t * restrict ,va_list);

# 996 "B:/occ/_include-mingw/stdio.h"


# 1135 "B:/occ/_include-mingw/stdio.h"


# 1218 "B:/occ/_include-mingw/stdio.h"


  int __cdecl fwscanf(FILE * restrict _File,const wchar_t * restrict _Format,...) ;
  int __cdecl swscanf(const wchar_t * restrict _Src,const wchar_t * restrict _Format,...) ;
  int __cdecl wscanf(const wchar_t * restrict _Format,...) ;

int __cdecl __ms_vwscanf (const wchar_t * restrict , va_list);
  int __cdecl __ms_vfwscanf (FILE * restrict ,const wchar_t * restrict ,va_list);
  int __cdecl __ms_vswscanf (const wchar_t * restrict ,const wchar_t * restrict ,va_list);

  static __cdecl
  
  int vfwscanf (FILE *__stream,  const wchar_t *__format, void* __local_argv)
  {
    return __ms_vfwscanf (__stream, __format, __local_argv);
  }

  static __cdecl
  
  int vswscanf (const wchar_t * restrict __source, const wchar_t * restrict __format, void* __local_argv)
  {
    return __ms_vswscanf( __source, __format, __local_argv );
  }
  static __cdecl
  
  int vwscanf(const wchar_t *__format,  void* __local_argv)
  {
    return __ms_vwscanf (__format, __local_argv);
  }



  int __cdecl fwprintf(FILE * restrict _File,const wchar_t * restrict _Format,...);
  int __cdecl wprintf(const wchar_t * restrict _Format,...);
  int __cdecl vfwprintf(FILE * restrict _File,const wchar_t * restrict _Format,va_list _ArgList);
  int __cdecl vwprintf(const wchar_t * restrict _Format,va_list _ArgList);







# 1263 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) FILE *__cdecl _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);


  wint_t __cdecl fgetwc(FILE *_File);
  __declspec(dllimport) wint_t __cdecl _fgetwchar(void);
  wint_t __cdecl fputwc(wchar_t _Ch,FILE *_File);
  __declspec(dllimport) wint_t __cdecl _fputwchar(wchar_t _Ch);
  wint_t __cdecl getwc(FILE *_File);
  wint_t __cdecl getwchar(void);
  wint_t __cdecl putwc(wchar_t _Ch,FILE *_File);
  wint_t __cdecl putwchar(wchar_t _Ch);
  wint_t __cdecl ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__cdecl fgetws(wchar_t * restrict _Dst,int _SizeInWords,FILE * restrict _File);
  int __cdecl fputws(const wchar_t * restrict _Str,FILE * restrict _File);
  __declspec(dllimport) wchar_t *__cdecl _getws(wchar_t *_String) ;
  __declspec(dllimport) int __cdecl _putws(const wchar_t *_Str);

# 1344 "B:/occ/_include-mingw/stdio.h"

__declspec(dllimport) int __cdecl _scwprintf(const wchar_t * restrict _Format,...);
  __declspec(dllimport) int __cdecl _swprintf_c(wchar_t * restrict _DstBuf,size_t _SizeInWords,const wchar_t * restrict _Format,...);
  __declspec(dllimport) int __cdecl _vswprintf_c(wchar_t * restrict _DstBuf,size_t _SizeInWords,const wchar_t * restrict _Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _snwprintf(wchar_t * restrict _Dest,size_t _Count,const wchar_t * restrict _Format,...) ;
  __declspec(dllimport) int __cdecl _vsnwprintf(wchar_t * restrict _Dest,size_t _Count,const wchar_t * restrict _Format,va_list _Args) ;
  __declspec(dllimport) int __cdecl _vscwprintf(const wchar_t * restrict _Format,va_list _ArgList);




#pragma push_macro("snwprintf")
#pragma push_macro("vsnwprintf")


int __cdecl __ms_snwprintf (wchar_t * restrict s, size_t n, const wchar_t * restrict format, ...);
  int __cdecl __ms_vsnwprintf (wchar_t * restrict , size_t, const wchar_t * restrict , va_list);
  static __cdecl
  int snwprintf (wchar_t * restrict s, size_t n, const wchar_t * restrict format, ...)
  {
    int r;
    va_list argp;
    __builtin_va_start (argp, format);
    r = _vsnwprintf (s, n, format, argp);
    __builtin_va_end (argp);
    return r;
  }
  static __cdecl
  int __cdecl vsnwprintf (wchar_t * restrict s, size_t n, const wchar_t * restrict format, va_list arg)
  {
    return _vsnwprintf(s,n,format,arg);
  }
#pragma pop_macro ("vsnwprintf")
#pragma pop_macro ("snwprintf")



__declspec(dllimport) int __cdecl _swprintf(wchar_t * restrict _Dest,const wchar_t * restrict _Format,...);
  __declspec(dllimport) int __cdecl _vswprintf(wchar_t * restrict _Dest,const wchar_t * restrict _Format,va_list _Args);



# 1 "B:/occ/_include-mingw/swprintf.inl" 1





# 1 "B:/occ/_include-mingw/vadefs.h" 1

# 107 "B:/occ/_include-mingw/vadefs.h"


# 11 "B:/occ/_include-mingw/swprintf.inl" 2



# 22 "B:/occ/_include-mingw/swprintf.inl"
# 22 "B:/occ/_include-mingw/swprintf.inl"




static __cdecl
/* __attribute__((__format__ (gnu_wprintf, 3, 0))) */ 
int vswprintf (wchar_t *__stream, size_t __count, const wchar_t *__format, void* __local_argv)
{
  return vsnwprintf( __stream, __count, __format, __local_argv );
}

static __cdecl
/* __attribute__((__format__ (gnu_wprintf, 3, 4))) */ 
int swprintf (wchar_t *__stream, size_t __count, const wchar_t *__format, ...)
{
  int __retval;
  void* __local_argv;

  __builtin_va_start( __local_argv, __format );
  __retval = vswprintf( __stream, __count, __format, __local_argv );
  __builtin_va_end( __local_argv );
  return __retval;
}

# 86 "B:/occ/_include-mingw/swprintf.inl"
# 86 "B:/occ/_include-mingw/swprintf.inl"



# 1386 "B:/occ/_include-mingw/stdio.h" 2



# 1394 "B:/occ/_include-mingw/stdio.h"


  __declspec(dllimport) wchar_t *__cdecl _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  __declspec(dllimport) int __cdecl _snwscanf(const wchar_t * restrict _Src,size_t _MaxCount,const wchar_t * restrict _Format,...);
  __declspec(dllimport) FILE *__cdecl _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  __declspec(dllimport) FILE *__cdecl _wfopen(const wchar_t * restrict _Filename,const wchar_t *restrict  _Mode) ;
  __declspec(dllimport) FILE *__cdecl _wfreopen(const wchar_t * restrict _Filename,const wchar_t * restrict _Mode,FILE * restrict _OldFile) ;



__declspec(dllimport) void __cdecl _wperror(const wchar_t *_ErrMsg);

__declspec(dllimport) FILE *__cdecl _wpopen(const wchar_t *_Command,const wchar_t *_Mode);




  __declspec(dllimport) int __cdecl _wremove(const wchar_t *_Filename);
  __declspec(dllimport) wchar_t *__cdecl _wtmpnam(wchar_t *_Buffer);
  __declspec(dllimport) wint_t __cdecl _fgetwc_nolock(FILE *_File);
  __declspec(dllimport) wint_t __cdecl _fputwc_nolock(wchar_t _Ch,FILE *_File);
  __declspec(dllimport) wint_t __cdecl _ungetwc_nolock(wint_t _Ch,FILE *_File);






# 1425 "B:/occ/_include-mingw/stdio.h"











# 1441 "B:/occ/_include-mingw/stdio.h"











  __declspec(dllimport) void __cdecl _lock_file(FILE *_File);
  __declspec(dllimport) void __cdecl _unlock_file(FILE *_File);
  __declspec(dllimport) int __cdecl _fclose_nolock(FILE *_File);
  __declspec(dllimport) int __cdecl _fflush_nolock(FILE *_File);
  __declspec(dllimport) size_t __cdecl _fread_nolock(void * restrict _DstBuf,size_t _ElementSize,size_t _Count,FILE * restrict _File);
  __declspec(dllimport) int __cdecl _fseek_nolock(FILE *_File,long _Offset,int _Origin);
  __declspec(dllimport) long __cdecl _ftell_nolock(FILE *_File);
  __declspec(dllimport) int __cdecl _fseeki64_nolock(FILE *_File,long long _Offset,int _Origin);
  __declspec(dllimport) long long __cdecl _ftelli64_nolock(FILE *_File);
  __declspec(dllimport) size_t __cdecl _fwrite_nolock(const void * restrict _DstBuf,size_t _Size,size_t _Count,FILE * restrict _File);
  __declspec(dllimport) int __cdecl _ungetc_nolock(int _Ch,FILE *_File);





  char *__cdecl tempnam(const char *_Directory,const char *_FilePrefix) ;
  int __cdecl fcloseall(void) ;
  FILE *__cdecl fdopen(int _FileHandle,const char *_Format) ;
  int __cdecl fgetchar(void) ;
  int __cdecl fileno(FILE *_File) ;
  int __cdecl flushall(void) ;
  int __cdecl fputchar(int _Ch) ;
  int __cdecl getw(FILE *_File) ;
  int __cdecl putw(int _Ch,FILE *_File) ;
  int __cdecl rmtmp(void) ;





/**
 * __mingw_str_wide_utf8
 * Converts a null terminated UCS-2 string to a multibyte (UTF-8) equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[in] wptr Pointer to wide string.
 * @param[out] mbptr Pointer to multibyte string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return Number of characters converted, 0 for failure.
 *
 * WideCharToMultiByte - http://msdn.microsoft.com/en-us/library/dd374130(VS.85).aspx
 */
int __cdecl __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);

/**
 * __mingw_str_utf8_wide
 * Converts a null terminated UTF-8 string to a UCS-2 equivalent.
 * Caller is supposed to free allocated buffer with __mingw_str_free().
 * @param[out] mbptr Pointer to multibyte string.
 * @param[in] wptr Pointer to wide string.
 * @param[out] buflen Optional parameter for length of allocated buffer.
 * @return Number of characters converted, 0 for failure.
 *
 * MultiByteToWideChar - http://msdn.microsoft.com/en-us/library/dd319072(VS.85).aspx
 */

int __cdecl __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);

/**
 * __mingw_str_free
 * Frees buffer create by __mingw_str_wide_utf8 and __mingw_str_utf8_wide.
 * @param[in] ptr memory block to free.
 *
 */

void __cdecl __mingw_str_free(void *ptr);





__declspec(dllimport) intptr_t __cdecl _wspawnl(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __declspec(dllimport) intptr_t __cdecl _wspawnle(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __declspec(dllimport) intptr_t __cdecl _wspawnlp(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __declspec(dllimport) intptr_t __cdecl _wspawnlpe(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __declspec(dllimport) intptr_t __cdecl _wspawnv(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __declspec(dllimport) intptr_t __cdecl _wspawnve(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  __declspec(dllimport) intptr_t __cdecl _wspawnvp(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __declspec(dllimport) intptr_t __cdecl _wspawnvpe(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
















__declspec(dllimport) intptr_t __cdecl _spawnv(int _Mode,const char *_Filename,const char *const *_ArgList);
  __declspec(dllimport) intptr_t __cdecl _spawnve(int _Mode,const char *_Filename,const char *const *_ArgList,const char *const *_Env);
  __declspec(dllimport) intptr_t __cdecl _spawnvp(int _Mode,const char *_Filename,const char *const *_ArgList);
  __declspec(dllimport) intptr_t __cdecl _spawnvpe(int _Mode,const char *_Filename,const char *const *_ArgList,const char *const *_Env);


# 1555 "B:/occ/_include-mingw/stdio.h"


#pragma pack(pop)

# 1 "B:/occ/_include-mingw/sec_api/stdio_s.h" 1




# 1 "B:/occ/_include-mingw/stdio.h" 1

# 1561 "B:/occ/_include-mingw/stdio.h"

# 10 "B:/occ/_include-mingw/sec_api/stdio_s.h" 2


# 15 "B:/occ/_include-mingw/sec_api/stdio_s.h"






# 23 "B:/occ/_include-mingw/sec_api/stdio_s.h"




__declspec(dllimport) errno_t __cdecl clearerr_s(FILE *_File);

  size_t __cdecl fread_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);

# 469 "B:/occ/_include-mingw/sec_api/stdio_s.h"

int __cdecl fprintf_s(FILE *_File,const char *_Format,...);
  __declspec(dllimport) int __cdecl _fscanf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  int __cdecl printf_s(const char *_Format,...);
  __declspec(dllimport) int __cdecl _scanf_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _scanf_s_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _snprintf_c(char *_DstBuf,size_t _MaxCount,const char *_Format,...);
  __declspec(dllimport) int __cdecl _vsnprintf_c(char *_DstBuf,size_t _MaxCount,const char *_Format,va_list _ArgList);

  __declspec(dllimport) int __cdecl _fscanf_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _sscanf_l(const char *_Src,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _sscanf_s_l(const char *_Src,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl sscanf_s(const char *_Src,const char *_Format,...);
  __declspec(dllimport) int __cdecl _snscanf_s(const char *_Src,size_t _MaxCount,const char *_Format,...);
  __declspec(dllimport) int __cdecl _snscanf_l(const char *_Src,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _snscanf_s_l(const char *_Src,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  int __cdecl vfprintf_s(FILE *_File,const char *_Format,va_list _ArgList);
  int __cdecl vprintf_s(const char *_Format,va_list _ArgList);

  int __cdecl vsnprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,va_list _ArgList);

  __declspec(dllimport) int __cdecl _vsnprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,va_list _ArgList);

  __declspec(dllimport) int __cdecl vsprintf_s(char *_DstBuf,size_t _Size,const char *_Format,va_list _ArgList);

  __declspec(dllimport) int __cdecl sprintf_s(char *_DstBuf,size_t _DstSize,const char *_Format,...);

  __declspec(dllimport) int __cdecl _snprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,...);

  __declspec(dllimport) int __cdecl _fprintf_p(FILE *_File,const char *_Format,...);
  __declspec(dllimport) int __cdecl _printf_p(const char *_Format,...);
  __declspec(dllimport) int __cdecl _sprintf_p(char *_Dst,size_t _MaxCount,const char *_Format,...);
  __declspec(dllimport) int __cdecl _vfprintf_p(FILE *_File,const char *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vprintf_p(const char *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vsprintf_p(char *_Dst,size_t _MaxCount,const char *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _scprintf_p(const char *_Format,...);
  __declspec(dllimport) int __cdecl _vscprintf_p(const char *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _printf_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _printf_p_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vprintf_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vprintf_p_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _fprintf_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _fprintf_p_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vfprintf_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vfprintf_p_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _sprintf_l(char *_DstBuf,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _sprintf_p_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vsprintf_l(char *_DstBuf,const char *_Format,_locale_t,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vsprintf_p_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _scprintf_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _scprintf_p_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vscprintf_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vscprintf_p_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _printf_s_l(const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vprintf_s_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _fprintf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vfprintf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _sprintf_s_l(char *_DstBuf,size_t _DstSize,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vsprintf_s_l(char *_DstBuf,size_t _DstSize,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _snprintf_s_l(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vsnprintf_s_l(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _snprintf_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _snprintf_c_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vsnprintf_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vsnprintf_c_l(char *_DstBuf,size_t _MaxCount,const char *,_locale_t _Locale,va_list _ArgList);


  
  
  
  
  

  __declspec(dllimport) errno_t __cdecl fopen_s(FILE **_File,const char *_Filename,const char *_Mode);
  __declspec(dllimport) errno_t __cdecl freopen_s(FILE** _File, const char *_Filename, const char *_Mode, FILE *_Stream);

  __declspec(dllimport) char* __cdecl gets_s(char*,rsize_t);
  

  __declspec(dllimport) errno_t __cdecl tmpnam_s(char*,rsize_t);
  




__declspec(dllimport) wchar_t *__cdecl _getws_s(wchar_t *_Str,size_t _SizeInWords);
  

# 737 "B:/occ/_include-mingw/sec_api/stdio_s.h"

int __cdecl fwprintf_s(FILE *_File,const wchar_t *_Format,...);
  int __cdecl wprintf_s(const wchar_t *_Format,...);
  int __cdecl vfwprintf_s(FILE *_File,const wchar_t *_Format,va_list _ArgList);
  int __cdecl vwprintf_s(const wchar_t *_Format,va_list _ArgList);

  int __cdecl vswprintf_s(wchar_t *_Dst,size_t _SizeInWords,const wchar_t *_Format,va_list _ArgList);

  int __cdecl swprintf_s(wchar_t *_Dst,size_t _SizeInWords,const wchar_t *_Format,...);

  __declspec(dllimport) int __cdecl _vsnwprintf_s(wchar_t *_DstBuf,size_t _DstSizeInWords,size_t _MaxCount,const wchar_t *_Format,va_list _ArgList);

  __declspec(dllimport) int __cdecl _snwprintf_s(wchar_t *_DstBuf,size_t _DstSizeInWords,size_t _MaxCount,const wchar_t *_Format,...);


  __declspec(dllimport) int __cdecl _wprintf_s_l(const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vwprintf_s_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _fwprintf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vfwprintf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _swprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vswprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _snwprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vsnwprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _fwscanf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _swscanf_s_l(const wchar_t *_Src,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl swscanf_s(const wchar_t *_Src,const wchar_t *_Format,...);
  __declspec(dllimport) int __cdecl _snwscanf_s(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,...);
  __declspec(dllimport) int __cdecl _snwscanf_s_l(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _wscanf_s_l(const wchar_t *_Format,_locale_t _Locale,...);


  
  
  
  

  __declspec(dllimport) errno_t __cdecl _wfopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode);
  __declspec(dllimport) errno_t __cdecl _wfreopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode,FILE *_OldFile);

  __declspec(dllimport) errno_t __cdecl _wtmpnam_s(wchar_t *_DstBuf,size_t _SizeInWords);
  


__declspec(dllimport) int __cdecl _fwprintf_p(FILE *_File,const wchar_t *_Format,...);
  __declspec(dllimport) int __cdecl _wprintf_p(const wchar_t *_Format,...);
  __declspec(dllimport) int __cdecl _vfwprintf_p(FILE *_File,const wchar_t *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vwprintf_p(const wchar_t *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _swprintf_p(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,...);
  __declspec(dllimport) int __cdecl _vswprintf_p(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _scwprintf_p(const wchar_t *_Format,...);
  __declspec(dllimport) int __cdecl _vscwprintf_p(const wchar_t *_Format,va_list _ArgList);
  __declspec(dllimport) int __cdecl _wprintf_l(const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _wprintf_p_l(const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vwprintf_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vwprintf_p_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _fwprintf_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _fwprintf_p_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vfwprintf_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vfwprintf_p_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _swprintf_c_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _swprintf_p_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vswprintf_c_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _vswprintf_p_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _scwprintf_l(const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _scwprintf_p_l(const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vscwprintf_p_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _snwprintf_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _vsnwprintf_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl __swprintf_l(wchar_t *_Dest,const wchar_t *_Format,_locale_t _Plocinfo,...);
  __declspec(dllimport) int __cdecl __vswprintf_l(wchar_t *_Dest,const wchar_t *_Format,_locale_t _Plocinfo,va_list _Args);
  __declspec(dllimport) int __cdecl _vscwprintf_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  __declspec(dllimport) int __cdecl _fwscanf_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _swscanf_l(const wchar_t *_Src,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _snwscanf_l(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  __declspec(dllimport) int __cdecl _wscanf_l(const wchar_t *_Format,_locale_t _Locale,...);





  __declspec(dllimport) size_t __cdecl _fread_nolock_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);

# 821 "B:/occ/_include-mingw/sec_api/stdio_s.h"


# 1559 "B:/occ/_include-mingw/stdio.h" 2



# 41 "B:/occ/tests/pp-test.c" 2




int main(int argc, char* argv[])
{
	int typedef a;
	a f(void);
	a v(void);
	
	a (*pp[2])(void) = {
		f, v
	};
	
	pp[1] = &argc;
	
	printf("address of 'f' is %p\n", pp[0]);
	printf("this is line " "5" "!\n");
}
