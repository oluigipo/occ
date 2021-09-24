# 1 "tests/pp-test.c"

# 18 "tests/pp-test.c"
const char* name = "pepe";
void* hello(void)
{
	return "world";
}

int an_int = 0xffffULL;
float a_float = 35.0f;
float wtf = 0x8080.0p+3f;

const char* a_string = "pepe, " "MY_MACRO" " is " "hello";

# 1 "c:/programs/tcc/include/stdio.h" 1
# 9 "c:/programs/tcc/include/stdio.h"
# 1 "c:/programs/tcc/include/_mingw.h" 1
# 27 "c:/programs/tcc/include/_mingw.h"
# 1 "c:/programs/tcc/include/stddef.h" 1



typedef unsigned long long size_t;
typedef long long ssize_t;
typedef unsigned short wchar_t;
typedef long long ptrdiff_t;
typedef long long intptr_t;
typedef unsigned long long uintptr_t;



typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;



typedef signed long long int int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;



typedef unsigned long long int uint64_t;
# 37 "c:/programs/tcc/include/stddef.h"
void *alloca(size_t size);
# 28 "c:/programs/tcc/include/_mingw.h" 2
# 1 "c:/programs/tcc/include/stdarg.h" 1
# 34 "c:/programs/tcc/include/stdarg.h"
typedef char *va_list;
# 76 "c:/programs/tcc/include/stdarg.h"
typedef va_list __gnuc_va_list;
# 29 "c:/programs/tcc/include/_mingw.h" 2
# 104 "c:/programs/tcc/include/_mingw.h"
typedef long __time32_t;




typedef long long __time64_t;







typedef __time64_t time_t;





typedef wchar_t wctype_t;




typedef unsigned short wint_t;


typedef int errno_t;


typedef struct threadlocaleinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;
typedef struct localeinfo_struct _locale_tstruct,*_locale_t;
# 10 "c:/programs/tcc/include/stdio.h" 2

#pragma pack(push,8)
# 24 "c:/programs/tcc/include/stdio.h"
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
# 78 "c:/programs/tcc/include/stdio.h"
  typedef long _off_t;

  typedef long off_t;






  typedef long long _off64_t;

  typedef long long off64_t;





  extern FILE * __iob_func(void);
# 113 "c:/programs/tcc/include/stdio.h"
  typedef long long fpos_t;
