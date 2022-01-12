#ifndef OS_H
#define OS_H

#if defined(_WIN32)
#   define MAX_PATH_SIZE 32767
#elif defined(__linux__)
#   define MAX_PATH_SIZE (4096+1)
#endif

#define OS_ScopedLockWrite(lock) for (int32 il__ = (OS_LockRWLockWrite(lock), 0); !il__; ++il__, OS_UnlockRWLockWrite(lock))
#define OS_ScopedLockRead(lock)  for (int32 il__ = (OS_LockRWLockRead(lock), 0); !il__; ++il__, OS_UnlockRWLockRead(lock))

typedef void* OS_Thread;
typedef void* OS_RWLock;

struct OS_Info
{
	uint32 num_threads;
}
typedef OS_Info;

internal void OS_Exit(int32 code);

internal uint64 OS_Time(void);
internal uintsize OS_GetMyPath(Arena* arena);
internal uintsize OS_ReadWholeFile(const char* path, Arena* arena);
internal bool32 OS_WriteWholeFile(const char* path, const void* data, uintsize size, Arena* scratch_arena);
internal void OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE], Arena* scratch_arena);
internal void* OS_ReserveMemory(uintsize size);
internal void* OS_CommitMemory(void* ptr, uintsize size);
internal void OS_FreeMemory(void* ptr, uintsize size);
internal bool32 OS_GetInfo(OS_Info* out_info);

internal OS_Thread OS_CreateThread(void func(void* user_data), void* user_data);
internal void OS_ExitThisThread(int32 result);
internal int32 OS_JoinThread(OS_Thread thrd);
internal bool32 OS_JoinableThread(OS_Thread thrd);
internal void OS_SleepThisThread(uint32 ms);

internal OS_RWLock OS_CreateRWLock(void);
internal void OS_LockRWLockRead(OS_RWLock lock);
internal void OS_LockRWLockWrite(OS_RWLock lock);
internal bool32 OS_TryLockRWLockRead(OS_RWLock lock);
internal bool32 OS_TryLockRWLockWrite(OS_RWLock lock);
internal void OS_UnlockRWLockRead(OS_RWLock lock);
internal void OS_UnlockRWLockWrite(OS_RWLock lock);
internal void OS_DestroyRWLock(OS_RWLock lock);

#endif //OS_H

#ifdef OS_H_IMPLEMENTATION
#if defined(_WIN32)

#if 1
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#else
#   ifndef WINAPI
#       define WINAPI __stdcall
#   endif

#define CP_UTF8 65001
#define FILE_SHARE_READ 0x00000001
#define GENERIC_READ (1u << 31)
#define OPEN_EXISTING 3
#define FILE_ATTRIBUTE_READONLY 1
#define INVALID_HANDLE_VALUE NULL
#define MEM_RESERVE 0x00002000
#define MEM_COMMIT 0x00001000
#define PAGE_READWRITE 0x04
#define MEM_RELEASE 0x00008000
#define INFINITE (-1)
#define STILL_ACTIVE 259

typedef uint32 UINT;
typedef uint16 WORD;
typedef uint32 DWORD;
typedef unsigned long ULONG;
typedef ULONG* ULONG_PTR;
typedef unsigned long long ULONGLONG;
typedef long LONG;
typedef long long LONGLONG;
typedef DWORD* LPDWORD;
typedef int BOOL;
typedef BOOL* LPBOOL;
typedef void* HMODULE;
typedef char* LPSTR;
typedef wchar_t* LPWSTR;
typedef const wchar_t* LPCWSTR;
typedef const char* LPCCH;
typedef const wchar_t* LPCWCH;
typedef void* HANDLE;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef void* PVOID;
typedef uintsize SIZE_T;
typedef DWORD WINAPI THREAD_START_ROUTINE(LPVOID lpParameter);
typedef THREAD_START_ROUTINE* LPTHREAD_START_ROUTINE;

typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _FILETIME {
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef union _ULARGE_INTEGER {
	struct {
		DWORD LowPart;
		DWORD HighPart;
	} DUMMYSTRUCTNAME;
	struct {
		DWORD LowPart;
		DWORD HighPart;
	} u;
	ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef union _LARGE_INTEGER {
	struct {
		DWORD LowPart;
		LONG  HighPart;
	} DUMMYSTRUCTNAME;
	struct {
		DWORD LowPart;
		LONG  HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _SECURITY_ATTRIBUTES {
	DWORD  nLength;
	LPVOID lpSecurityDescriptor;
	BOOL   bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _OVERLAPPED {
	ULONG_PTR Internal;
	ULONG_PTR InternalHigh;
	union {
		struct {
			DWORD Offset;
			DWORD OffsetHigh;
		} DUMMYSTRUCTNAME;
		PVOID Pointer;
	} DUMMYUNIONNAME;
	HANDLE    hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _SRWLOCK {
	void* Ptr;
} SRWLOCK, *PSRWLOCK, *LPSRWLOCK;

void WINAPI ExitProcess(uint32 code);
void WINAPI GetSystemTime(LPSYSTEMTIME lpSystemTime);
BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME* lpSystemTime, LPFILETIME lpFileTime);
DWORD WINAPI GetModuleFileNameA(HMODULE hModule, LPSTR lpFilename, DWORD nSize);
DWORD WINAPI GetModuleFileNameW(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
int WINAPI MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
HANDLE WINAPI CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
BOOL WINAPI GetFileSizeEx(HANDLE hFile, PLARGE_INTEGER lpFileSize);
BOOL WINAPI CloseHandle(HANDLE hObject);
BOOL WINAPI ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
BOOL WINAPI WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
					  LPOVERLAPPED lpOverlapped);
DWORD WINAPI GetFullPathNameW(LPCWSTR lpFileName, DWORD nBufferLength, LPWSTR lpBuffer, LPWSTR* lpFilePart);
int WINAPI WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar);
LPVOID WINAPI VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD  flAllocationType, DWORD  flProtect);
BOOL WINAPI VirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType);
HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
						   SIZE_T dwStackSize,
						   LPTHREAD_START_ROUTINE lpStartAddress,
						   LPVOID lpParameter,
						   DWORD dwCreationFlags,
						   LPDWORD lpThreadId);
void WINAPI ExitThread(DWORD dwExitCode);
BOOL WINAPI GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode);
DWORD WINAPI WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds);
void WINAPI InitializeSRWLock(PSRWLOCK SRWLock);
LPVOID WINAPI HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
HANDLE WINAPI GetProcessHeap(void);
void WINAPI AcquireSRWLockShared(PSRWLOCK SRWLock);
void WINAPI AcquireSRWLockExclusive(PSRWLOCK SRWLock);
BOOL WINAPI TryAcquireSRWLockShared(PSRWLOCK SRWLock);
BOOL WINAPI TryAcquireSRWLockExclusive(PSRWLOCK SRWLock);
void WINAPI ReleaseSRWLockShared(PSRWLOCK SRWLock);
void WINAPI ReleaseSRWLockExclusive(PSRWLOCK SRWLock);
BOOL WINAPI HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
#endif

#include <stdio.h>

internal void
OS_Exit(int32 code)
{ ExitProcess(code); }

internal uint64
OS_Time(void)
{
	// NOTE(ljre): *sigh*
	// https://docs.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-rtltimetosecondssince1970
	union
	{
		FILETIME filetime;
		ULARGE_INTEGER largeint;
	} now, then;
	
	SYSTEMTIME systime_now;
	SYSTEMTIME systime_then = {
		.wYear = 1970,
		.wMonth = 1,
		.wDayOfWeek = 4,
		.wDay = 1,
		// rest is 0
	};
	
	GetSystemTime(&systime_now);
	SystemTimeToFileTime(&systime_now, &now.filetime);
	SystemTimeToFileTime(&systime_then, &then.filetime);
	
	uint64 result = now.largeint.QuadPart - then.largeint.QuadPart;
	result /= 10000000;
	
	return result;
}

