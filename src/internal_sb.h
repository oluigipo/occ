#ifndef INTERNAL_SB_H
#define INTERNAL_SB_H

#define SB__Header(buf) ((buf) ? (Arena*)(buf)-1 : 0)
#define SB__Offset(buf) SB__Header(buf)->offset
#define SB_Offset(buf) ((buf) ? SB__Header(buf)->offset : 0)

#define SB_Len(buf) ((buf) ? SB__Offset(buf) / sizeof *(buf) : 0)
#define SB_ReserveAtLeast(buf,s) SB__ReserveAtLeast((void**)&(buf), (s) * sizeof *(buf))
#define SB_ReserveMore(buf,s) SB__ReserveAtLeast((void**)&(buf), SB_Offset(buf) + (s) * sizeof *(buf))
#define SB_Push(buf,item) (SB_ReserveAtLeast(buf, SB_Len(buf)+1), (buf)[SB_Len(buf)] = (item), SB_AddLen(buf,1))
#define SB_End(buf) ((buf) + SB_Len(buf))
#define SB_AddLen(buf,s) (SB__Offset(buf) += (s)*sizeof*(buf))
#define SB_PushArray(buf,s,data) (SB_ReserveMore(buf, s),\
memcpy(SB_End(buf), data, (s)*sizeof*(buf)),\
SB_AddLen(buf, s))
#define SB_PushArrayConst(buf,data) (SB_ReserveMore(buf, ArrayLength(data)),\
memcpy(SB_End(buf), data, sizeof(data)),\
SB__Offset(buf) += sizeof(data))

internal void
SB__ReserveAtLeast(void** buf, uintsize desired)
{
	Arena* header;
	
	if (*buf)
	{
		header = *buf;
		header -= 1;
	}
	else
	{
		header = Arena_Create(Gigabytes(1));
	}
	
	while (desired > header->commited)
		Arena_Commit(header);
	
	*buf = header + 1;
}

internal uintsize
vbprintf(char** buf, const char* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);
	
	uintsize len = vsnprintf(NULL, 0, fmt, args_copy);
	SB_ReserveMore(*buf, len + 1);
	vsnprintf(SB_End(*buf), len + 1, fmt, args);
	SB_AddLen(*buf, len);
	
	va_end(args_copy);
	
	return len;
}

internal uintsize
bprintf(char** buf, const char* fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	uintsize len = vbprintf(buf, fmt, list);
	va_end(list);
	
	return len;
}

#endif //INTERNAL_SB_H