# 150 "c:/programs/tcc/include/stdio.h"
  extern int  _filbuf(FILE *_File);
  extern int  _flsbuf(int _Ch,FILE *_File);



  extern FILE * _fsopen(const char *_Filename,const char *_Mode,int _ShFlag);

  void  clearerr(FILE *_File);
  int  fclose(FILE *_File);
  extern int  _fcloseall(void);



  extern FILE * _fdopen(int _FileHandle,const char *_Mode);

  int  feof(FILE *_File);
  int  ferror(FILE *_File);
  int  fflush(FILE *_File);
  int  fgetc(FILE *_File);
  extern int  _fgetchar(void);
  int  fgetpos(FILE *_File ,fpos_t *_Pos);
  char * fgets(char *_Buf,int _MaxCount,FILE *_File);



  extern int  _fileno(FILE *_File);

  extern char * _tempnam(const char *_DirName,const char *_FilePrefix);
  extern int  _flushall(void);
  FILE * fopen(const char *_Filename,const char *_Mode);
  FILE *fopen64(const char *filename,const char *mode);
  int  fprintf(FILE *_File,const char *_Format,...);
  int  fputc(int _Ch,FILE *_File);
  extern int  _fputchar(int _Ch);
  int  fputs(const char *_Str,FILE *_File);
  size_t  fread(void *_DstBuf,size_t _ElementSize,size_t _Count,FILE *_File);
  FILE * freopen(const char *_Filename,const char *_Mode,FILE *_File);
  int  fscanf(FILE *_File,const char *_Format,...);
  int  fsetpos(FILE *_File,const fpos_t *_Pos);
  int  fseek(FILE *_File,long _Offset,int _Origin);
   int fseeko64(FILE* stream, _off64_t offset, int whence);
  long  ftell(FILE *_File);
  _off64_t ftello64(FILE * stream);
  int  _fseeki64(FILE *_File,long long _Offset,int _Origin);
  long long  _ftelli64(FILE *_File);
  size_t  fwrite(const void *_Str,size_t _Size,size_t _Count,FILE *_File);
  int  getc(FILE *_File);
  int  getchar(void);
  extern int  _getmaxstdio(void);
  char * gets(char *_Buffer);
  int  _getw(FILE *_File);


  void  perror(const char *_ErrMsg);

  extern int  _pclose(FILE *_File);
  extern FILE * _popen(const char *_Command,const char *_Mode);




  int  printf(const char *_Format,...);
  int  putc(int _Ch,FILE *_File);
  int  putchar(int _Ch);
  int  puts(const char *_Str);
  extern int  _putw(int _Word,FILE *_File);


  int  remove(const char *_Filename);
  int  rename(const char *_OldFilename,const char *_NewFilename);
  extern int  _unlink(const char *_Filename);

  int  unlink(const char *_Filename);


  void  rewind(FILE *_File);
  extern int  _rmtmp(void);
  int  scanf(const char *_Format,...);
  void  setbuf(FILE *_File,char *_Buffer);
  extern int  _setmaxstdio(int _Max);
  extern unsigned int  _set_output_format(unsigned int _Format);
  extern unsigned int  _get_output_format(void);
  int  setvbuf(FILE *_File,char *_Buf,int _Mode,size_t _Size);
  extern int  _scprintf(const char *_Format,...);
  int  sscanf(const char *_Src,const char *_Format,...);
  extern int  _snscanf(const char *_Src,size_t _MaxCount,const char *_Format,...);
  FILE * tmpfile(void);
  char * tmpnam(char *_Buffer);
  int  ungetc(int _Ch,FILE *_File);
  int  vfprintf(FILE *_File,const char *_Format,va_list _ArgList);
  int  vprintf(const char *_Format,va_list _ArgList);





  extern
  __attribute__((format(gnu_printf, 3, 0))) __attribute__((nonnull (3)))
  int __mingw_vsnprintf(char *_DstBuf,size_t _MaxCount,const char *_Format,va_list _ArgList);
  extern
  __attribute__((format(gnu_printf, 3, 4))) __attribute__((nonnull (3)))
  int __mingw_snprintf(char* s, size_t n, const char*  format, ...);
  int  vsnprintf(char *_DstBuf,size_t _MaxCount,const char *_Format,va_list _ArgList);
  extern int  _snprintf(char *_Dest,size_t _Count,const char *_Format,...);
  extern int  _vsnprintf(char *_Dest,size_t _Count,const char *_Format,va_list _Args);
  int  sprintf(char *_Dest,const char *_Format,...);
  int  vsprintf(char *_Dest,const char *_Format,va_list _Args);

  int  snprintf(char* s, size_t n, const char*  format, ...);
  extern __inline__ int  vsnprintf (char* s, size_t n, const char* format,va_list arg) {
    return _vsnprintf ( s, n, format, arg);
  }
  int  vscanf(const char * Format, va_list argp);
  int  vfscanf (FILE * fp, const char * Format,va_list argp);
  int  vsscanf (const char * _Str,const char * Format,va_list argp);
# 278 "c:/programs/tcc/include/stdio.h"
  extern int  _vscprintf(const char *_Format,va_list _ArgList);
  extern int  _set_printf_count_output(int _Value);
  extern int  _get_printf_count_output(void);
