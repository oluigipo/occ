//~ NOTE(ljre): Forward Decls
internal C_AstStmt* C_ParseCompoundStmt(C_Context* ctx, C_Parser* parser);
internal C_AstExpr* C_ParseExpr(C_Context* ctx, C_Parser* parser, int32 level);
internal C_AstDecl* C_ParseParameter(C_Context* ctx, C_Parser* parser);
internal C_AstDecl* C_ParseDecl(C_Context* ctx, C_Parser* parser);
internal C_AstType* C_ParseType(C_Context* ctx, C_Parser* parser);

//~ NOTE(ljre): Helpers
internal inline C_AstNode*
C_UpdateNode(C_Context* ctx, C_Parser* parser, C_AstKind kind, C_AstNode* node)
{
	(void)ctx;
	
	if (kind)
		node->kind = kind;
	node->trace = &parser->rd.head->trace;
	
	return node;
}

#define C_CreateNode(ctx, parser, kind, Type) \
( (Type*)C_UpdateNode(ctx, parser, kind, Arena_Push((ctx)->tree_arena, sizeof(Type))) )

//~ NOTE(ljre): Basic typedef information
internal inline void
C_RegisterParserTypedef(C_Context* ctx, C_Parser* parser, String name)
{
	if (!parser->scope->typedefed)
		parser->scope->typedefed = Map_Create(ctx->scratch_arena, 64);
	
	Map_Insert(parser->scope->typedefed, name, (void*)1);
}

internal inline void
C_PushParserTypedefScope(C_Context* ctx, C_Parser* parser)
{
	C_ParserScope* scope = Arena_Push(ctx->scratch_arena, sizeof(*scope));
	
	scope->up = parser->scope;
	parser->scope = scope;
}

internal inline void
C_PopParserTypedefScope(C_Context* ctx, C_Parser* parser)
{
	parser->scope = parser->scope->up;
	
	Assert(parser->scope);
}

internal bool32
C_IsBeginningOfType(C_Context* ctx, C_Parser* parser)
{
	Trace();
	
	switch (parser->rd.head->kind)
	{
		case C_TokenKind_Identifier:
		{
			String ident = parser->rd.head->as_string;
			uint64 hash = SimpleHash(ident);
			
			C_ParserScope* scope = parser->scope;
			
			while (scope)
			{
				if (scope->typedefed && Map_FetchWithHash(scope->typedefed, ident, hash))
					return true;
				
				scope = scope->up;
			}
			
			return false;
		}
		
		// NOTE(ljre): Maybe remove these?
		case C_TokenKind_Inline:
		case C_TokenKind_Auto:
		case C_TokenKind_Extern:
		case C_TokenKind_Register:
		case C_TokenKind_Static:
		case C_TokenKind_Typedef:
		
		case C_TokenKind_Bool:
		case C_TokenKind_Char:
		case C_TokenKind_Complex:
		case C_TokenKind_Const:
		case C_TokenKind_Double:
		case C_TokenKind_Enum:
		case C_TokenKind_Float:
		case C_TokenKind_Int:
		case C_TokenKind_Long:
		case C_TokenKind_Restrict:
		case C_TokenKind_Short:
		case C_TokenKind_Signed:
		case C_TokenKind_Struct:
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

//~ NOTE(ljre): Expressions
internal C_AstExpr*
C_ParseDesignatedInitializer(C_Context* ctx, C_Parser* parser)
{
	Trace();
	C_AstExpr* result = NULL;
	C_AstExpr** head = &result;
	
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_Dot:
			{
				C_AstExpr* node = C_CreateNode(ctx, parser, C_AstKind_ExprDesignatedField, C_AstExpr);
				
				C_NextToken(&parser->rd);
				if (C_AssertToken(ctx, &parser->rd, C_TokenKind_Identifier))
				{
					node->desig_field = parser->rd.head->as_string;
					C_NextToken(&parser->rd);
				}
				
				*head = node;
				head = &node->next;
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				C_AstExpr* node = C_CreateNode(ctx, parser, C_AstKind_ExprDesignatedIndex, C_AstExpr);
				
				C_NextToken(&parser->rd);
				node->desig_index = C_ParseExpr(ctx, parser, 0);
				C_EatToken(ctx, &parser->rd, C_TokenKind_RightBrkt);
				
				*head = node;
				head = &node->next;
			} continue;
		}
		
		break;
	}
	
	return result;
}

