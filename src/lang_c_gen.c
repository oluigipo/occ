#include "backend_format_pe.h"

internal bool32
C_GenIr(C_Context* ctx)
{
	Trace();
	
	C_SymbolScope* global = ctx->scope;
	LittleMap_Iterator iter = { global->names };
	
	for (C_Symbol* sym; LittleMap_Next(&iter, NULL, (void**)&sym); )
	{
		
	}
	
	return ctx->error_count == 0;
}
