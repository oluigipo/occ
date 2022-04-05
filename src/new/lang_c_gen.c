internal void
C_GenTestGlobalDecl(C_Context* ctx, C_AstDecl* decl)
{
	switch (decl->kind)
	{
		case C_AstKind_Decl:
		{
			if (decl->type && decl->type->kind == C_AstKind_TypeFunction)
			{
				
			}
			else
			{
				Arena_Printf(ctx->array_arena, "");
			}
		} break;
		
		case C_AstKind_DeclStatic: break;
		case C_AstKind_DeclExtern: break;
		case C_AstKind_DeclTypedef: break;
		
		case C_AstKind_DeclAuto:
		case C_AstKind_DeclRegister:
		{
			// TODO(ljre): Error
		} break;
		
		case C_AstKind_DeclBitfield:
		default: Unreachable(); break;
	}
}

internal String
C_GenTest(C_Context* ctx, C_AstDecl* ast)
{
	char* buf = Arena_End(ctx->array_arena);
	
	for (C_AstDecl* decl = ast; decl; decl = decl->next)
		C_GenTestGlobalDecl(ctx, decl);
	
	return StrMake(buf, (char*)Arena_End(ctx->array_arena) - buf);
}
