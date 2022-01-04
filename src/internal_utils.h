#ifndef INTERNAL_UTILS_H
#define INTERNAL_UTILS_H

// NOTE(ljre): Functions prefixed with 'Our' are simple replacements of some CRT procedures
//             with (maybe) some differences.

#ifdef _MSC_VER
#   include <intrin.h>
#endif

// NOTE(ljre): This function *does* accept overlapping memory.
internal inline void*
OurMemCopy(void* restrict dst, const void* restrict src, uintsize size)
{
	Trace();
	
#if defined(__clang__) || defined(__GNUC__)
	void* d = dst;
	__asm__ __volatile__("rep movsb"
						 :"+D"(d), "+S"(src), "+c"(size)
						 :: "memory");
#elif defined(_MSC_VER)
	__movsb(dst, src, size);
#else
	uint8* d = dst;
	const uint8* s = src;
	while (size--)
		*d++ = *s++;
#endif
	
	return dst;
}

// NOTE(ljre): For overlapping memory where 'dst > src'.
internal inline void*
OurMemCopyReserved(void* dst, const void* src, uintsize size)
{
	uint8* d = dst;
	const uint8* s = src;
	
	d += size;
	s += size;
	
	while (size--)
		*--d = *--s;
	
	return dst;
}

internal inline void*
OurMemSet(void* restrict dst, uint8 byte, uintsize size)
{
#if defined(__clang__) || defined(__GNUC__)
	void* d = dst;
	__asm__ __volatile__("rep stosb"
						 :"+D"(d), "+a"(byte), "+c"(size)
						 :: "memory");
#elif defined(_MSC_VER)
	__stosb(dst, byte, size);
#else
	uint8* d = dst;
	while (size--)
		*d++ = byte;
#endif
	
	return dst;
}

internal inline uintsize
OurStrLen(const char* restrict str)
{
	uintsize result = 0;
	
	while (*str++)
		++result;
	
	return result;
}

struct StringList typedef StringList;
struct StringList
{
	StringList* next;
	String value;
};

internal uint64
SimpleHash(String str)
{
	uint64 hash = 2166136261ull;
	uintsize i = 0;
	
	while (i < str.size && str.data[i]) {
		uint8 value = (uint64)str.data[i++];
		
		hash ^= value;
		hash *= 16777619ull;
	}
	
	return hash;
}

internal uint64
SimpleHashNullTerminated(const char* str)
{
	uint64 hash = 2166136261ull;
	
	while (*str) {
		uint8 value = *str++;
		
		hash ^= value;
		hash *= 16777619ull;
	}
	
	return hash;
}

internal const char*
NullTerminateString(String str)
{
	if (str.size == 0)
		return "";
	
	if (str.data[str.size-1] == 0)
		return str.data;
	
	char* result = PushMemory(str.size+1);
	OurMemCopy(result, str.data, str.size);
	result[str.size] = 0;
	
	return result;
}

internal String
SliceString(String str, uintsize offset)
{
	str.data += offset;
	str.size -= offset;
	
	return str;
}

internal String
IgnoreNullTerminator(String str)
{
	while (str.size > 0 && !str.data[str.size-1])
		--str.size;
	
	return str;
}

internal bool32
MatchCString(const char* a, String cmp)
{
	while (cmp.size --> 0)
	{
		if (*a++ != *cmp.data++)
			return false;
	}
	
	return !(*a);
}

internal inline int32
CompareStringFast(String a, String b)
{
	if (a.size != b.size)
		return (int32)a.size - (int32)b.size;
	
	return memcmp(a.data, b.data, a.size);
}

internal int32
CompareString(String a, String b)
{
	a = IgnoreNullTerminator(a);
	b = IgnoreNullTerminator(b);
	
	if (a.size != b.size)
		return (int32)a.size - (int32)b.size;
	
	int32 n = (int32)a.size;
	const uint8* s1 = (const uint8*)a.data;
	const uint8* s2 = (const uint8*)b.data;
	
	while (n > 0 && *s1 && *s1 == *s2)
	{
		++s1;
		++s2;
		--n;
	}
	
	if (n == 0)
		return 0;
	
	return (int32)*s1 - (int32)*s2;
}

internal bool32
StringStartsWith(String str, const char* prefix)
{
	const char* head = str.data;
	const char* end = head + str.size;
	
	while (*prefix && head < end && *head == *prefix)
	{
		++head;
		++prefix;
	}
	
	return !*prefix;
}

internal uintsize
OurStrCopy_(char* restrict buf, const char* restrict from, uintsize max)
{
	char* old_buf = buf;
	
	while (*from && max --> 0)
		*buf++ = *from++;
	
	return buf - old_buf;
}

