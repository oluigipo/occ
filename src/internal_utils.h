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

#endif //INTERNAL_UTILS_H
