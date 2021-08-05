#ifndef OS_H
#define OS_H

internal void OS_GetMyPath(char* out_path, uintsize max);
internal const char* OS_ReadWholeFile(const char* path);

#endif //OS_H

#ifdef OS_H_IMPLEMENTATION
#if defined(_WIN32)
#include <windows.h>
#include <stdio.h>

internal void
OS_GetMyPath(char* out_path, uintsize max)
{
	DWORD len = GetModuleFileNameA(NULL, out_path, max);
	for (DWORD i = 0; i < len; ++i)
	{
		if (out_path[i] == '\\')
			out_path[i] = '/';
	}
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

#elif defined(__linux__) //_WIN32

internal void
OS_GetMyPath(char* out_path, uintsize max)
{
	// TODO
}

#endif //__linux__

#endif //OS_H_IMPLEMENTATION
