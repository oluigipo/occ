internal C_AstNode*
C_UpdateNode(C_Context* ctx, C_Parser* parser, C_AstKind kind, C_AstNode* node)
{
	Assert(node);
	(void)ctx;
	
	if (kind)
		node->kind = kind;
	node->trace = &parser->rd.head->trace;
	
	return node;
}

#define C_CreateNode(ctx, parser, kind, Type) \
( (Type*)C_UpdateNode(ctx, parser, kind, Arena_Push((ctx)->tree_arena, sizeof(Type))) )

internal bool32
C_IsBeginningOfType(C_Context* ctx, C_Parser* parser)
{
	Trace();
	
	switch (parser->rd.head->kind)
	{
		case C_TokenKind_Identifier:
		{
			String ident = parser->rd.head->as_string;
			uint64 hash = SimpleHash(ident) + 1;
			
			C_ParserScope* scope = parser->scope;
			
			while (scope)
			{
				if (scope->typedefed && Map_FetchWithHash(scope->typedefed, ident, hash))
					return true;
				
				scope = scope->up;
			}
			
			return false;
		}
		
		case C_TokenKind_Auto:
		case C_TokenKind_Bool:
		case C_TokenKind_Char:
		case C_TokenKind_Complex:
		case C_TokenKind_Const:
		case C_TokenKind_Double:
		case C_TokenKind_Enum:
		case C_TokenKind_Extern:
		case C_TokenKind_Float:
		case C_TokenKind_Inline:
		case C_TokenKind_Int:
		case C_TokenKind_Long:
		case C_TokenKind_Register:
		case C_TokenKind_Restrict:
		case C_TokenKind_Short:
		case C_TokenKind_Signed:
		case C_TokenKind_Static:
		case C_TokenKind_Struct:
		case C_TokenKind_Typedef:
		case C_TokenKind_Union:
		case C_TokenKind_Unsigned:
		case C_TokenKind_Void:
		case C_TokenKind_Volatile:
		{
			return true;
		}
		
		default: return false;
	}
}

internal C_AstExpr*
C_ParseExprFactor(C_Context* ctx, C_Parser* parser)
{
	Trace();
	
	C_AstExpr* result = C_CreateNode(ctx, parser, 0, C_AstExpr);
	C_AstExpr* head = result;
	
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_Plus:
			{
				head->kind = C_AstKind_Expr1Positive;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Minus:
			{
				head->kind = C_AstKind_Expr1Negative;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Mul:
			{
				head->kind = C_AstKind_Expr1Deref;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_And:
			{
				head->kind = C_AstKind_Expr1Ref;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Not:
			{
				head->kind = C_AstKind_Expr1Not;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_LNot:
			{
				head->kind = C_AstKind_Expr1LogicalNot;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Inc:
			{
				head->kind = C_AstKind_Expr1PrefixInc;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Dec:
			{
				head->kind = C_AstKind_Expr1PrefixDec;
				head = head->unary = C_CreateNode(ctx, parser, 0, C_AstExpr);
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Sizeof:
			{
				head->kind = C_AstKind_Expr1Sizeof;
				
				
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				
			} continue;
		}
		
		break;
	}
	
	switch (parser->rd.head->kind)
	{
		case C_TokenKind_LeftCurl:
		{
			
		} break;
		
		case C_TokenKind_IntLiteral:
		{
			
		} break;
		
		case C_TokenKind_LIntLiteral:
		{
			
		} break;
		
		case C_TokenKind_LLIntLiteral:
		{
			
		} break;
		
		case C_TokenKind_UintLiteral:
		{
			
		} break;
		
		case C_TokenKind_LUintLiteral:
		{
			
		} break;
		
		case C_TokenKind_LLUintLiteral:
		{
			
		} break;
		
		case C_TokenKind_StringLiteral:
		case C_TokenKind_WideStringLiteral:
		{
			
		} break;
		
		case C_TokenKind_FloatLiteral:
		{
			
		} break;
		
		case C_TokenKind_DoubleLiteral:
		{
			
		} break;
		
		case C_TokenKind_Identifier:
		{
			
		} break;
		
		default:
		{
			C_TraceErrorRd(ctx, &parser->rd, "expected expression.");
		} break;
	}
	
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_Inc:
			{
				
			} continue;
			
			case C_TokenKind_Dec:
			{
				
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				
			} continue;
			
			// C_TokenKind_Dot and C_TokenKind_Arrow
			{
				C_AstKind kind;
				
				if (0) case C_TokenKind_Dot:   kind = C_AstKind_Expr2Access;
				if (0) case C_TokenKind_Arrow: kind = C_AstKind_Expr2DerefAccess;
				
				(void)kind;
			} continue;
		}
		
		break;
	}
	
	return result;
}

internal C_AstExpr*
C_ParseExpr(C_Context* ctx, C_Parser* parser, int32 level)
{
	Trace();
	
	C_AstExpr* result = C_ParseExprFactor(ctx, parser);
	
	C_OperatorPrecedence prec;
	while (prec = C_operators_precedence[parser->rd.head->kind],
		   prec.level > level)
	{
		int32 op = C_TokenToExprKind(parser->rd.head->kind);
		C_NextToken(&parser->rd);
		C_AstExpr* right = C_ParseExprFactor(ctx, parser);
		
		C_TokenKind lookahead = parser->rd.head->kind;
		C_OperatorPrecedence lookahead_prec = C_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			C_NextToken(&parser->rd);
			
			C_AstExpr* tmp;
			
			if (lookahead == C_TokenKind_QuestionMark) {
				tmp = C_CreateNode(ctx, parser, C_AstKind_Expr3Condition, C_AstExpr);
				tmp->ternary.left = right;
				tmp->ternary.middle = C_ParseExpr(ctx, parser, 0);
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_Colon);
				tmp->ternary.right = C_ParseExpr(ctx, parser, lookahead_prec.level - 1);
			} else {
				tmp = C_CreateNode(ctx, parser, C_TokenToExprKind(lookahead), C_AstExpr);
				tmp->binary.left = right;
				tmp->binary.right = C_ParseExpr(ctx, parser, level + 1);
			}
			
			lookahead = parser->rd.head->kind;
			lookahead_prec = C_operators_precedence[lookahead];
			right = tmp;
		}
		
		C_AstExpr* tmp = C_CreateNode(ctx, parser, op, C_AstExpr);
		
		if (op == C_AstKind_Expr3Condition)
		{
			tmp->ternary.left = result;
			tmp->ternary.middle = right;
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_Colon);
			tmp->ternary.right = C_ParseExpr(ctx, parser, prec.level);
		}
		else
		{
			tmp->binary.left = result;
			tmp->binary.right = right;
		}
		
		result = tmp;
	}
	
	return result;
}

internal C_AstDecl*
C_ParseGlobalDecl(C_Context* ctx, C_Parser* parser)
{
	Trace();
	
	return NULL;
}

internal C_AstDecl*
C_Parse(C_Context* ctx, C_TokenSlice tokens)
{
	Trace();
	
	C_Parser parser = {
		.rd = {
			.slice_head = tokens.data,
			.slice_end = tokens.data + tokens.size,
		},
		
		.scope = Arena_Push(ctx->tree_arena, sizeof(&parser.scope)),
	};
	
	C_NextToken(&parser.rd);
	Arena_Clear(ctx->scratch_arena);
	
	C_AstDecl* result = NULL;
	C_AstDecl** head = &result;
	
	while (parser.rd.head->kind)
	{
		*head = C_ParseGlobalDecl(ctx, &parser);
		head = &(*head)->next;
	}
	
	return result;
}