internal String
C_ParseStringLiteral(C_Context* ctx, C_Parser* parser, bool* out_is_wide)
{
	Trace();
	Assert(parser->rd.head->kind == C_TokenKind_StringLiteral || parser->rd.head->kind == C_TokenKind_WideStringLiteral);
	
	bool is_wide = false;
	char* buf = Arena_End(ctx->array_arena);
	
	while (parser->rd.head->kind == C_TokenKind_StringLiteral || (is_wide |= parser->rd.head->kind == C_TokenKind_WideStringLiteral))
	{
		const char* head = parser->rd.head->as_string.data;
		const char* end  = parser->rd.head->as_string.size + head;
		
		if (head[0] == 'L')
			++head;
		
		Assert(head[0] == '"');
		Assert(end[-1] == '"');
		
		++head;
		--end;
		
		Arena_PushMemory(ctx->array_arena, end - head, head);
		C_NextToken(&parser->rd);
	}
	
	if (out_is_wide)
		*out_is_wide = is_wide;
	
	return StrMake(buf, (char*)Arena_End(ctx->array_arena) - buf);
}

internal C_AstExpr*
C_ParseExprFactor(C_Context* ctx, C_Parser* parser)
{
	Trace();
	
	C_AstExpr* result = NULL;
	C_AstExpr** head = &result;
	
	//- NOTE(ljre): Prefix
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_Plus:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Positive, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Minus:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Negative, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Mul:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Deref, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_And:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Ref, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Not:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Not, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_LNot:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1LogicalNot, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Inc:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1PrefixInc, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Dec:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1PrefixDec, C_AstExpr);
				head = &(*head)->unary;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Sizeof:
			{
				*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Sizeof, C_AstExpr);
				
				C_NextToken(&parser->rd);
				if (parser->rd.head->kind == C_TokenKind_LeftParen)
				{
					C_NextToken(&parser->rd);
					
					if (C_IsBeginningOfType(ctx, parser))
					{
						(*head)->kind = C_AstKind_ExprSizeofType;
						(*head)->sizeof_type = C_ParseType(ctx, parser);
					}
					else
					{
						(*head)->unary = C_ParseExpr(ctx, parser, 0);
					}
					
					C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
					goto parse_postfix;
				}
				else
					head = &(*head)->unary;
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				C_NextToken(&parser->rd);
				
				if (C_IsBeginningOfType(ctx, parser))
				{
					C_AstType* type = C_ParseType(ctx, parser);
					
					C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
					if (parser->rd.head->kind == C_TokenKind_LeftCurl)
					{
						*head = C_CreateNode(ctx, parser, C_AstKind_ExprCompoundLiteral, C_AstExpr);
						(*head)->compound.type = type;
						head = &(*head)->compound.init;
						
						goto parse_initializer;
					}
					
					*head = C_CreateNode(ctx, parser, C_AstKind_Expr1Cast, C_AstExpr);
					(*head)->cast.to = type;
					head = &(*head)->cast.expr;
				}
				else
				{
					*head = C_ParseExpr(ctx, parser, 0);
					
					C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
					goto parse_postfix;
				}
				
			} continue;
		}
		
		break;
	}
	
	//- NOTE(ljre): Infix
	switch (parser->rd.head->kind)
	{
		// C_TokenKind_IntLiteral, C_TokenKind_LIntLiteral, C_TokenKind_LLIntLiteral
		{
			C_AstKind kind;
			
			if (0) case C_TokenKind_IntLiteral:   kind = C_AstKind_ExprInt;
			if (0) case C_TokenKind_LIntLiteral:  kind = C_AstKind_ExprLInt;
			if (0) case C_TokenKind_LLIntLiteral: kind = C_AstKind_ExprLLInt;
			
			*head = C_CreateNode(ctx, parser, kind, C_AstExpr);
			(*head)->value_int = C_TokenizeIntLiteral(ctx, parser->rd.head->as_string);
			
			C_NextToken(&parser->rd);
		} break;
		
		// C_TokenKind_UintLiteral, C_TokenKind_LUintLiteral, C_TokenKind_LLUintLiteral
		{
			C_AstKind kind;
			
			if (0) case C_TokenKind_UintLiteral:   kind = C_AstKind_ExprUInt;
			if (0) case C_TokenKind_LUintLiteral:  kind = C_AstKind_ExprULInt;
			if (0) case C_TokenKind_LLUintLiteral: kind = C_AstKind_ExprULLInt;
			
			*head = C_CreateNode(ctx, parser, kind, C_AstExpr);
			(*head)->value_uint = (uint64)C_TokenizeIntLiteral(ctx, parser->rd.head->as_string);
			
			C_NextToken(&parser->rd);
		} break;
		
		case C_TokenKind_StringLiteral:
		case C_TokenKind_WideStringLiteral:
		{
			bool is_wide;
			String str = C_ParseStringLiteral(ctx, parser, &is_wide);
			C_AstKind kind = (is_wide) ? C_AstKind_ExprWideString : C_AstKind_ExprString;
			
			*head = C_CreateNode(ctx, parser, kind, C_AstExpr);
			(*head)->value_str = str;
		} break;
		
		case C_TokenKind_FloatLiteral:
		{
			*head = C_CreateNode(ctx, parser, C_AstKind_ExprFloat, C_AstExpr);
			(*head)->value_float = (float)C_TokenizeDoubleLiteral(ctx, parser->rd.head->as_string);
			
			C_NextToken(&parser->rd);
		} break;
		
		case C_TokenKind_DoubleLiteral:
		{
			*head = C_CreateNode(ctx, parser, C_AstKind_ExprDouble, C_AstExpr);
			(*head)->value_double = C_TokenizeDoubleLiteral(ctx, parser->rd.head->as_string);
			
			C_NextToken(&parser->rd);
		} break;
		
		case C_TokenKind_Identifier:
		{
			if (C_IsBeginningOfType(ctx, parser))
				C_TraceErrorRd(ctx, &parser->rd, "type not allowed here.");
			
			*head = C_CreateNode(ctx, parser, C_AstKind_ExprIdent, C_AstExpr);
			(*head)->value_ident = parser->rd.head->as_string;
			
			C_NextToken(&parser->rd);
		} break;
		
		case C_TokenKind_LeftCurl:
		{
			parse_initializer:;
			
			*head = C_CreateNode(ctx, parser, C_AstKind_ExprInitializer, C_AstExpr);
			C_AstExpr** h = &(*head)->init;
			
			C_NextToken(&parser->rd);
			while (parser->rd.head->kind && parser->rd.head->kind != C_TokenKind_RightCurl)
			{
				C_AstExpr* node;
				
				if (parser->rd.head->kind == C_TokenKind_Dot || parser->rd.head->kind == C_TokenKind_LeftBrkt)
				{
					node = C_CreateNode(ctx, parser, C_AstKind_ExprDesignatedInitializer, C_AstExpr);
					node->desig_init.offset = C_ParseDesignatedInitializer(ctx, parser);
					
					C_EatToken(ctx, &parser->rd, C_TokenKind_Assign);
					node->desig_init.expr = C_ParseExpr(ctx, parser, 1);
				}
				else
					node = C_ParseExpr(ctx, parser, 1);
				
				*h = node;
				h = &node->next;
				
				C_TryToEatToken(&parser->rd, C_TokenKind_Comma);
			}
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_RightCurl);
		} break;
		
		default:
		{
			C_TraceErrorRd(ctx, &parser->rd, "expected expression.");
		} break;
	}
	
	//- NOTE(ljre): Postfix
	parse_postfix:;
	
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_Inc:
			{
				C_AstExpr* node = C_CreateNode(ctx, parser, C_AstKind_Expr1PostfixInc, C_AstExpr);
				node->unary = *head;
				*head = node;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Dec:
			{
				C_AstExpr* node = C_CreateNode(ctx, parser, C_AstKind_Expr1PostfixDec, C_AstExpr);
				node->unary = *head;
				*head = node;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				C_AstExpr* node = C_CreateNode(ctx, parser, C_AstKind_Expr2Call, C_AstExpr);
				node->binary.left = *head;
				*head = node;
				
				C_AstExpr** args = &node->binary.right;
				
				C_NextToken(&parser->rd);
				while (parser->rd.head->kind && parser->rd.head->kind != C_TokenKind_RightParen)
				{
					*args = C_ParseExpr(ctx, parser, 1);
					args = &(*args)->next;
					
					if (!C_TryToEatToken(&parser->rd, C_TokenKind_Comma))
						break;
				}
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				C_AstExpr* node = C_CreateNode(ctx, parser, C_AstKind_Expr2Index, C_AstExpr);
				node->binary.left = *head;
				*head = node;
				
				C_NextToken(&parser->rd);
				node->binary.right = C_ParseExpr(ctx, parser, 0);
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_RightBrkt);
			} continue;
			
			// C_TokenKind_Dot, C_TokenKind_Arrow
			{
				C_AstKind kind;
				
				if (0) case C_TokenKind_Dot:   kind = C_AstKind_Expr2Access;
				if (0) case C_TokenKind_Arrow: kind = C_AstKind_Expr2DerefAccess;
				
				C_AstExpr* node = C_CreateNode(ctx, parser, kind, C_AstExpr);
				node->access.expr = *head;
				
				C_NextToken(&parser->rd);
				if (C_AssertToken(ctx, &parser->rd, C_TokenKind_Identifier))
				{
					node->access.field = parser->rd.head->as_string;
					
					C_NextToken(&parser->rd);
				}
				
				*head = node;
			} continue;
		}
		
		break;
	}
	
	return result;
}

