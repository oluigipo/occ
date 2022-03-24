internal bool
C_IsWarningEnabled(C_Context* ctx, C_Warning warning)
{
	const uint32 bit_n_64 = 6;
	
	uint32 index = warning >> bit_n_64;
	uint32 mask = warning & (1 << bit_n_64)-1;
	
	Assert(index < ArrayLength(ctx->options->warnings));
	
	return (ctx->options->warnings[index] & mask) != 0;
}

internal void
C_PrintFileTrace(C_Context* ctx, Arena* arena, C_SourceFileTrace* file, uint32 included_line)
{
	if (file->included_from)
		C_PrintFileTrace(ctx, arena, file->included_from, file->included_line);
	
	Arena_Printf(arena, "%C1%S%C0(%u): in included file\n", StrFmt(file->path), included_line + 1);
}

internal void
C_TraceErrorVarArgs(C_Context* ctx, C_SourceTrace* trace, const char* fmt, va_list args)
{
	++ctx->error_count;
	Arena* arena = ctx->scratch_arena;
	char* msg = Arena_End(arena);
	
	{
		if (trace->file->included_from)
			C_PrintFileTrace(ctx, arena, trace->file->included_from, trace->file->included_line);
		
		Arena_Printf(arena, "%C1%S%C0(%u:%u): %C2error%C0: ", StrFmt(trace->file->path), trace->line + 1, trace->col + 1);
		Arena_VPrintf(arena, fmt, args);
		Arena_PushMemory(arena, 1, "\n");
		
		while (trace->invocation)
		{
			trace = trace->invocation;
			Arena_Printf(arena, "expanded from %C1%S%C0(%u:%u)\n", StrFmt(trace->file->path), trace->line + 1, trace->col + 1);
		}
		
		Arena_PushMemory(arena, 2, "\n");
		
		PrintFast(msg);
	}
	
	Arena_Pop(arena, msg);
}

internal void
C_TraceWarningVarArgs(C_Context* ctx, C_SourceTrace* trace, C_Warning warning, const char* fmt, va_list args)
{
	if (!C_IsWarningEnabled(ctx, warning))
		return;
	
	++ctx->warning_count;
	Arena* arena = ctx->scratch_arena;
	char* msg = Arena_End(arena);
	
	{
		if (trace->file->included_from)
			C_PrintFileTrace(ctx, arena, trace->file->included_from, trace->file->included_line);
		
		Arena_Printf(arena, "%C1%S%C0(%u:%u): %C3warning%C0: ", StrFmt(trace->file->path), trace->line + 1, trace->col + 1);
		Arena_VPrintf(arena, fmt, args);
		Arena_PushMemory(arena, 1, "\n");
		
		while (trace->invocation)
		{
			trace = trace->invocation;
			Arena_Printf(arena, "expanded by %C1%S%C0(%u:%u)\n", StrFmt(trace->file->path), trace->line + 1, trace->col + 1);
		}
		
		Arena_PushMemory(arena, 2, "\n");
		
		PrintFast(msg);
	}
	
	Arena_Pop(arena, msg);
}

internal inline void
C_TraceError(C_Context* ctx, C_SourceTrace* trace, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	C_TraceErrorVarArgs(ctx, trace, fmt, args);
	va_end(args);
}

internal inline void
C_TraceWarning(C_Context* ctx, C_SourceTrace* trace, C_Warning warning, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	C_TraceWarningVarArgs(ctx, trace, warning, fmt, args);
	va_end(args);
}

internal inline void
C_TraceErrorRd(C_Context* ctx, C_TokenReader* rd, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	C_SourceTrace trace = rd->head->trace;
	
	if (rd->file_trace)
		trace.file = rd->file_trace;
	
	C_TraceErrorVarArgs(ctx, &trace, fmt, args);
	
	va_end(args);
}

internal inline void
C_TraceWarningRd(C_Context* ctx, C_TokenReader* rd, C_Warning warning, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	C_SourceTrace trace = rd->head->trace;
	
	if (rd->file_trace)
		trace.file = rd->file_trace;
	
	C_TraceWarningVarArgs(ctx, &trace, warning, fmt, args);
	
	va_end(args);
}
