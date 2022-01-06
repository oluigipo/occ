#include "backend_format_pe.h"

internal bool32
LangC_GenIr(LangC_Context* ctx)
{
	Trace();
	
	LangC_SymbolScope* global = ctx->scope;
	LittleMap_Iterator iter = { global->names };
	
	for (LangC_Symbol* sym; LittleMap_Next(&iter, NULL, (void**)&sym); )
	{
		
	}
	
	return ctx->error_count == 0;
}
