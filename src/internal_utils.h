#ifndef INTERNAL_UTILS_H
#define INTERNAL_UTILS_H

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
	memcpy(result, str.data, str.size);
	result[str.size] = 0;
	
	return result;
}

internal String
IgnoreNullTerminator(String str)
{
	while (str.size > 0 && !str.data[str.size-1])
		--str.size;
	
	return str;
}

internal bool32
MatchCString(const char* a, const char* cmp, int32 cmp_len)
{
	while (cmp_len --> 0)
	{
		if (*a++ != *cmp++)
			return false;
	}
	
	if (*a)
		return false;
	
	return true;
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

internal uintsize
OurStrCopy_(char* buf, const char* from, uintsize max)
{
	uintsize res = 0;
	
	while (max-- > 0 && *from)
		*buf++ = *from++, ++res;
	
	return res;
}

internal uintsize
OurVPrintf(char* buf, uintsize len, const char* fmt, va_list args)
{
	char* outhead = buf;
	char* outend = buf + len;
	
	while (outhead < outend && *fmt)
	{
		if (*fmt != '%')
		{
			*outhead++ = *fmt++;
			continue;
		}
		
		++fmt;
		switch(*fmt)
		{
			case 'C':
			{
				uint32 color = *++fmt - '0';
				if (global_colors)
					outhead += OurStrCopy_(outhead, global_colors[color], outend - outhead);
				++fmt;
			} break;
			
			case 'S':
			{
				uintsize len = va_arg(args, uintsize);
				const char* ptr = va_arg(args, const char*);
				
				len = (len > outend - outhead) ? outend - outhead : len;
				outhead += OurStrCopy_(outhead, ptr, len);
				++fmt;
			} break;
			
			case 's':
			{
				const char* ptr = va_arg(args, const char*);
				outhead += OurStrCopy_(outhead, ptr, outend - outhead);
				++fmt;
			} break;
			
			case 'u':
			{
				uint32 n = va_arg(args, uint32);
				outhead += snprintf(outhead, outend - outhead, "%u", n);
				++fmt;
			} break;
			
			case 'i':
			{
				int32 n = va_arg(args, int32);
				outhead += snprintf(outhead, outend - outhead, "%i", n);
				++fmt;
			} break;
			
			case 'U':
			{
				uint64 n = va_arg(args, uint64);
				outhead += snprintf(outhead, outend - outhead, "%llu", n);
				++fmt;
			} break;
			
			case 'I':
			{
				int64 n = va_arg(args, int64);
				outhead += snprintf(outhead, outend - outhead, "%lli", n);
				++fmt;
			} break;
			
			case 'z':
			{
				uintsize n = va_arg(args, uintsize);
				outhead += snprintf(outhead, outend - outhead, "%zu", n);
				++fmt;
			} break;
			
			case 'f':
			{
				double n = va_arg(args, double);
				outhead += snprintf(outhead, outend - outhead, "%f", n);
				++fmt;
			} break;
			
			default:
			{
				*outhead++ = *fmt++;
			} break;
		}
	}
	
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
					result += strlen(global_colors[color]);
			} break;
			
			case 'S':
			{
				uintsize len = va_arg(args, uintsize);
				const char* ptr = va_arg(args, const char*);
				
				result += strnlen(ptr, len);
			} break;
			
			case 's':
			{
				const char* ptr = va_arg(args, const char*);
				result += strlen(ptr);
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

#endif //INTERNAL_UTILS_H