// NOTE(ljre): Operator Precedence Parser (https://en.wikipedia.org/wiki/Operator-precedence_parser)
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
				(lookahead_prec.level == prec.level && lookahead_prec.right2left)))
		{
			C_NextToken(&parser->rd);
			
			C_AstExpr* tmp = C_CreateNode(ctx, parser, 0, C_AstExpr);
			
			if (lookahead == C_TokenKind_QuestionMark)
			{
				tmp->kind = C_AstKind_Expr3Condition;
				tmp->ternary.left = right;
				tmp->ternary.middle = C_ParseExpr(ctx, parser, 0);
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_Colon);
				tmp->ternary.right = C_ParseExpr(ctx, parser, lookahead_prec.level - 1);
			}
			else
			{
				tmp->kind = C_TokenToExprKind(lookahead);
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

//~ NOTE(ljre): Statements
internal C_AstStmt*
C_ParseStmt(C_Context* ctx, C_Parser* parser, bool allow_decl)
{
	Trace();
	C_AstStmt* result = NULL;
	
	switch (parser->rd.head->kind)
	{
		case C_TokenKind_Eof:
		{
			C_TraceErrorRd(ctx, &parser->rd, "expected statement before end of file.");
		} break;
		
		//- NOTE(ljre): Normal
		case C_TokenKind_LeftCurl:
		{
			result = C_ParseCompoundStmt(ctx, parser);
		} break;
		
		case C_TokenKind_If:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtIf, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_LeftParen);
			result->ifelse.cond = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
			result->ifelse.then = C_ParseStmt(ctx, parser, false);
			
			if (C_TryToEatToken(&parser->rd, C_TokenKind_Else))
				result->ifelse.otherwise = C_ParseStmt(ctx, parser, false);
		} break;
		
		case C_TokenKind_While:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtWhile, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_LeftParen);
			result->whileloop.cond = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
			result->whileloop.body = C_ParseStmt(ctx, parser, false);
		} break;
		
		case C_TokenKind_For:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtFor, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_LeftParen);
			
			if (parser->rd.head->kind != C_TokenKind_Semicolon)
			{
				if (C_IsBeginningOfType(ctx, parser))
					result->forloop.decl_or_expr = (C_AstNode*)C_ParseDecl(ctx, parser);
				else
					result->forloop.decl_or_expr = (C_AstNode*)C_ParseExpr(ctx, parser, 0);
			}
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
			if (parser->rd.head->kind != C_TokenKind_Semicolon)
				result->forloop.cond = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
			if (parser->rd.head->kind != C_TokenKind_RightParen)
				result->forloop.it = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
			result->forloop.body = C_ParseStmt(ctx, parser, false);
		} break;
		
		case C_TokenKind_Do:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtDoWhile, C_AstStmt);
			
			C_NextToken(&parser->rd);
			result->whileloop.body = C_ParseStmt(ctx, parser, false);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_While);
			C_EatToken(ctx, &parser->rd, C_TokenKind_LeftParen);
			result->whileloop.cond = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
		} break;
		
		case C_TokenKind_Switch:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtSwitch, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_LeftParen);
			result->switchcase.num = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
			result->switchcase.body = C_ParseStmt(ctx, parser, false);
		} break;
		
		case C_TokenKind_Return:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtReturn, C_AstStmt);
			
			C_NextToken(&parser->rd);
			result->expr = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Case:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtCase, C_AstStmt);
			
			C_NextToken(&parser->rd);
			result->casestmt.expr = C_ParseExpr(ctx, parser, 0);
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_Colon);
			result->casestmt.stmt = C_ParseStmt(ctx, parser, false);
		} break;
		
		case C_TokenKind_Goto:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtGoto, C_AstStmt);
			
			C_NextToken(&parser->rd);
			if (C_AssertToken(ctx, &parser->rd, C_TokenKind_Identifier))
			{
				result->go_to = parser->rd.head->as_string;
				
				C_NextToken(&parser->rd);
			}
			
			C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Break:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtBreak, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Continue:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtContinue, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Default:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtBreak, C_AstStmt);
			
			C_NextToken(&parser->rd);
			C_EatToken(ctx, &parser->rd, C_TokenKind_Colon);
			result->stmt = C_ParseStmt(ctx, parser, false);
		} break;
		
		case C_TokenKind_Semicolon:
		{
			result = C_CreateNode(ctx, parser, C_AstKind_StmtEmpty, C_AstStmt);
			
			C_NextToken(&parser->rd);
		} break;
		
		default:
		{
			if (C_IsBeginningOfType(ctx, parser))
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "declaration not allowed here.");
				
				result = (C_AstStmt*)C_ParseDecl(ctx, parser);
			}
			else if (parser->rd.head->kind == C_TokenKind_Identifier && C_PeekToken(&parser->rd)->kind == C_TokenKind_Colon)
			{
				result = C_CreateNode(ctx, parser, C_AstKind_StmtLabel, C_AstStmt);
				result->label.name = parser->rd.head->as_string;
				
				C_NextToken(&parser->rd);
				C_NextToken(&parser->rd);
				result->label.stmt = C_ParseStmt(ctx, parser, false);
			}
			else
				result = (C_AstStmt*)C_ParseExpr(ctx, parser, 0);
		} break;
		
		//- NOTE(ljre): Extensions
		case C_TokenKind_GccAttribute:
		{
		} break;
		
		case C_TokenKind_GccAsm:
		{
		} break;
		
		case C_TokenKind_MsvcAsm:
		{
		} break;
	}
	
	return result;
}