// NOTE(ljre): A 'printf' replacement with simple custom features.
//             IMPORTANT: use 'OurPrintfSize' to get the needed size for the buffer.
//             IMPORTANT: these functions does *not* include the null-terminator at the end of the buffer.
//
//             %CN (uint32) sets the output color to 'global_colors[N]' if 'global_colors != NULL'
//             %S  (uintsize, const char*) same as printf's %.*s
//             %s  (const char*)
//             %u  (uint32)
//             %i  (int32)
//             %U  (uint64)
//             %I  (int64)
//             %z  (uintsize)
//             %f  (double)
//             %c  (char)
//
//             Returns how many bytes where written to 'buf'.
internal uintsize
OurVPrintf(char* buf, uintsize len, const char* fmt, va_list args)
{
	char* outhead = buf;
	char* outend = buf + len;
	
	for (; outhead < outend && *fmt; ++fmt)
	{
		if (*fmt != '%')
		{
			*outhead++ = *fmt;
			continue;
		}
		
		++fmt;
		switch(*fmt)
		{
			case 'C':
			{
				if (*++fmt)
				{
					uint32 color = *fmt - '0';
					if (global_colors)
						outhead += OurStrCopy_(outhead, global_colors[color], outend - outhead);
				}
			} break;
			
			case 'c':
			{
				char c = va_arg(args, int);
				*outhead++ = c;
			} break;
			
			case 'S':
			{
				uintsize len = va_arg(args, uintsize);
				const char* ptr = va_arg(args, const char*);
				
				len = Min(len, outend - outhead);
				outhead += OurStrCopy_(outhead, ptr, len);
			} break;
			
			case 's':
			{
				const char* ptr = va_arg(args, const char*);
				outhead += OurStrCopy_(outhead, ptr, outend - outhead);
			} break;
			
			case 'u':
			{
				uint32 n = va_arg(args, uint32);
				outhead += snprintf(outhead, outend - outhead, "%u", n);
			} break;
			
			case 'i':
			{
				int32 n = va_arg(args, int32);
				outhead += snprintf(outhead, outend - outhead, "%i", n);
			} break;
			
			case 'U':
			{
				uint64 n = va_arg(args, uint64);
				outhead += snprintf(outhead, outend - outhead, "%llu", n);
			} break;
			
			case 'I':
			{
				int64 n = va_arg(args, int64);
				outhead += snprintf(outhead, outend - outhead, "%lli", n);
			} break;
			
			case 'z':
			{
				uintsize n = va_arg(args, uintsize);
				outhead += snprintf(outhead, outend - outhead, "%zu", n);
			} break;
			
			case 'f':
			{
				double n = va_arg(args, double);
				outhead += snprintf(outhead, outend - outhead, "%f", n);
			} break;
			
			case 0: goto out_of_the_loop;
			
			default:
			{
				*outhead++ = *fmt;
			} break;
		}
	}
	
	out_of_the_loop:;
	
	return outhead - buf;
}

internal uintsize
OurPrintf(char* buf, uintsize len, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	uintsize result = OurVPrintf(buf, len, fmt, args);
	va_end(args);
	return result;
}

// NOTE(ljre): This function will return the maximum size OurPrintf is going to need, not *exactly* the same size.
internal uintsize
OurVPrintfSize(const char* fmt, va_list args)
{
	uintsize result = 0;
	for (; *fmt; ++fmt)
	{
		if (*fmt != '%')
		{
			++result;
			continue;
		}
		
		++fmt;
		switch(*fmt)
		{
			case 'C':
			{
				uint32 color = *++fmt - '0';
				if (global_colors)
					result += OurStrLen(global_colors[color]);
			} break;
			
			case 'c':
			{
				va_arg(args, int);
				++result;
			} break;
			
			case 'S':
			{
				uintsize len = va_arg(args, uintsize);
				const char* ptr = va_arg(args, const char*);
				
				result += len;
				(void)ptr;
			} break;
			
			case 's':
			{
				const char* ptr = va_arg(args, const char*);
				result += OurStrLen(ptr);
			} break;
			
			case 'u':
			{
				uint32 n = va_arg(args, uint32);
				result += snprintf(NULL, 0, "%u", n);
			} break;
			
			case 'i':
			{
				int32 n = va_arg(args, int32);
				result += snprintf(NULL, 0, "%i", n);
			} break;
			
			case 'U':
			{
				uint64 n = va_arg(args, uint64);
				result += snprintf(NULL, 0, "%llu", n);
			} break;
			
			case 'I':
			{
				int64 n = va_arg(args, int64);
				result += snprintf(NULL, 0, "%lli", n);
			} break;
			
			case 'z':
			{
				uintsize n = va_arg(args, uintsize);
				result += snprintf(NULL, 0, "%zu", n);
			} break;
			
			case 'f':
			{
				double n = va_arg(args, double);
				result += snprintf(NULL, 0, "%f", n);
			} break;
			
			default:
			{
				result += 1;
			} break;
		}
	}
	
	return result;
}

internal uintsize
OurPrintfSize(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	uintsize result = OurVPrintfSize(fmt, args);
	va_end(args);
	return result;
}

internal double
OurStrToDouble(const char* str, const char** out_end)
{
	// TODO(ljre)
	return 0.0;
}

// TODO(ljre): Check if this is correct
internal uint64
OurStrToU64(const char* str, const char** out_end, uint32 base)
{
	const uint8* buf = (const uint8*)str;
	const uint8 case_mask = 0b11011111;
	
	uint64 result = 0;
	
	if (base > 10)
	{
		uint8 c;
		uint8 max = 'A' + base-11;
		
		for (; c = *buf&case_mask, *buf; ++buf)
		{
			uint8 r;
			if (c >= 'A' && c <= max)
				r = c - 'A';
			else if (c >= '0' && c <= '9')
				r = c - '0';
			else
				break;
			result += r;
			result *= base;
		}
	}
	else
	{
		uint8 max = '0' + base-1;
		
		for (; *buf >= '0' && *buf <= max; ++buf)
		{
			result *= base;
			result += *buf - '0';
		}
	}
	
	if (out_end)
		*out_end = (const char*)buf;
	
	return result;
}

#endif //INTERNAL_UTILS_H
