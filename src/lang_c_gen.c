#include "backend_format_pe.h"

internal uint32
LangC_SymbolCount(LangC_Symbol* sym)
{
	uint32 result = 0;
	
	while (sym)
	{
		sym = sym->next;
		++result;
	}
	
	return result;
}

internal bool32
LangC_GenIr(LangC_Context* ctx)
{
	Trace();
	
	LangC_SymbolStack* global = ctx->symbol_stack;
	for (LangC_Symbol* sym = global->symbols; sym; sym = sym->next)
	{
		
	}
	
	return LangC_error_count == 0;
}