internal C_AstStmt*
C_ParseCompoundStmt(C_Context* ctx, C_Parser* parser)
{
	Trace();
	C_AstStmt* result = C_CreateNode(ctx, parser, C_AstKind_StmtCompound, C_AstStmt);
	C_AstStmt** head = &result->compound;
	
	C_EatToken(ctx, &parser->rd, C_TokenKind_LeftCurl);
	
	// NOTE(ljre): Simple early exit
	if (C_TryToEatToken(&parser->rd, C_TokenKind_RightCurl))
		return result;
	
	C_PushParserTypedefScope(ctx, parser);
	while (parser->rd.head->kind && parser->rd.head->kind != C_TokenKind_RightCurl)
	{
		*head = C_ParseStmt(ctx, parser, true);
		head = &(*head)->next;
	}
	
	C_PopParserTypedefScope(ctx, parser);
	C_EatToken(ctx, &parser->rd, C_TokenKind_RightCurl);
	
	return result;
}

//~ NOTE(ljre): Declarations & Types
internal C_AstDecl*
C_ParseStructBody(C_Context* ctx, C_Parser* parser)
{
	Trace();
	Assert(parser->rd.head->kind == C_TokenKind_LeftCurl);
	C_AstDecl* result = NULL;
	C_AstDecl** head = &result;
	
	C_NextToken(&parser->rd);
	while (parser->rd.head->kind && parser->rd.head->kind != C_TokenKind_RightCurl)
	{
		C_AstDecl* node = C_ParseDecl(ctx, parser);
		
		if (C_TryToEatToken(&parser->rd, C_TokenKind_Colon))
		{
			// TODO(ljre): This syntax is incorrect. The bitfield part is attached to the declarator.
			node->kind = C_AstKind_DeclBitfield;
			node->bitfield = C_ParseExpr(ctx, parser, 1);
		}
		
		*head = node;
		head = &node->next;
		
		if (!C_TryToEatToken(&parser->rd, C_TokenKind_Semicolon))
		{
			C_TraceErrorRd(ctx, &parser->rd, "missing ';' after field.");
			break;
		}
	}
	
	if (!result)
		C_TraceErrorRd(ctx, &parser->rd, "structs and unions need to have at least one field.");
	
	C_EatToken(ctx, &parser->rd, C_TokenKind_RightCurl);
	return result;
}

