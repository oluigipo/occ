#ifndef OS_H
#define OS_H

#if defined(_WIN32)
#   define MAX_PATH_SIZE 32767
#elif defined(__linux__)
#   define MAX_PATH_SIZE (PATH_MAX+1)
#endif

internal String OS_GetMyPath(void);
internal const char* OS_ReadWholeFile(const char* path);
internal bool32 OS_WriteWholeFile(const char* path, const void* data, uintsize size);
internal void OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE]);
internal void* OS_ReserveMemory(uintsize size);
internal void* OS_CommitMemory(void* ptr, uintsize size);
internal void OS_FreeMemory(void* ptr, uintsize size);

#endif //OS_H

#ifdef OS_H_IMPLEMENTATION
#if defined(_WIN32)
#include <windows.h>
#include <stdio.h>

internal String
OS_GetMyPath(void)
{
	char* path = PushMemory(MAX_PATH_SIZE);
	
	DWORD len = GetModuleFileNameA(NULL, path, MAX_PATH_SIZE);
	for (DWORD i = 0; i < len; ++i)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}
	
	return StrMake(path, len + 1);
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

internal bool32
OS_WriteWholeFile(const char* path, const void* data, uintsize size)
{
	FILE* file = fopen(path, "wb");
	if (!file)
		return false;
	
	bool32 success = (fwrite(data, 1, size, file) == size);
	fclose(file);
	
	return success;
}

internal void
OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE])
{
	wchar_t wpath[32767];
	wchar_t fullpath[32767];
	int32 i;
	
	i = MultiByteToWideChar(CP_UTF8, 0, path.data, path.size, wpath, ArrayLength(wpath));
	wpath[i] = 0;
	
	GetFullPathNameW(wpath, ArrayLength(fullpath), fullpath, NULL);
	i = WideCharToMultiByte(CP_UTF8, 0, fullpath, -1, out_buf, MAX_PATH_SIZE, NULL, NULL);
	out_buf[i] = 0;
	
	for (char* it = out_buf; *it; ++it)
	{
		if (*it == '\\')
			*it = '/';
	}
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

internal void
OS_FreeMemory(void* ptr, uintsize size /* ignored */)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
}

#elif defined(__linux__) //_WIN32
#include <sys/mman.h>
#include <stdio.h>
#include <libgen.h>         // dirname
#include <unistd.h>         // readlink
#include <linux/limits.h>   // PATH_MAX

internal String
OS_GetMyPath(void)
{
	char link[MAX_PATH_SIZE];
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
	
	return StrFrom(result);
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

internal void
OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE])
{
	Assert(false);
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
