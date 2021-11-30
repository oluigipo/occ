#include "backend_format_pe.h"

internal void
LangC_WritePEHeader(LangC_Context* ctx)
{
	BackFormat_PeCoffHeader* header = Arena_PushAligned(ctx->persistent_arena, sizeof *header, 1);
	
	header->machine = 0x8664;
	header->number_of_sections = 0;
	header->time_date_stamp = OS_Time();
	header->ptr_to_symbol_table = 0;
	header->number_of_symbols = 0;
	header->size_of_optional_header = 0;
	header->characteristics = 0x0020;
}

internal bool32
LangC_GenerateCode(LangC_Context* ctx, String output_file)
{
	Trace();
	
	char* buf = Arena_End(ctx->persistent_arena);
	LangC_WritePEHeader(ctx);
	
	OS_WriteWholeFile(Arena_NullTerminateString(ctx->stage_arena, output_file),
					  buf, (ctx->persistent_arena->memory + ctx->persistent_arena->offset) - (uint8*)buf);
	
	return LangC_error_count == 0;
}