// NOTE(ljre): 'flags' is a bitset
//
//             1 - If a name is mandatory. Otherwise, it's opitional if 'out_name' is not NULL.
internal C_AstType*
C_ParseDeclaratorType(C_Context* ctx, C_Parser* parser, C_AstType* base_type, int32 flags, String* out_name)
{
	Trace();
	C_AstType* result = base_type;
	C_AstType** head = &result;
	
	//- NOTE(ljre): Prefix
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_Mul:
			{
				C_AstType* node = C_CreateNode(ctx, parser, C_AstKind_TypePointer, C_AstType);
				node->ptr = *head;
				*head = node;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Const:
			{
				Assert(*head != base_type);
				(*head)->flags |= C_AstFlags_Const;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Restrict:
			{
				Assert(*head != base_type);
				(*head)->flags |= C_AstFlags_Restrict;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				C_NextToken(&parser->rd);
				*head = C_ParseDeclaratorType(ctx, parser, *head, flags, out_name);
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
				goto parse_postfix;
			} continue;
			
			case C_TokenKind_MsvcCdecl:
			case C_TokenKind_MsvcStdcall:
			case C_TokenKind_MsvcVectorcall:
			case C_TokenKind_MsvcFastcall:
			{
				// TODO(ljre)
			} continue;
		}
		
		break;
	}
	
	//- NOTE(ljre): Infix
	if (parser->rd.head->kind == C_TokenKind_Identifier)
	{
		if (out_name)
			*out_name = parser->rd.head->as_string;
		else
			C_TraceErrorRd(ctx, &parser->rd, "identifier not allowed here.");
		
		C_NextToken(&parser->rd);
	}
	else if (flags & 1)
		C_TraceErrorRd(ctx, &parser->rd, "expected identifier.");
	
	//- NOTE(ljre): Postfix
	parse_postfix:;
	
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			case C_TokenKind_LeftParen:
			{
				C_AstType* node = C_CreateNode(ctx, parser, C_AstKind_TypeFunction, C_AstType);
				node->function.ret = *head;
				*head = node;
				head = &node->function.ret;
				
				C_AstDecl** p = &node->function.params;
				
				C_NextToken(&parser->rd);
				while (parser->rd.head->kind && parser->rd.head->kind != C_TokenKind_RightParen)
				{
					if (parser->rd.head->kind == C_TokenKind_VarArgs)
					{
						C_NextToken(&parser->rd);
						node->flags |= C_AstFlags_VarArgs;
						break;
					}
					
					*p = C_ParseParameter(ctx, parser);
					p = &(*p)->next;
					
					if (!C_TryToEatToken(&parser->rd, C_TokenKind_Comma))
						break;
				}
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_RightParen);
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				C_AstType* node = C_CreateNode(ctx, parser, C_AstKind_TypeArray, C_AstType);
				node->array.of = *head;
				*head = node;
				head = &node->array.of;
				
				C_NextToken(&parser->rd);
				for (;;)
				{
					if (C_TryToEatToken(&parser->rd, C_TokenKind_Const))
						node->flags |= C_AstFlags_Const;
					else if (C_TryToEatToken(&parser->rd, C_TokenKind_Restrict))
						node->flags |= C_AstFlags_Restrict;
					else
						break;
				}
				
				if (C_TryToEatToken(&parser->rd, C_TokenKind_Static))
					0; // TODO(ljre): static size
				else if (C_TryToEatToken(&parser->rd, C_TokenKind_Mul))
					0; // TODO(ljre): VLA of unknown size
				
				if (parser->rd.head->kind != C_TokenKind_RightBrkt)
					node->array.length = C_ParseExpr(ctx, parser, 1);
				
				C_EatToken(ctx, &parser->rd, C_TokenKind_RightBrkt);
			} continue;
		}
		
		break;
	}
	
	return result;
}

