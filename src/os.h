#ifndef OS_H
#define OS_H

internal const char* OS_GetMyPath(void);
internal const char* OS_ReadWholeFile(const char* path);
internal void* OS_ReserveMemory(uintsize size);
internal void* OS_CommitMemory(void* ptr, uintsize size);

#endif //OS_H

#ifdef OS_H_IMPLEMENTATION
#if defined(_WIN32)
#include <windows.h>
#include <stdio.h>

internal const char*
OS_GetMyPath(void)
{
	char* path = PushMemory(32767);
	
	DWORD len = GetModuleFileNameA(NULL, path, 32767);
	for (DWORD i = 0; i < len; ++i)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}
	
	return path;
}

internal const char*
OS_ReadWholeFile(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (!file)
		return NULL;
	
	fseek(file, 0, SEEK_END);
	uintsize size = ftell(file);
	rewind(file);
	
	char* data = PushMemory(size+1);
	size = fread(data, 1, size, file);
	data[size] = 0;
	
	fclose(file);
	return data;
}

internal void*
OS_ReserveMemory(uintsize size)
{
	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

internal void*
OS_CommitMemory(void* ptr, uintsize size)
{
	return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

#elif defined(__linux__) //_WIN32
#include <sys/mman.h>
#include <stdio.h>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

internal const char*
OS_GetMyPath(void)
{
	char link[PATH_MAX + 1];
	ssize_t count = readlink("/proc/self/exe", link, PATH_MAX);
	
	const char* result;
	if (count != -1)
	{
		link[count] = 0;
		result = dirname(link);
	}
	else
	{
		result = "/usr/";
	}
	
	return result;
}

internal const char*
OS_ReadWholeFile(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (!file)
		return NULL;
	
	fseek(file, 0, SEEK_END);
	uintsize size = ftell(file);
	rewind(file);
	
	char* data = PushMemory(size+1);
	size = fread(data, 1, size, file);
	data[size] = 0;
	
	fclose(file);
	return data;
}

internal void*
OS_ReserveMemory(uintsize size)
{
	return mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
}

internal void*
OS_CommitMemory(void* ptr, uintsize size)
{
	return ptr; // memory is commited automatically
}

#endif //__linux__

#endif //OS_H_IMPLEMENTATION
