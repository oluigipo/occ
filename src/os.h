#ifndef OS_H
#define OS_H

#if defined(_WIN32)
#   define MAX_PATH_SIZE 32767
#elif defined(__linux__)
#   define MAX_PATH_SIZE (PATH_MAX+1)
#endif

typedef void* OS_Thread;
typedef void* OS_Mutex;

internal String OS_GetMyPath(void);
internal const char* OS_ReadWholeFile(const char* path, uintsize* size);
internal bool32 OS_WriteWholeFile(const char* path, const void* data, uintsize size);
internal void OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE]);
internal void* OS_ReserveMemory(uintsize size);
internal void* OS_CommitMemory(void* ptr, uintsize size);
internal void OS_FreeMemory(void* ptr, uintsize size);

internal OS_Thread OS_CreateThread(int32 func(void* user_data), void* user_data);
internal void OS_ExitThread(OS_Thread thrd, int32 code);
internal void OS_JoinThread(OS_Thread thrd);
internal bool32 OS_JoinableThread(OS_Thread thrd);
internal void OS_DestroyThread(OS_Thread thrd);

internal OS_Mutex OS_CreateMutex(void);
internal void OS_LockMutexRead(OS_Mutex mtx);
internal void OS_LockMutexWrite(OS_Mutex mtx);
internal bool32 OS_TryLockMutexRead(OS_Mutex mtx);
internal bool32 OS_TryLockMutexWrite(OS_Mutex mtx);
internal void OS_UnlockMutexRead(OS_Mutex mtx);
internal void OS_UnlockMutexWrite(OS_Mutex mtx);
internal void OS_DestroyMutex(OS_Mutex mtx);

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
OS_ReadWholeFile(const char* path, uintsize* out_size)
{
	TraceName(StrFrom(path));
	wchar_t wpath[MAX_PATH_SIZE];
	int32 wpath_len = MultiByteToWideChar(CP_UTF8, 0, path, strlen(path), wpath, ArrayLength(wpath));
	if (wpath_len <= 0 || wpath_len >= MAX_PATH_SIZE)
		return NULL;
	
	wpath[wpath_len] = 0;
	
	HANDLE file = CreateFileW(wpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return NULL;
	
	LARGE_INTEGER large_int;
	if (!GetFileSizeEx(file, &large_int))
	{
		CloseHandle(file);
		return NULL;
	}
	
	uintsize file_size = large_int.QuadPart;
	char* file_data = PushMemory(file_size + 1);
	
	uintsize still_to_read = file_size;
	char* p = file_data;
	while (still_to_read > 0)
	{
		DWORD to_read = (DWORD)Min(still_to_read, UINT32_MAX);
		DWORD did_read;
		
		if (!ReadFile(file, p, to_read, &did_read, NULL) ||
			did_read != to_read)
		{
			CloseHandle(file);
			global_arena->offset -= file_size + 1;
			return NULL;
		}
		
		still_to_read -= to_read;
		p += to_read;
	}
	
	CloseHandle(file);
	
	p[0] = 0; // null terminated
	
	if (out_size)
		*out_size = file_size;
	return file_data;
}

internal bool32
OS_WriteWholeFile(const char* path, const void* data, uintsize size)
{
	TraceName(StrFrom(path));
	
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
	wchar_t wpath[MAX_PATH_SIZE];
	wchar_t fullpath[MAX_PATH_SIZE];
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

internal OS_Thread
OS_CreateThread(int32 func(void* user_data), void* user_data)
{
	return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, user_data, 0, NULL);
}

internal void
OS_ExitThread(OS_Thread thrd, int32 code)
{
	TerminateThread(thrd, (DWORD)code);
}

internal void
OS_JoinThread(OS_Thread thrd)
{
	WaitForSingleObject(thrd, INFINITE);
}

internal bool32
OS_JoinableThread(OS_Thread thrd)
{
	DWORD code;
	
	if (GetExitCodeThread(thrd, &code))
	{
		return code != STILL_ACTIVE;
	}
	
	return true;
}

internal void
OS_DestroyThread(OS_Thread thrd)
{
	TerminateThread(thrd, 1);
	CloseHandle(thrd);
}

internal OS_Mutex
OS_CreateMutex(void)
{
	SRWLOCK* mtx = HeapAlloc(GetProcessHeap(), 0, sizeof *mtx);
	InitializeSRWLock(mtx);
	return mtx;
}

internal void
OS_LockMutexRead(OS_Mutex mtx)
{ AcquireSRWLockShared(mtx); }

internal void
OS_LockMutexWrite(OS_Mutex mtx)
{ AcquireSRWLockExclusive(mtx); }

internal bool32
OS_TryLockMutexRead(OS_Mutex mtx)
{ return TryAcquireSRWLockShared(mtx); }

internal bool32
OS_TryLockMutexWrite(OS_Mutex mtx)
{ return TryAcquireSRWLockExclusive(mtx); }

internal void
OS_UnlockMutexRead(OS_Mutex mtx)
{ ReleaseSRWLockShared(mtx); }

internal void
OS_UnlockMutexWrite(OS_Mutex mtx)
{ ReleaseSRWLockExclusive(mtx); }

internal void
OS_DestroyMutex(OS_Mutex mtx)
{ HeapFree(GetProcessHeap(), 0, mtx); }

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