// NOTE(ljre): 'flags' is the same as C_ParseDeclaratorType 'flags'.
internal C_AstDecl*
C_ParseDeclarator(C_Context* ctx, C_Parser* parser, C_AstType* base_type, C_AstKind specifier_kind, C_AstFlags specifier_flags, int32 flags)
{
	Trace();
	C_AstDecl* result = C_CreateNode(ctx, parser, C_AstKind_Decl, C_AstDecl);
	
	result->kind = specifier_kind;
	result->flags = specifier_flags;
	result->type = C_ParseDeclaratorType(ctx, parser, base_type, flags, &result->name);
	
	return result;
}

internal void
C_ParseDeclarationSpecifiers(C_Context* ctx, C_Parser* parser, C_AstType* type, C_AstKind* out_kind, C_AstFlags* out_flags)
{
	Trace();
	C_AstKind decl_kind = 0;
	C_AstFlags decl_flags = 0;
	
	bool allow_decl = (out_kind && out_flags);
	
	for (;;)
	{
		switch (parser->rd.head->kind)
		{
			//- NOTE(ljre): Normal
			case C_TokenKind_Auto:
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "expected type.");
				else if (decl_kind == C_AstKind_DeclAuto)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'auto' keyword.");
				else if (decl_kind != 0)
					C_TraceErrorRd(ctx, &parser->rd, "cannot have more than 1 storage class.");
				else
					decl_kind = C_AstKind_DeclAuto;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Bool:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
					type->kind = C_AstKind_TypeBaseBool;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Char:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
					type->kind = C_AstKind_TypeBaseChar;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Complex:
			{
				if (type->flags & C_AstFlags_Complex)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of '_Complex' keyword.");
				else
					type->flags |= C_AstFlags_Complex;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Const:
			{
				if (type->flags & C_AstFlags_Const)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'const' keyword.");
				else
					type->flags |= C_AstFlags_Const;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Double:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
					type->kind = C_AstKind_TypeBaseDouble;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Extern:
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "expected type.");
				else if (decl_kind == C_AstKind_DeclExtern)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'extern' keyword.");
				else if (decl_kind != 0)
					C_TraceErrorRd(ctx, &parser->rd, "cannot have more than 1 storage class.");
				else
					decl_kind = C_AstKind_DeclExtern;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Enum:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				
				bool has_name_or_body = false;
				type->kind = C_AstKind_TypeBaseEnum;
				
				C_NextToken(&parser->rd);
				if (parser->rd.head->kind == C_TokenKind_Identifier)
				{
					has_name_or_body = true;
					type->enumerator.name = parser->rd.head->as_string;
					
					C_NextToken(&parser->rd);
				}
				
				if (parser->rd.head->kind == C_TokenKind_LeftCurl)
				{
					has_name_or_body = true;
					C_AstExpr** h = &type->enumerator.entries;
					
					C_NextToken(&parser->rd);
					while (parser->rd.head->kind && parser->rd.head->kind != C_TokenKind_RightCurl)
					{
						*h = C_ParseExpr(ctx, parser, 1);
						h = &(*h)->next;
						
						if (!C_TryToEatToken(&parser->rd, C_TokenKind_Comma))
							break;
					}
				}
				
				if (!has_name_or_body)
					C_TraceErrorRd(ctx, &parser->rd, "expected identifier or '{' after 'enum' keyword.");
			} continue;
			
			case C_TokenKind_Float:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
					type->kind = C_AstKind_TypeBaseFloat;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Int:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
					type->kind = C_AstKind_TypeBaseInt;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Inline:
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "expected type.");
				else if (decl_flags & C_AstFlags_Inline)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'inline' keyword.");
				else
					decl_flags |= C_AstFlags_Inline;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Long:
			{
				if (type->flags & C_AstFlags_LongLong)
					C_TraceErrorRd(ctx, &parser->rd, "'long long long' is too long for me.");
				else if (type->flags & C_AstFlags_Long)
					type->flags |= C_AstFlags_LongLong;
				else
					type->flags |= C_AstFlags_Long;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Register:
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "expected type.");
				else if (decl_kind == C_AstKind_DeclRegister)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'register' keyword.");
				else if (decl_kind != 0)
					C_TraceErrorRd(ctx, &parser->rd, "cannot have more than 1 storage class.");
				else
					decl_kind = C_AstKind_DeclRegister;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Restrict:
			{
				if (type->flags & C_AstFlags_Restrict)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'restrict' keyword.");
				else
					type->flags |= C_AstFlags_Restrict;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Short:
			{
				if (type->flags & C_AstFlags_Short)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'short' keyword.");
				else
					type->flags |= C_AstFlags_Short;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Signed:
			{
				if (type->flags & C_AstFlags_Signed)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'signed' keyword.");
				else
					type->flags |= C_AstFlags_Signed;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Static:
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "expected type.");
				else if (decl_kind == C_AstKind_DeclStatic)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'static' keyword.");
				else if (decl_kind != 0)
					C_TraceErrorRd(ctx, &parser->rd, "cannot have more than 1 storage class.");
				else
					decl_kind = C_AstKind_DeclStatic;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Struct:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				
				bool has_name_or_body = false;
				type->kind = C_AstKind_TypeBaseStruct;
				
				C_NextToken(&parser->rd);
				if (parser->rd.head->kind == C_TokenKind_Identifier)
				{
					has_name_or_body = true;
					type->structure.name = parser->rd.head->as_string;
					
					C_NextToken(&parser->rd);
				}
				
				if (parser->rd.head->kind == C_TokenKind_LeftCurl)
				{
					has_name_or_body = true;
					type->structure.body = C_ParseStructBody(ctx, parser);
				}
				
				if (!has_name_or_body)
					C_TraceErrorRd(ctx, &parser->rd, "expected identifier or '{' after 'struct' keyword.");
			} continue;
			
			case C_TokenKind_Typedef:
			{
				if (!allow_decl)
					C_TraceErrorRd(ctx, &parser->rd, "expected type.");
				else if (decl_kind == C_AstKind_DeclTypedef)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'typedef' keyword.");
				else if (decl_kind != 0)
					C_TraceErrorRd(ctx, &parser->rd, "cannot have more than 1 storage class.");
				else
					decl_kind = C_AstKind_DeclTypedef;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Union:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				
				bool has_name_or_body = false;
				type->kind = C_AstKind_TypeBaseUnion;
				
				C_NextToken(&parser->rd);
				if (parser->rd.head->kind == C_TokenKind_Identifier)
				{
					has_name_or_body = true;
					type->structure.name = parser->rd.head->as_string;
					
					C_NextToken(&parser->rd);
				}
				
				if (parser->rd.head->kind == C_TokenKind_LeftCurl)
				{
					has_name_or_body = true;
					type->structure.body = C_ParseStructBody(ctx, parser);
				}
				
				if (!has_name_or_body)
					C_TraceErrorRd(ctx, &parser->rd, "expected identifier or '{' after 'union' keyword.");
			} continue;
			
			case C_TokenKind_Unsigned:
			{
				if (type->flags & C_AstFlags_Unsigned)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'unsigned' keyword.");
				else
					type->flags |= C_AstFlags_Unsigned;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Void:
			{
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
					type->kind = C_AstKind_TypeBaseVoid;
				
				C_NextToken(&parser->rd);
			} continue;
			
			case C_TokenKind_Volatile:
			{
				if (type->flags & C_AstFlags_Volatile)
					C_TraceErrorRd(ctx, &parser->rd, "repeated use of 'volatile' keyword.");
				else
					type->flags |= C_AstFlags_Volatile;
				
				C_NextToken(&parser->rd);
			} continue;
			
			//- NOTE(ljre): Type or get off
			case C_TokenKind_Identifier:
			{
				if (!C_IsBeginningOfType(ctx, parser))
					break;
				
				if (type->kind != C_AstKind_Type)
					C_TraceErrorRd(ctx, &parser->rd, "object cannot have more than 1 type; did you forget a semicolon?");
				else
				{
					type->kind = C_AstKind_TypeBaseTypename;
					type->name = parser->rd.head->as_string;
				}
				
				C_NextToken(&parser->rd);
			} continue;
			
			//- NOTE(ljre): Extensions
			case C_TokenKind_GccExtension:
			{
				// TODO(ljre)
			} continue;
			
			case C_TokenKind_MsvcDeclspec:
			{
				// TODO(ljre)
			} continue;
			
			case C_TokenKind_MsvcCdecl:
			case C_TokenKind_MsvcStdcall:
			case C_TokenKind_MsvcVectorcall:
			case C_TokenKind_MsvcFastcall:
			{
				// TODO(ljre)
			} continue;
			
			case C_TokenKind_MsvcForceinline:
			{
				// TODO(ljre)
			} continue;
		}
		
		break;
	}
	
	// TODO(ljre): Validate usage of flags and kinds for both declaration and types.
	
	if (out_kind)
		*out_kind = decl_kind;
	if (out_flags)
		*out_flags = decl_flags;
}