internal uintsize
OS_GetMyPath(Arena* arena)
{
	char* arena_end = Arena_End(arena);
	
	wchar_t* wpath = Arena_PushDirtyAligned(arena, MAX_PATH_SIZE * sizeof(*wpath), 2);
	DWORD wpath_len = GetModuleFileNameW(NULL, wpath, MAX_PATH_SIZE);
	if (wpath_len == 0 || wpath_len == MAX_PATH_SIZE)
	{
		Arena_Pop(arena, wpath);
		return 0;
	}
	
	int32 path_len = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, NULL, 0, NULL, NULL);
	if (path_len == 0)
		return 0;
	
	char* path = Arena_PushDirtyAligned(arena, path_len, 1);
	WideCharToMultiByte(CP_UTF8, 0, wpath, -1, path, path_len, NULL, NULL);
	
	OurMemCopy(arena_end, path, path_len);
	Arena_Pop(arena, arena_end + path_len);
	
	path = arena_end;
	
	if (path_len == 0)
	{
		Arena_Pop(arena, arena_end);
		return 0;
	}
	
	for (int32 i = 0; i < path_len-1; ++i)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}
	
	return path_len;
}

internal uintsize
OS_ReadWholeFile(const char* path, Arena* arena)
{
	TraceName(StrFrom(path));
	char* arena_end = Arena_End(arena);
	
	int32 wpath_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	if (wpath_len <= 0)
		return 0;
	
	wchar_t* wpath = Arena_PushDirtyAligned(arena, wpath_len * sizeof(*wpath), 2);
	MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, wpath_len);
	
	HANDLE file = CreateFileW(wpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return 0;
	
	LARGE_INTEGER large_int;
	if (!GetFileSizeEx(file, &large_int))
	{
		CloseHandle(file);
		Arena_Pop(arena, arena_end);
		return 0;
	}
	
	uintsize file_size = large_int.QuadPart;
	char* file_data = Arena_PushDirtyAligned(arena, file_size + 1, 1);
	
	uintsize still_to_read = file_size;
	char* p = file_data;
	while (still_to_read > 0)
	{
		DWORD to_read = (DWORD)Min(still_to_read, UINT32_MAX);
		DWORD did_read;
		
		if (!ReadFile(file, p, to_read, &did_read, NULL))
		{
			CloseHandle(file);
			Arena_Pop(arena, arena_end);
			return 0;
		}
		
		still_to_read -= to_read;
		p += to_read;
	}
	
	CloseHandle(file);
	
	p[0] = 0; // null terminated
	
	OurMemCopy(arena_end, file_data, file_size + 1);
	Arena_Pop(arena, arena_end + file_size + 1);
	
	return file_size;
}

internal bool32
OS_WriteWholeFile(const char* path, const void* data, uintsize size, Arena* scratch_arena)
{
	TraceName(StrFrom(path));
	char* arena_end = Arena_End(scratch_arena);
	
	int32 wpath_len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	if (wpath_len <= 0)
		return false;
	
	wchar_t* wpath = Arena_PushDirtyAligned(scratch_arena, wpath_len * sizeof(*wpath), 2);
	MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, wpath_len);
	
	bool32 result = true;
	DWORD bytes_written = 0;
	
	HANDLE file = CreateFileW(wpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (file == INVALID_HANDLE_VALUE)
		result = false;
	else
	{
		while (size > 0)
		{
			DWORD to_write = Min(size, UINT32_MAX);
			if (!WriteFile(file, data, to_write, &bytes_written, NULL))
			{
				result = false;
				break;
			}
			
			size -= bytes_written;
			data = (uint8*)data + bytes_written;
		}
		
		CloseHandle(file);
	}
	
	Arena_Pop(scratch_arena, arena_end);
	return result;
}