# 291 "c:/programs/tcc/include/stdio.h"
  extern FILE * _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);

  wint_t  fgetwc(FILE *_File);
  extern wint_t  _fgetwchar(void);
  wint_t  fputwc(wchar_t _Ch,FILE *_File);
  extern wint_t  _fputwchar(wchar_t _Ch);
  wint_t  getwc(FILE *_File);
  wint_t  getwchar(void);
  wint_t  putwc(wchar_t _Ch,FILE *_File);
  wint_t  putwchar(wchar_t _Ch);
  wint_t  ungetwc(wint_t _Ch,FILE *_File);
  wchar_t * fgetws(wchar_t *_Dst,int _SizeInWords,FILE *_File);
  int  fputws(const wchar_t *_Str,FILE *_File);
  extern wchar_t * _getws(wchar_t *_String);
  extern int  _putws(const wchar_t *_Str);
  int  fwprintf(FILE *_File,const wchar_t *_Format,...);
  int  wprintf(const wchar_t *_Format,...);
  extern int  _scwprintf(const wchar_t *_Format,...);
  int  vfwprintf(FILE *_File,const wchar_t *_Format,va_list _ArgList);
  int  vwprintf(const wchar_t *_Format,va_list _ArgList);
  extern int  swprintf(wchar_t*, const wchar_t*, ...);
  extern int  vswprintf(wchar_t*, const wchar_t*,va_list);
  extern int  _swprintf_c(wchar_t *_DstBuf,size_t _SizeInWords,const wchar_t *_Format,...);
  extern int  _vswprintf_c(wchar_t *_DstBuf,size_t _SizeInWords,const wchar_t *_Format,va_list _ArgList);
  extern int  _snwprintf(wchar_t *_Dest,size_t _Count,const wchar_t *_Format,...);
  extern int  _vsnwprintf(wchar_t *_Dest,size_t _Count,const wchar_t *_Format,va_list _Args);

  int  snwprintf (wchar_t* s, size_t n, const wchar_t*  format, ...);
  extern __inline__ int  vsnwprintf (wchar_t* s, size_t n, const wchar_t* format, va_list arg) { return _vsnwprintf(s,n,format,arg); }
  int  vwscanf (const wchar_t *, va_list);
  int  vfwscanf (FILE *,const wchar_t *,va_list);
  int  vswscanf (const wchar_t *,const wchar_t *,va_list);

  extern int  _swprintf(wchar_t *_Dest,const wchar_t *_Format,...);
  extern int  _vswprintf(wchar_t *_Dest,const wchar_t *_Format,va_list _Args);


# 1 "c:/programs/tcc/include/vadefs.h" 1
# 329 "c:/programs/tcc/include/stdio.h" 2
# 340 "c:/programs/tcc/include/stdio.h"
  extern wchar_t * _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);
  extern int  _vscwprintf(const wchar_t *_Format,va_list _ArgList);
  int  fwscanf(FILE *_File,const wchar_t *_Format,...);
  int  swscanf(const wchar_t *_Src,const wchar_t *_Format,...);
  extern int  _snwscanf(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,...);
  int  wscanf(const wchar_t *_Format,...);
  extern FILE * _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  extern FILE * _wfopen(const wchar_t *_Filename,const wchar_t *_Mode);
  extern FILE * _wfreopen(const wchar_t *_Filename,const wchar_t *_Mode,FILE *_OldFile);


  extern void  _wperror(const wchar_t *_ErrMsg);

  extern FILE * _wpopen(const wchar_t *_Command,const wchar_t *_Mode);



  extern int  _wremove(const wchar_t *_Filename);
  extern wchar_t * _wtmpnam(wchar_t *_Buffer);
  extern wint_t  _fgetwc_nolock(FILE *_File);
  extern wint_t  _fputwc_nolock(wchar_t _Ch,FILE *_File);
  extern wint_t  _ungetwc_nolock(wint_t _Ch,FILE *_File);
# 393 "c:/programs/tcc/include/stdio.h"
  extern void  _lock_file(FILE *_File);
  extern void  _unlock_file(FILE *_File);
  extern int  _fclose_nolock(FILE *_File);
  extern int  _fflush_nolock(FILE *_File);
  extern size_t  _fread_nolock(void *_DstBuf,size_t _ElementSize,size_t _Count,FILE *_File);
  extern int  _fseek_nolock(FILE *_File,long _Offset,int _Origin);
  extern long  _ftell_nolock(FILE *_File);
  extern int  _fseeki64_nolock(FILE *_File,long long _Offset,int _Origin);
  extern long long  _ftelli64_nolock(FILE *_File);
  extern size_t  _fwrite_nolock(const void *_DstBuf,size_t _Size,size_t _Count,FILE *_File);
  extern int  _ungetc_nolock(int _Ch,FILE *_File);





  char * tempnam(const char *_Directory,const char *_FilePrefix);
  int  fcloseall(void);
  FILE * fdopen(int _FileHandle,const char *_Format);
  int  fgetchar(void);
  int  fileno(FILE *_File);
  int  flushall(void);
  int  fputchar(int _Ch);
  int  getw(FILE *_File);
  int  putw(int _Ch,FILE *_File);
  int  rmtmp(void);






