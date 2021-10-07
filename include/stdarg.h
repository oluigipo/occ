#ifndef __STDARG_H
#define __STDARG_H

#define va_list __builtin_va_list
#define va_arg(list, T) __builtin_va_arg(list, T)
#define va_copy(dst, src) __builtin_va_copy(dst, src)
#define va_end(list) __builtin_va_end(list)
#define va_start(list, p) __builtin_va_start(list, p)

#endif //__STDARG_H