internal void
OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE], Arena* scratch_arena)
{
	TraceName(path);
	
	char* arena_end = Arena_End(scratch_arena);
	
	wchar_t* wpath;
	wchar_t* wfullpath;
	int32 len;
	path = IgnoreNullTerminator(path);
	
	len = MultiByteToWideChar(CP_UTF8, 0, path.data, path.size, NULL, 0);
	if (len == 0)
		return;
	++len;
	
	wpath = Arena_PushDirtyAligned(scratch_arena, len * sizeof(*wpath), 2);
	MultiByteToWideChar(CP_UTF8, 0, path.data, path.size, wpath, len);
	wpath[len-1] = 0;
	
	wfullpath = Arena_PushDirtyAligned(scratch_arena, MAX_PATH_SIZE * sizeof(*wfullpath), 2);
	GetFullPathNameW(wpath, MAX_PATH_SIZE, wfullpath, NULL);
	len = WideCharToMultiByte(CP_UTF8, 0, wfullpath, -1, out_buf, MAX_PATH_SIZE, NULL, NULL);
	
	for (char* it = out_buf; *it; ++it)
	{
		if (*it == '\\')
			*it = '/';
	}
	
	Arena_Pop(scratch_arena, arena_end);
}

internal void*
OS_ReserveMemory(uintsize size)
{ return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE); }

internal void*
OS_CommitMemory(void* ptr, uintsize size)
{ return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE); }

internal void
OS_FreeMemory(void* ptr, uintsize size)
{ VirtualFree(ptr, 0, MEM_RELEASE); }

internal OS_Thread
OS_CreateThread(void func(void* user_data), void* user_data)
{ return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, user_data, 0, NULL); }

internal void
OS_ExitThisThread(int32 result)
{ ExitThread((DWORD)result); }

internal void
OS_SleepThisThread(uint32 ms)
{ Sleep(ms); }

internal int32
OS_JoinThread(OS_Thread thrd)
{
	Trace();
	
	WaitForSingleObject(thrd, INFINITE);
	
	DWORD code;
	if (!GetExitCodeThread(thrd, &code))
		code = -1;
	
	CloseHandle(thrd);
	return (int32)code;
}

internal bool32
OS_JoinableThread(OS_Thread thrd)
{
	DWORD code;
	
	if (GetExitCodeThread(thrd, &code))
		return code != STILL_ACTIVE;
	
	return true;
}

internal OS_RWLock
OS_CreateRWLock(void)
{
	SRWLOCK* lock = HeapAlloc(GetProcessHeap(), 0, sizeof(*lock));
	InitializeSRWLock(lock);
	return lock;
}

internal void
OS_LockRWLockRead(OS_RWLock lock)
{ AcquireSRWLockShared(lock); }

internal void
OS_LockRWLockWrite(OS_RWLock lock)
{ AcquireSRWLockExclusive(lock); }

internal bool32
OS_TryLockRWLockRead(OS_RWLock lock)
{ return TryAcquireSRWLockShared(lock); }

internal bool32
OS_TryLockRWLockWrite(OS_RWLock lock)
{ return TryAcquireSRWLockExclusive(lock); }

internal void
OS_UnlockRWLockRead(OS_RWLock lock)
{ ReleaseSRWLockShared(lock); }

internal void
OS_UnlockRWLockWrite(OS_RWLock lock)
{ ReleaseSRWLockExclusive(lock); }

internal void
OS_DestroyRWLock(OS_RWLock lock)
{ HeapFree(GetProcessHeap(), 0, lock); }

#elif defined(__linux__) //_WIN32
#include <sys/mman.h>
#include <pthread.h>
#include <stdio.h>
#include <libgen.h>       // dirname
#include <linux/limits.h> // PATH_MAX
#include <unistd.h>       // readlink

internal void
OS_Exit(int32 code)
{ exit(code); }