internal C_AstType*
C_ParseType(C_Context* ctx, C_Parser* parser)
{
	Trace();
	C_AstType* result = C_CreateNode(ctx, parser, C_AstKind_Type, C_AstType);
	
	C_ParseDeclarationSpecifiers(ctx, parser, result, NULL, NULL);
	result = C_ParseDeclaratorType(ctx, parser, result, 0, NULL);
	
	return result;
}

internal C_AstDecl*
C_ParseParameter(C_Context* ctx, C_Parser* parser)
{
	Trace();
	C_AstKind specifier_kind;
	C_AstFlags specifier_flags;
	C_AstType* base_type = C_CreateNode(ctx, parser, C_AstKind_Type, C_AstType);
	
	C_ParseDeclarationSpecifiers(ctx, parser, base_type, &specifier_kind, &specifier_flags);
	
	C_AstDecl* result = C_ParseDeclarator(ctx, parser, base_type, specifier_kind, specifier_flags, 0);
	if (C_TryToEatToken(&parser->rd, C_TokenKind_Assign))
	{
		C_TraceErrorRd(ctx, &parser->rd, "default parameter value is not allowed.");
		result->init = C_ParseExpr(ctx, parser, 1);
	}
	
	return result;
}

internal C_AstDecl*
C_ParseDecl(C_Context* ctx, C_Parser* parser)
{
	Trace();
	C_AstKind specifier_kind;
	C_AstFlags specifier_flags;
	C_AstType* base_type = C_CreateNode(ctx, parser, C_AstKind_Type, C_AstType);
	
	C_ParseDeclarationSpecifiers(ctx, parser, base_type, &specifier_kind, &specifier_flags);
	if (parser->rd.head->kind == C_TokenKind_Semicolon)
	{
		C_AstDecl* r = C_CreateNode(ctx, parser, C_AstKind_Decl, C_AstDecl);
		r->type = base_type;
		
		C_NextToken(&parser->rd);
		return r;
	}
	
	C_AstDecl* result = NULL;
	C_AstDecl** head = &result;
	
	bool should_eat_semicolon = true;
	
	do
	{
		C_AstDecl* declarator = C_ParseDeclarator(ctx, parser, base_type, specifier_kind, specifier_flags, 1);
		
		*head = declarator;
		head = &declarator->next;
		
		if (specifier_kind == C_AstKind_DeclTypedef && declarator->name.size > 0)
			C_RegisterParserTypedef(ctx, parser, declarator->name);
		
		if (declarator->type && declarator->type->kind == C_AstKind_TypeFunction && parser->rd.head->kind == C_TokenKind_LeftCurl)
		{
			declarator->body = C_ParseCompoundStmt(ctx, parser);
			should_eat_semicolon = false;
			break;
		}
		else if (C_TryToEatToken(&parser->rd, C_TokenKind_Assign))
		{
			declarator->init = C_ParseExpr(ctx, parser, 1);
		}
	}
	while (C_TryToEatToken(&parser->rd, C_TokenKind_Comma));
	
	if (should_eat_semicolon)
		C_EatToken(ctx, &parser->rd, C_TokenKind_Semicolon);
	
	return result;
}

//~ NOTE(ljre): Main function API.
internal C_AstDecl*
C_Parse(C_Context* ctx, C_TokenSlice tokens)
{
	Trace();
	Arena_Clear(ctx->scratch_arena);
	
	C_Parser parser = {
		.rd = {
			.slice_head = tokens.data,
			.slice_end = tokens.data + tokens.size,
		},
		
		.scope = Arena_Push(ctx->scratch_arena, sizeof(*parser.scope)),
	};
	
	C_NextToken(&parser.rd);
	
	parser.scope->typedefed = Map_Create(ctx->scratch_arena, 1 << 13);
	
	C_AstDecl* result = NULL;
	C_AstDecl** head = &result;
	
	while (parser.rd.head->kind)
	{
		*head = C_ParseDecl(ctx, &parser);
		head = &(*head)->next;
	}
	
	return result;
}
