#ifndef __ASSERT_H
#define __ASSERT_H

#ifndef NDEBUG

__cdecl_ void _assert(const char* message, const char* file, unsigned int line);
#define assert(x) ((void)((x) ? 0 : (_assert(#x, __FILE__, __LINE__), 0)))

#else //NDEBUG

#define assert(x) ((void)0)

#endif //NDEBUG

#endif //__ASSERT_H