#pragma pack(pop)

# 1 "c:/programs/tcc/include/sec_api/stdio_s.h" 1
# 9 "c:/programs/tcc/include/sec_api/stdio_s.h"
# 1 "c:/programs/tcc/include/stdio.h" 1
# 10 "c:/programs/tcc/include/sec_api/stdio_s.h" 2
# 19 "c:/programs/tcc/include/sec_api/stdio_s.h"
  extern errno_t  clearerr_s(FILE *_File);
  int  fprintf_s(FILE *_File,const char *_Format,...);
  size_t  fread_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);
  extern int  _fscanf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  int  printf_s(const char *_Format,...);
  extern int  _scanf_l(const char *_Format,_locale_t _Locale,...);
  extern int  _scanf_s_l(const char *_Format,_locale_t _Locale,...);
  extern int  _snprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,...);
  extern int  _snprintf_c(char *_DstBuf,size_t _MaxCount,const char *_Format,...);
  extern int  _vsnprintf_c(char *_DstBuf,size_t _MaxCount,const char *_Format,va_list _ArgList);
  int  sprintf_s(char *_DstBuf,size_t _DstSize,const char *_Format,...);
  extern int  _fscanf_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  extern int  _sscanf_l(const char *_Src,const char *_Format,_locale_t _Locale,...);
  extern int  _sscanf_s_l(const char *_Src,const char *_Format,_locale_t _Locale,...);
  extern int  _snscanf_s(const char *_Src,size_t _MaxCount,const char *_Format,...);
  extern int  _snscanf_l(const char *_Src,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  extern int  _snscanf_s_l(const char *_Src,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  int  vfprintf_s(FILE *_File,const char *_Format,va_list _ArgList);
  int  vprintf_s(const char *_Format,va_list _ArgList);
  int  vsnprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,va_list _ArgList);
  extern int  _vsnprintf_s(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,va_list _ArgList);
  int  vsprintf_s(char *_DstBuf,size_t _Size,const char *_Format,va_list _ArgList);
  extern int  _fprintf_p(FILE *_File,const char *_Format,...);
  extern int  _printf_p(const char *_Format,...);
  extern int  _sprintf_p(char *_Dst,size_t _MaxCount,const char *_Format,...);
  extern int  _vfprintf_p(FILE *_File,const char *_Format,va_list _ArgList);
  extern int  _vprintf_p(const char *_Format,va_list _ArgList);
  extern int  _vsprintf_p(char *_Dst,size_t _MaxCount,const char *_Format,va_list _ArgList);
  extern int  _scprintf_p(const char *_Format,...);
  extern int  _vscprintf_p(const char *_Format,va_list _ArgList);
  extern int  _printf_l(const char *_Format,_locale_t _Locale,...);
  extern int  _printf_p_l(const char *_Format,_locale_t _Locale,...);
  extern int  _vprintf_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vprintf_p_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _fprintf_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  extern int  _fprintf_p_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  extern int  _vfprintf_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vfprintf_p_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _sprintf_l(char *_DstBuf,const char *_Format,_locale_t _Locale,...);
  extern int  _sprintf_p_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  extern int  _vsprintf_l(char *_DstBuf,const char *_Format,_locale_t,va_list _ArgList);
  extern int  _vsprintf_p_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _scprintf_l(const char *_Format,_locale_t _Locale,...);
  extern int  _scprintf_p_l(const char *_Format,_locale_t _Locale,...);
  extern int  _vscprintf_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vscprintf_p_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _printf_s_l(const char *_Format,_locale_t _Locale,...);
  extern int  _vprintf_s_l(const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _fprintf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,...);
  extern int  _vfprintf_s_l(FILE *_File,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _sprintf_s_l(char *_DstBuf,size_t _DstSize,const char *_Format,_locale_t _Locale,...);
  extern int  _vsprintf_s_l(char *_DstBuf,size_t _DstSize,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _snprintf_s_l(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  extern int  _vsnprintf_s_l(char *_DstBuf,size_t _DstSize,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _snprintf_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  extern int  _snprintf_c_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,...);
  extern int  _vsnprintf_l(char *_DstBuf,size_t _MaxCount,const char *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vsnprintf_c_l(char *_DstBuf,size_t _MaxCount,const char *,_locale_t _Locale,va_list _ArgList);



  extern wchar_t * _getws_s(wchar_t *_Str,size_t _SizeInWords);
  int  fwprintf_s(FILE *_File,const wchar_t *_Format,...);
  int  wprintf_s(const wchar_t *_Format,...);
  int  vwprintf_s(const wchar_t *_Format,va_list _ArgList);
  int  swprintf_s(wchar_t *_Dst,size_t _SizeInWords,const wchar_t *_Format,...);
  int  vswprintf_s(wchar_t *_Dst,size_t _SizeInWords,const wchar_t *_Format,va_list _ArgList);
  extern int  _snwprintf_s(wchar_t *_DstBuf,size_t _DstSizeInWords,size_t _MaxCount,const wchar_t *_Format,...);
  extern int  _vsnwprintf_s(wchar_t *_DstBuf,size_t _DstSizeInWords,size_t _MaxCount,const wchar_t *_Format,va_list _ArgList);
  extern int  _wprintf_s_l(const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vwprintf_s_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _fwprintf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vfwprintf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _swprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vswprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _snwprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vsnwprintf_s_l(wchar_t *_DstBuf,size_t _DstSize,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _fwscanf_s_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _swscanf_s_l(const wchar_t *_Src,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _snwscanf_s(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,...);
  extern int  _snwscanf_s_l(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _wscanf_s_l(const wchar_t *_Format,_locale_t _Locale,...);
  extern errno_t  _wfopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode);
  extern errno_t  _wfreopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode,FILE *_OldFile);
  extern errno_t  _wtmpnam_s(wchar_t *_DstBuf,size_t _SizeInWords);
  extern int  _fwprintf_p(FILE *_File,const wchar_t *_Format,...);
  extern int  _wprintf_p(const wchar_t *_Format,...);
  extern int  _vfwprintf_p(FILE *_File,const wchar_t *_Format,va_list _ArgList);
  extern int  _vwprintf_p(const wchar_t *_Format,va_list _ArgList);
  extern int  _swprintf_p(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,...);
  extern int  _vswprintf_p(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,va_list _ArgList);
  extern int  _scwprintf_p(const wchar_t *_Format,...);
  extern int  _vscwprintf_p(const wchar_t *_Format,va_list _ArgList);
  extern int  _wprintf_l(const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _wprintf_p_l(const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vwprintf_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vwprintf_p_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _fwprintf_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _fwprintf_p_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vfwprintf_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vfwprintf_p_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _swprintf_c_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _swprintf_p_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vswprintf_c_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _vswprintf_p_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _scwprintf_l(const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _scwprintf_p_l(const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vscwprintf_p_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _snwprintf_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _vsnwprintf_l(wchar_t *_DstBuf,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  __swprintf_l(wchar_t *_Dest,const wchar_t *_Format,_locale_t _Plocinfo,...);
  extern int  __vswprintf_l(wchar_t *_Dest,const wchar_t *_Format,_locale_t _Plocinfo,va_list _Args);
  extern int  _vscwprintf_l(const wchar_t *_Format,_locale_t _Locale,va_list _ArgList);
  extern int  _fwscanf_l(FILE *_File,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _swscanf_l(const wchar_t *_Src,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _snwscanf_l(const wchar_t *_Src,size_t _MaxCount,const wchar_t *_Format,_locale_t _Locale,...);
  extern int  _wscanf_l(const wchar_t *_Format,_locale_t _Locale,...);



  extern size_t  _fread_nolock_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);
# 428 "c:/programs/tcc/include/stdio.h" 2
# 31 "tests/pp-test.c" 2

int main(int argc, char* argv[])
{
	int typedef a;
	a f(void);
	a v(void);

	a (*pp[2])(void) = {
		f, v
	};
}