internal uintsize
OS_GetMyPath(Arena* arena)
{
	char* link = Arena_PushDirtyAligned(arena, MAX_PATH_SIZE, 1);
	ssize_t count = readlink("/proc/self/exe", link, MAX_PATH_SIZE);
	
	const char* result;
	uintsize length;
	
	if (count != -1)
	{
		link[count] = 0;
		result = dirname(link);
		length = OurStrLen(result) + 1;
	}
	else
	{
		result = "/usr/";
		length = sizeof("/usr/");
	}
	
	Arena_Pop(arena, result + length);
	return length;
}

internal uintsize
OS_ReadWholeFile(const char* path, Arena* arena)
{
	TraceName(StrFrom(path));
	
	FILE* file = fopen(path, "rb");
	if (!file)
		return NULL;
	
	fseek(file, 0, SEEK_END);
	uintsize size = ftell(file);
	rewind(file);
	
	char* data = Arena_PushDirtyAligned(arena, size+1, 1);
	size = fread(data, 1, size, file);
	data[size] = 0;
	
	fclose(file);
	return size;
}

internal bool32
OS_WriteWholeFile(const char* path, const void* data, uintsize size, Arena* scratch_arena)
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
OS_ResolveFullPath(String path, char out_buf[MAX_PATH_SIZE], Arena* scratch_arena)
{
	const char* cpath = Arena_NullTerminateString(scratch_arena, path);
	char* result = realpath(cpath, out_buf);
	
	if (!result)
		out_buf[0] = 0;
	if (Arena_Owns(scratch_arena, cpath))
		Arena_Pop(scratch_arena, cpath);
}

internal void*
OS_ReserveMemory(uintsize size)
{
	void* result = mmap(NULL, size, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (result == MAP_FAILED)
		result = NULL;
	
	return result;
}

internal void*
OS_CommitMemory(void* ptr, uintsize size)
{ return mprotect(ptr, size, PROT_READ | PROT_WRITE); }

internal void
OS_FreeMemory(void* ptr, uintsize size)
{ munmap(ptr, size); }

internal OS_Thread
OS_CreateThread(void func(void* user_data), void* user_data)
{
	pthread_t result;
	int32 err = pthread_create(&result, NULL, (void*)func, user_data);
	
	if (err)
		result = NULL;
	
	return result;
}

internal void
OS_ExitThisThread(int32 result)
{ pthread_exit((void*)result); }

internal int32
OS_JoinThread(OS_Thread thrd)
{
	void* result;
	if (pthread_join((pthread_t)thrd, &result))
		return -1;
	
	return (int32)result;
}

internal bool32
OS_JoinableThread(OS_Thread thrd)
{
	Assert(false);
}

internal OS_RWLock
OS_CreateRWLock(void)
{
	pthread_rwlock_t lock;
	
	if (!pthread_rwlock_init(&lock, NULL))
	{
		pthread_rwlock_t* result = malloc(sizeof(*result));
		*result = lock;
		return result;
	}
	
	return NULL;
}

internal void
OS_LockRWLockRead(OS_RWLock lock)
{ pthread_rwlock_rdlock(lock); }

internal void
OS_LockRWLockWrite(OS_RWLock lock)
{ pthread_rwlock_wrlock(lock); }

internal bool32
OS_TryLockRWLockRead(OS_RWLock lock)
{ return pthread_rwlock_tryrdlock(lock) == 0; }

internal bool32
OS_TryLockRWLockWrite(OS_RWLock lock)
{ return pthread_rwlock_trywrlock(lock) == 0; }

internal void
OS_UnlockRWLockRead(OS_RWLock lock)
{ pthread_rwlock_unlock(lock); }

internal void
OS_UnlockRWLockWrite(OS_RWLock lock)
{ pthread_rwlock_unlock(lock); }

internal void
OS_DestroyRWLock(OS_RWLock lock)
{
	pthread_rwlock_destroy(lock);
	free(lock);
}

#endif //__linux__

#endif //OS_H_IMPLEMENTATION
