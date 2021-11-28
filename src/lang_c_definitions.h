#ifndef LANG_C_DEFINITIONS_H
#define LANG_C_DEFINITIONS_H

// NOTE(ljre): Forward decls
struct LangC_Context typedef LangC_Context;
struct LangC_OperatorPrecedence typedef LangC_OperatorPrecedence;
struct LangC_TokenList typedef LangC_TokenList;
struct LangC_LexerFile typedef LangC_LexerFile;
struct LangC_SymbolStack typedef LangC_SymbolStack;
struct LangC_Symbol typedef LangC_Symbol;
struct LangC_Node typedef LangC_Node;

enum LangC_TokenKind
{
	LangC_TokenKind_Eof = 0,
	
	//- NOTE(ljre): Keywords
	LangC_TokenKind__FirstKeyword,
	LangC_TokenKind_Auto = LangC_TokenKind__FirstKeyword,
	LangC_TokenKind_Break,
	LangC_TokenKind_Case,
	LangC_TokenKind_Char,
	LangC_TokenKind_Const,
	LangC_TokenKind_Continue,
	LangC_TokenKind_Default,
	LangC_TokenKind_Do,
	LangC_TokenKind_Double,
	LangC_TokenKind_Else,
	LangC_TokenKind_Enum,
	LangC_TokenKind_Extern,
	LangC_TokenKind_Float,
	LangC_TokenKind_For,
	LangC_TokenKind_Goto,
	LangC_TokenKind_If,
	LangC_TokenKind_Inline,
	LangC_TokenKind_Int,
	LangC_TokenKind_Long,
	LangC_TokenKind_Register,
	LangC_TokenKind_Restrict,
	LangC_TokenKind_Return,
	LangC_TokenKind_Short,
	LangC_TokenKind_Signed,
	LangC_TokenKind_Sizeof,
	LangC_TokenKind_Static,
	LangC_TokenKind_Struct,
	LangC_TokenKind_Switch,
	LangC_TokenKind_Typedef,
	LangC_TokenKind_Union,
	LangC_TokenKind_Unsigned,
	LangC_TokenKind_Void,
	LangC_TokenKind_Volatile,
	LangC_TokenKind_While,
	LangC_TokenKind_Bool,
	LangC_TokenKind_Complex,
	
	// NOTE(ljre): GCC stuff
	LangC_TokenKind_GccAttribute, // __attribute __attribute__
	LangC_TokenKind_GccAsm, // __asm__ asm
	LangC_TokenKind_GccExtension, // __extension__
	LangC_TokenKind_GccTypeof, // __typeof__
	LangC_TokenKind_GccAutoType, // __auto_type
	
	// NOTE(ljre): MSVC stuff
	LangC_TokenKind_MsvcDeclspec, // __declspec
	LangC_TokenKind_MsvcAsm, // __asm
	LangC_TokenKind_MsvcForceinline, // __forceinline
	LangC_TokenKind_MsvcCdecl, // __cdecl
	LangC_TokenKind_MsvcStdcall, // __stdcall
	LangC_TokenKind_MsvcVectorcall, // __vectorcall
	LangC_TokenKind_MsvcFastcall, // __fastcall
	
	LangC_TokenKind__LastKeyword = LangC_TokenKind_MsvcFastcall,
	
	//- NOTE(ljre): Rest
	// NOTE(ljre): those 3 below shall be in order
	LangC_TokenKind_IntLiteral,
	LangC_TokenKind_LIntLiteral,
	LangC_TokenKind_LLIntLiteral,
	
	// NOTE(ljre): same thing for those 3
	LangC_TokenKind_UintLiteral,
	LangC_TokenKind_LUintLiteral,
	LangC_TokenKind_LLUintLiteral,
	
	LangC_TokenKind_StringLiteral,
	LangC_TokenKind_WideStringLiteral,
	LangC_TokenKind_FloatLiteral,
	LangC_TokenKind_DoubleLiteral,
	
	LangC_TokenKind_Identifier,
	
	LangC_TokenKind_LeftParen, // (
	LangC_TokenKind_RightParen, // )
	LangC_TokenKind_LeftBrkt, // [
	LangC_TokenKind_RightBrkt, // ]
	LangC_TokenKind_LeftCurl, // {
	LangC_TokenKind_RightCurl, // }
	LangC_TokenKind_Dot, // .
	LangC_TokenKind_VarArgs, // ...
	LangC_TokenKind_Arrow, // ->
	LangC_TokenKind_Comma, // ,
	LangC_TokenKind_Colon, // :
	LangC_TokenKind_Semicolon, // ;
	LangC_TokenKind_QuestionMark, // ?
	LangC_TokenKind_Plus, // +
	LangC_TokenKind_Minus, // -
	LangC_TokenKind_Mul, // *
	LangC_TokenKind_Div, // /
	LangC_TokenKind_Mod, // %
	LangC_TokenKind_LThan, // <
	LangC_TokenKind_GThan, // >
	LangC_TokenKind_LEqual, // <=
	LangC_TokenKind_GEqual, // >=
	LangC_TokenKind_Equals, // ==
	LangC_TokenKind_Inc, // ++
	LangC_TokenKind_Dec, // --
	LangC_TokenKind_LNot, // !
	LangC_TokenKind_LAnd, // &&
	LangC_TokenKind_LOr, // ||
	LangC_TokenKind_Not, // ~
	LangC_TokenKind_And, // &
	LangC_TokenKind_Or, // |
	LangC_TokenKind_Xor, // ^
	LangC_TokenKind_LeftShift, // <<
	LangC_TokenKind_RightShift, // >>
	LangC_TokenKind_NotEquals, // !=
	LangC_TokenKind_Assign, // =
	LangC_TokenKind_PlusAssign, // +=
	LangC_TokenKind_MinusAssign, // -=
	LangC_TokenKind_MulAssign, // *=
	LangC_TokenKind_DivAssign, // /=
	LangC_TokenKind_ModAssign, // %=
	LangC_TokenKind_LeftShiftAssign, // <<=
	LangC_TokenKind_RightShiftAssign, // >>=
	LangC_TokenKind_AndAssign, // &=
	LangC_TokenKind_OrAssign, // |=
	LangC_TokenKind_XorAssign, // ^=
	LangC_TokenKind_Hashtag, // #
	
	// only used when 'LangC_Lexer.preprocessor' is true
	LangC_TokenKind_NewLine,
	LangC_TokenKind_DoubleHashtag, // ##
	
	LangC_TokenKind__Count,
} typedef LangC_TokenKind;

internal const char* LangC_token_str_table[LangC_TokenKind__Count] = {
	[LangC_TokenKind_Eof] = "(EOF)",
	
	[LangC_TokenKind_Auto] = "auto",
	[LangC_TokenKind_Break] = "break",
	[LangC_TokenKind_Case] = "case",
	[LangC_TokenKind_Char] = "char",
	[LangC_TokenKind_Const] = "const",
	[LangC_TokenKind_Continue] = "continue",
	[LangC_TokenKind_Default] = "default",
	[LangC_TokenKind_Do] = "do",
	[LangC_TokenKind_Double] = "double",
	[LangC_TokenKind_Else] = "else",
	[LangC_TokenKind_Enum] = "enum",
	[LangC_TokenKind_Extern] = "extern",
	[LangC_TokenKind_Float] = "float",
	[LangC_TokenKind_For] = "for",
	[LangC_TokenKind_Goto] = "goto",
	[LangC_TokenKind_If] = "if",
	[LangC_TokenKind_Inline] = "inline",
	[LangC_TokenKind_Int] = "int",
	[LangC_TokenKind_Long] = "long",
	[LangC_TokenKind_Register] = "register",
	[LangC_TokenKind_Restrict] = "restrict",
	[LangC_TokenKind_Return] = "return",
	[LangC_TokenKind_Short] = "short",
	[LangC_TokenKind_Signed] = "signed",
	[LangC_TokenKind_Sizeof] = "sizeof",
	[LangC_TokenKind_Static] = "static",
	[LangC_TokenKind_Struct] = "struct",
	[LangC_TokenKind_Switch] = "switch",
	[LangC_TokenKind_Typedef] = "typedef",
	[LangC_TokenKind_Union] = "union",
	[LangC_TokenKind_Unsigned] = "unsigned",
	[LangC_TokenKind_Void] = "void",
	[LangC_TokenKind_Volatile] = "volatile",
	[LangC_TokenKind_While] = "while",
	[LangC_TokenKind_Bool] = "bool",
	[LangC_TokenKind_Complex] = "complex",
	
	[LangC_TokenKind_GccAttribute] = "__attribute__",
	[LangC_TokenKind_GccAsm] = "__asm__",
	[LangC_TokenKind_GccExtension] = "__extension__",
	[LangC_TokenKind_GccTypeof] = "__typeof__",
	[LangC_TokenKind_GccAutoType] = "__auto_type",
	
	[LangC_TokenKind_MsvcDeclspec] = "__declspec",
	[LangC_TokenKind_MsvcAsm] = "__asm",
	[LangC_TokenKind_MsvcForceinline] = "__forceinline",
	[LangC_TokenKind_MsvcCdecl] = "__cdecl",
	[LangC_TokenKind_MsvcStdcall] = "__stdcall",
	[LangC_TokenKind_MsvcVectorcall] = "__vectorcall",
	[LangC_TokenKind_MsvcFastcall] = "__fastcall",
	
	[LangC_TokenKind_IntLiteral] = "(int literal)",
	[LangC_TokenKind_LIntLiteral] = "(long literal)",
	[LangC_TokenKind_LLIntLiteral] = "(long long literal)",
	[LangC_TokenKind_UintLiteral] = "(unsigned literal)",
	[LangC_TokenKind_LUintLiteral] = "(unsigned long literal)",
	[LangC_TokenKind_LLUintLiteral] = "(unsigned long long literal)",
	[LangC_TokenKind_StringLiteral] = "(const char[] literal)",
	[LangC_TokenKind_WideStringLiteral] = "(const wchar_t[] literal)",
	[LangC_TokenKind_FloatLiteral] = "(float literal)",
	[LangC_TokenKind_DoubleLiteral] = "(double literal)",
	
	[LangC_TokenKind_Identifier] = "(identifier)",
	
	[LangC_TokenKind_LeftParen] = "(",
	[LangC_TokenKind_RightParen] = ")",
	[LangC_TokenKind_LeftBrkt] = "[",
	[LangC_TokenKind_RightBrkt] = "]",
	[LangC_TokenKind_LeftCurl] = "{",
	[LangC_TokenKind_RightCurl] = "}",
	[LangC_TokenKind_Dot] = ".",
	[LangC_TokenKind_VarArgs] = "...",
	[LangC_TokenKind_Arrow] = "->",
	[LangC_TokenKind_Comma] = ",",
	[LangC_TokenKind_Colon] = ":",
	[LangC_TokenKind_Semicolon] = ";",
	[LangC_TokenKind_QuestionMark] = "?",
	[LangC_TokenKind_Plus] = "+",
	[LangC_TokenKind_Minus] = "-",
	[LangC_TokenKind_Mul] = "*",
	[LangC_TokenKind_Div] = "/",
	[LangC_TokenKind_Mod] = "%",
	[LangC_TokenKind_LThan] = "<",
	[LangC_TokenKind_GThan] = ">",
	[LangC_TokenKind_LEqual] = "<=",
	[LangC_TokenKind_GEqual] = ">=",
	[LangC_TokenKind_Equals] = "==",
	[LangC_TokenKind_Inc] = "++",
	[LangC_TokenKind_Dec] = "--",
	[LangC_TokenKind_LNot] = "!",
	[LangC_TokenKind_LAnd] = "&&",
	[LangC_TokenKind_LOr] = "||",
	[LangC_TokenKind_Not] = "~",
	[LangC_TokenKind_And] = "&",
	[LangC_TokenKind_Or] = "|",
	[LangC_TokenKind_Xor] = "^",
	[LangC_TokenKind_LeftShift] = "<<",
	[LangC_TokenKind_RightShift] = ">>",
	[LangC_TokenKind_NotEquals] = "!=",
	[LangC_TokenKind_Assign] = "=",
	[LangC_TokenKind_PlusAssign] = "+=",
	[LangC_TokenKind_MinusAssign] = "-=",
	[LangC_TokenKind_MulAssign] = "*=",
	[LangC_TokenKind_DivAssign] = "/=",
	[LangC_TokenKind_ModAssign] = "%=",
	[LangC_TokenKind_LeftShiftAssign] = "<<=",
	[LangC_TokenKind_RightShiftAssign] = ">>=",
	[LangC_TokenKind_AndAssign] = "&=",
	[LangC_TokenKind_OrAssign] = "|=",
	[LangC_TokenKind_XorAssign] = "^=",
	
	[LangC_TokenKind_NewLine] = "(EOL)",
	[LangC_TokenKind_DoubleHashtag] = "##",
	[LangC_TokenKind_Hashtag] = "#",
};

struct LangC_OperatorPrecedence
{
	// from lower to higher
	int8 level; // if <= 0, it's not a binary operator
	bool8 right2left;
};

internal LangC_OperatorPrecedence LangC_operators_precedence[LangC_TokenKind__Count] = {
	[LangC_TokenKind_Comma] = { 1, false, },
	
	[LangC_TokenKind_Assign] = { 2, true, },
	[LangC_TokenKind_PlusAssign] = { 2, true, },
	[LangC_TokenKind_MinusAssign] = { 2, true, },
	[LangC_TokenKind_MulAssign] = { 2, true, },
	[LangC_TokenKind_DivAssign] = { 2, true, },
	[LangC_TokenKind_ModAssign] = { 2, true, },
	[LangC_TokenKind_LeftShiftAssign] = { 2, true, },
	[LangC_TokenKind_RightShiftAssign] = { 2, true, },
	[LangC_TokenKind_AndAssign] = { 2, true, },
	[LangC_TokenKind_OrAssign] = { 2, true, },
	[LangC_TokenKind_XorAssign] = { 2, true, },
	
	[LangC_TokenKind_QuestionMark] = { 3, true, },
	
	[LangC_TokenKind_LOr] = { 3, false, },
	[LangC_TokenKind_LAnd] = { 4, false, },
	[LangC_TokenKind_Or] = { 5, false, },
	[LangC_TokenKind_Xor] = { 6, false, },
	[LangC_TokenKind_And] = { 7, false, },
	
	[LangC_TokenKind_Equals] = { 8, false, },
	[LangC_TokenKind_NotEquals] = { 8, false, },
	
	[LangC_TokenKind_LThan] = { 9, false, },
	[LangC_TokenKind_GThan] = { 9, false, },
	[LangC_TokenKind_LEqual] = { 9, false, },
	[LangC_TokenKind_GEqual] = { 9, false, },
	
	[LangC_TokenKind_LeftShift] = { 10, false, },
	[LangC_TokenKind_RightShift] = { 10, false, },
	
	[LangC_TokenKind_Plus] = { 11, false, },
	[LangC_TokenKind_Minus] = { 11, false, },
	
	[LangC_TokenKind_Mul] = { 12, false, },
	[LangC_TokenKind_Div] = { 12, false, },
	[LangC_TokenKind_Mod] = { 12, false, },
};

struct LangC_Token
{
	int32 line, col;
	LangC_TokenKind kind;
	String as_string;
	String leading_spaces;
	
	union
	{
		int64 value_int;
		uint64 value_uint;
		String value_str;
		String value_ident;
		float value_float;
		double value_double;
	};
}
typedef LangC_Token;

struct LangC_TokenList
{
	LangC_TokenList* next;
	LangC_Token token;
};

struct LangC_LexerFile
{
	String path;
	int32 included_line;
	bool32 is_system_file;
	LangC_LexerFile* included_from;
};

struct LangC_Lexer
{
	LangC_Token token;
	LangC_LexerFile* file;
	Arena* arena;
	
	// NOTE(ljre): The lexer is special because we need multiple of those to run within a single context.
	//             It can also be NULL.
	LangC_Context* ctx;
	
	LangC_TokenList* waiting_token;
	LangC_TokenList* last_waiting_token;
	
	const char* head;
	const char* previous_head;
	int32 line, col;
	
	// When this bool is set, the following happens:
	//     - newlines and hashtags are tokens;
	//     - keywords are going to be LangC_TokenKind_Identifier;
	bool16 preprocessor;
	
	bool16 token_was_pushed;
}
typedef LangC_Lexer;

enum LangC_NodeKind
{
	LangC_NodeKind_Null = 0,
	LangC_NodeKind__Category = 12,
	LangC_NodeKind__CategoryMask = ~((1<<LangC_NodeKind__Category)-1),
	
	LangC_NodeKind_Type = 1 << LangC_NodeKind__Category,
	LangC_NodeKind_TypeBaseChar,
	LangC_NodeKind_TypeBase__First = LangC_NodeKind_TypeBaseChar,
	LangC_NodeKind_TypeBaseInt,
	LangC_NodeKind_TypeBaseFloat,
	LangC_NodeKind_TypeBaseDouble,
	LangC_NodeKind_TypeBaseVoid,
	LangC_NodeKind_TypeBaseBool,
	LangC_NodeKind_TypeBaseTypename,
	LangC_NodeKind_TypeBaseStruct,
	LangC_NodeKind_TypeBaseUnion,
	LangC_NodeKind_TypeBaseEnum,
	LangC_NodeKind_TypeBase__Last = LangC_NodeKind_TypeBaseEnum,
	LangC_NodeKind_TypeBaseEnumEntry,
	LangC_NodeKind_TypeFunction,
	LangC_NodeKind_TypePointer,
	LangC_NodeKind_TypeArray,
	
	LangC_NodeKind_Decl = 2 << LangC_NodeKind__Category,
	LangC_NodeKind_DeclStatic,
	LangC_NodeKind_DeclExtern,
	LangC_NodeKind_DeclAuto,
	LangC_NodeKind_DeclTypedef,
	LangC_NodeKind_DeclRegister,
	
	LangC_NodeKind_Stmt = 3 << LangC_NodeKind__Category,
	LangC_NodeKind_StmtEmpty,
	LangC_NodeKind_StmtExpr,
	LangC_NodeKind_StmtIf,
	LangC_NodeKind_StmtDoWhile,
	LangC_NodeKind_StmtWhile,
	LangC_NodeKind_StmtFor,
	LangC_NodeKind_StmtSwitch,
	LangC_NodeKind_StmtReturn,
	LangC_NodeKind_StmtGoto,
	LangC_NodeKind_StmtCompound,
	LangC_NodeKind_StmtLabel,
	LangC_NodeKind_StmtCase,
	LangC_NodeKind_StmtGccAsm,
	
	LangC_NodeKind_ExprFactor = 4 << LangC_NodeKind__Category,
	LangC_NodeKind_ExprIdent,
	LangC_NodeKind_ExprInt,
	LangC_NodeKind_ExprLInt,
	LangC_NodeKind_ExprLLInt,
	LangC_NodeKind_ExprUInt,
	LangC_NodeKind_ExprULInt,
	LangC_NodeKind_ExprULLInt,
	LangC_NodeKind_ExprFloat,
	LangC_NodeKind_ExprDouble,
	LangC_NodeKind_ExprString,
	LangC_NodeKind_ExprWideString,
	LangC_NodeKind_ExprCompoundLiteral,
	LangC_NodeKind_ExprInitializer,
	LangC_NodeKind_ExprInitializerMember,
	LangC_NodeKind_ExprInitializerIndex,
	
	LangC_NodeKind_Expr1 = 5 << LangC_NodeKind__Category,
	LangC_NodeKind_Expr1Plus,
	LangC_NodeKind_Expr1Negative,
	LangC_NodeKind_Expr1Not,
	LangC_NodeKind_Expr1LogicalNot,
	LangC_NodeKind_Expr1Deref,
	LangC_NodeKind_Expr1Ref,
	LangC_NodeKind_Expr1PrefixInc,
	LangC_NodeKind_Expr1PrefixDec,
	LangC_NodeKind_Expr1PostfixInc,
	LangC_NodeKind_Expr1PostfixDec,
	LangC_NodeKind_Expr1Sizeof,
	LangC_NodeKind_Expr1SizeofType,
	LangC_NodeKind_Expr1Cast,
	
	LangC_NodeKind_Expr2 = 6 << LangC_NodeKind__Category,
	LangC_NodeKind_Expr2Add,
	LangC_NodeKind_Expr2Sub,
	LangC_NodeKind_Expr2Mul,
	LangC_NodeKind_Expr2Div,
	LangC_NodeKind_Expr2Mod,
	LangC_NodeKind_Expr2LThan,
	LangC_NodeKind_Expr2GThan,
	LangC_NodeKind_Expr2LEqual,
	LangC_NodeKind_Expr2GEqual,
	LangC_NodeKind_Expr2Equals,
	LangC_NodeKind_Expr2NotEquals,
	LangC_NodeKind_Expr2LeftShift,
	LangC_NodeKind_Expr2RightShift,
	LangC_NodeKind_Expr2And,
	LangC_NodeKind_Expr2Or,
	LangC_NodeKind_Expr2Xor,
	LangC_NodeKind_Expr2LogicalAnd,
	LangC_NodeKind_Expr2LogicalOr,
	LangC_NodeKind_Expr2Assign,
	LangC_NodeKind_Expr2AssignAdd,
	LangC_NodeKind_Expr2AssignSub,
	LangC_NodeKind_Expr2AssignMul,
	LangC_NodeKind_Expr2AssignDiv,
	LangC_NodeKind_Expr2AssignMod,
	LangC_NodeKind_Expr2AssignLeftShift,
	LangC_NodeKind_Expr2AssignRightShift,
	LangC_NodeKind_Expr2AssignAnd,
	LangC_NodeKind_Expr2AssignOr,
	LangC_NodeKind_Expr2AssignXor,
	LangC_NodeKind_Expr2Comma,
	LangC_NodeKind_Expr2Call,
	LangC_NodeKind_Expr2Index,
	LangC_NodeKind_Expr2Access,
	LangC_NodeKind_Expr2DerefAccess,
	
	LangC_NodeKind_Expr3 = 7 << LangC_NodeKind__Category,
	LangC_NodeKind_Expr3Condition,
	
	LangC_NodeKind_Attribute = 8 << LangC_NodeKind__Category,
	LangC_NodeKind_AttributePacked,
	LangC_NodeKind_AttributeBitfield,
	
	LangC_NodeKind__CategoryCount = 8,
}
typedef LangC_NodeKind;

enum LangC_NodeFlags
{
	LangC_NodeFlags_Poisoned = 1 << 15,
	LangC_NodeFlags_Implicit = 1 << 14,
	LangC_NodeFlags_Decayed = 1 << 13,
	
	// 000
	LangC_NodeFlags_Volatile = 1024,
	LangC_NodeFlags_Restrict = 512,
	LangC_NodeFlags_Const = 256,
	
	// 00
	LangC_NodeFlags_VarArgs = 2,
	LangC_NodeFlags_Inline = 1,
	
	// 00_0000
	LangC_NodeFlags_Signed = 1,
	LangC_NodeFlags_Unsigned = 2,
	LangC_NodeFlags_Long = 4,
	LangC_NodeFlags_LongLong = 8,
	LangC_NodeFlags_Short = 16,
	LangC_NodeFlags_Complex = 32,
	
	// 000
	LangC_NodeFlags_GccAsmVolatile = 1,
	LangC_NodeFlags_GccAsmInline = 2,
	LangC_NodeFlags_GccAsmGoto = 4,
}
typedef LangC_NodeFlags;

internal const LangC_NodeKind LangC_token_to_op[LangC_TokenKind__Count] = {
	[LangC_TokenKind_Comma] = LangC_NodeKind_Expr2Comma,
	
	[LangC_TokenKind_Assign] = LangC_NodeKind_Expr2Assign,
	[LangC_TokenKind_PlusAssign] = LangC_NodeKind_Expr2AssignAdd,
	[LangC_TokenKind_MinusAssign] = LangC_NodeKind_Expr2AssignSub,
	[LangC_TokenKind_MulAssign] = LangC_NodeKind_Expr2AssignMul,
	[LangC_TokenKind_DivAssign] = LangC_NodeKind_Expr2AssignDiv,
	[LangC_TokenKind_LeftShiftAssign] = LangC_NodeKind_Expr2AssignLeftShift,
	[LangC_TokenKind_RightShiftAssign] = LangC_NodeKind_Expr2AssignRightShift,
	[LangC_TokenKind_AndAssign] = LangC_NodeKind_Expr2AssignAnd,
	[LangC_TokenKind_OrAssign] = LangC_NodeKind_Expr2AssignOr,
	[LangC_TokenKind_XorAssign] = LangC_NodeKind_Expr2AssignXor,
	
	[LangC_TokenKind_QuestionMark] = LangC_NodeKind_Expr3Condition,
	
	[LangC_TokenKind_LOr] = LangC_NodeKind_Expr2LogicalOr,
	[LangC_TokenKind_LAnd] = LangC_NodeKind_Expr2LogicalAnd,
	[LangC_TokenKind_Or] = LangC_NodeKind_Expr2Or,
	[LangC_TokenKind_Xor] = LangC_NodeKind_Expr2Xor,
	[LangC_TokenKind_And] = LangC_NodeKind_Expr2And,
	
	[LangC_TokenKind_Equals] = LangC_NodeKind_Expr2Equals,
	[LangC_TokenKind_NotEquals] = LangC_NodeKind_Expr2NotEquals,
	
	[LangC_TokenKind_LThan] = LangC_NodeKind_Expr2LThan,
	[LangC_TokenKind_GThan] = LangC_NodeKind_Expr2GThan,
	[LangC_TokenKind_LEqual] = LangC_NodeKind_Expr2LEqual,
	[LangC_TokenKind_GEqual] = LangC_NodeKind_Expr2GEqual,
	
	[LangC_TokenKind_LeftShift] = LangC_NodeKind_Expr2LeftShift,
	[LangC_TokenKind_RightShift] = LangC_NodeKind_Expr2RightShift,
	
	[LangC_TokenKind_Plus] = LangC_NodeKind_Expr2Add,
	[LangC_TokenKind_Minus] = LangC_NodeKind_Expr2Sub,
	
	[LangC_TokenKind_Mul] = LangC_NodeKind_Expr2Mul,
	[LangC_TokenKind_Div] = LangC_NodeKind_Expr2Div,
	[LangC_TokenKind_Mod] = LangC_NodeKind_Expr2Mod,
};

struct LangC_SymbolStack
{
	LangC_SymbolStack* up; // NOTE(ljre): One level above
	LangC_SymbolStack* next; // NOTE(ljre): In the same level
	LangC_SymbolStack* down; // NOTE(ljre): One level deeper
	LangC_Symbol* symbols;
};

enum LangC_SymbolKind
{
	LangC_SymbolKind_Null = 0,
	
	LangC_SymbolKind_Var,
	LangC_SymbolKind_VarDecl,
	LangC_SymbolKind_StaticVar,
	LangC_SymbolKind_Function,
	LangC_SymbolKind_FunctionDecl,
	LangC_SymbolKind_Parameter,
	LangC_SymbolKind_Field,
	LangC_SymbolKind_EnumConstant,
	
	LangC_SymbolKind_Typename,
	
	LangC_SymbolKind__OwnNamespace,
	LangC_SymbolKind_Struct = LangC_SymbolKind__OwnNamespace,
	LangC_SymbolKind_Union,
	LangC_SymbolKind_Enum,
}
typedef LangC_SymbolKind;

struct LangC_Symbol
{
	LangC_Symbol* next;
	
	LangC_Node* type;
	LangC_Node* decl;
	
	String name;
	uint64 name_hash;
	
	uintsize size;
	LangC_SymbolKind kind; // 4 bytes of padding :(
	
	union
	{
		// NOTE(ljre): For functions.
		struct
		{
			LangC_SymbolStack* locals;
		};
		
		// NOTE(ljre): For struct/union
		struct
		{
			LangC_SymbolStack* fields;
			uintsize alignment_mask;
		};
		
		// NOTE(ljre): for enum
		struct
		{
			LangC_SymbolStack* entries;
		};
		
		// NOTE(ljre): for enum constant
		struct
		{
			int32 value;
		};
		
		// NOTE(ljre): For struct/union field
		struct
		{
			uintsize offset;
		};
	};
};

struct LangC_Node
{
	LangC_NodeKind kind;
	LangC_Node* next;
	int32 line, col;
	LangC_LexerFile* lexfile;
	uint64 flags;
	
	String name;
	String leading_spaces;
	LangC_Node* type;
	LangC_Node* attributes;
	
	// for (init; expr; iter) stmt
	// if (expr) stmt else stmt2
	// left = right
	// left[right]
	// left(right, right->next, ...)
	// *expr
	// expr++
	// left ? middle : right
	// (type)expr
	// expr;
	// { stmt stmt->next ... }
	// type name[type->expr];
	// type name = expr;
	// type name: attributes;
	// type name(type->params, type->params->next, ...) body
	// sizeof expr
	// struct name body
	// enum { body->name = body->expr, body->next->name, ... }
	// name: stmt
	// case expr: stmt
	// (type) init
	// { .name[middle] = right, [left].middle = right, expr, [left][middle] = right, }
	// left.name
	// __asm__ flags ( leafs[0] : leafs[1] : leafs[2] : leafs[3] );
	union
	{
		struct
		{
			union { LangC_Node* stmt,  * left,   * body; };
			union { LangC_Node* stmt2, * middle, * params, *init; };
			union { LangC_Node* expr,  * right; };
			union { LangC_Node* iter; };
		};
		
		LangC_Node* leafs[5];
	};
	
	union
	{
		int64 value_int;
		uint64 value_uint;
		String value_str;
		float value_float;
		double value_double;
	};
	
	// NOTE(ljre): Data used after parsing stage
	LangC_Symbol* symbol;
	bool32 cannot_be_evaluated_at_compile_time;
	uint64 size; // size of type
	uint64 length; // length for array types
	uint64 alignment_mask; // alignment requirements (only for types)
};

enum LangC_Warning
{
	LangC_Warning_Null = 0,
	
	LangC_Warning_ImplicitInt, // implicit type "int" in declarations
	LangC_Warning_RegisterIgnored, // "register" keyword ignored for now
	LangC_Warning_ImplicitLengthOf1, // when a global array has no length, it has a length of 1
	LangC_Warning_UserWarning, // #warning directive
	
	LangC_Warning__Count,
}
typedef LangC_Warning;

#define LangC_MAX_INCLUDE_DIRS 64

struct LangC_CompilerOptions
{
	String include_dirs[LangC_MAX_INCLUDE_DIRS];
	int32 include_dirs_count;
	
	uint64 enabled_warnings[(LangC_Warning__Count + 63) / 64];
}
typedef LangC_CompilerOptions;

struct LangC_QueuedWarning typedef LangC_QueuedWarning;
struct LangC_QueuedWarning
{
	LangC_QueuedWarning* next;
	const char* to_print;
	LangC_Warning warning;
};

// NOTE(ljre): This type is only used during parsing to help identify declarations.
//             Proper type and symbol resolution occurs after the parsing stage.
struct LangC_TypeNode typedef LangC_TypeNode;
struct LangC_TypeNode
{
	LangC_TypeNode* next;
	LangC_TypeNode* previous;
	LangC_TypeNode* saved_last;
	
	uint64 name_hash;
	String name;
};

struct LangC_Macro typedef LangC_Macro;
struct LangC_Macro
{
	LangC_Macro* next;
	LangC_Macro* previous;
	
	const char* def;
	String name;
	bool8 is_func_like;
	bool8 expanding;
	bool8 persistent; // cant #undef it if true
};

struct LangC_MacroParameter
{
	String name;
	const char* expands_to;
}
typedef LangC_MacroParameter;

struct LangC_PPLoadedFile typedef LangC_PPLoadedFile;
struct LangC_PPLoadedFile
{
	LangC_PPLoadedFile* next;
	uint64 hash;
	const char* contents;
	String path;
	bool8 relative;
	bool8 pragma_onced;
};

struct LangC_Preprocessor
{
	LangC_Macro* first_macro;
	LangC_Macro* last_macro;
	
	LangC_PPLoadedFile* loaded_files;
}
typedef LangC_Preprocessor;

struct LangC_ABIType
{
	uint16 size;
	uint8 alignment_mask;
	bool8 unsig;
}
typedef LangC_ABIType;

struct LangC_ABI
{
	union
	{
		// NOTE(ljre): You *can* assume that 'index+1' is 'index' but unsigned, but only for types that allow it.
		
		struct
		{
			LangC_ABIType t_bool;
			LangC_ABIType t_char;
			LangC_ABIType t_uchar;
			LangC_ABIType t_schar;
			LangC_ABIType t_short;
			LangC_ABIType t_ushort;
			LangC_ABIType t_int;
			LangC_ABIType t_uint;
			LangC_ABIType t_long;
			LangC_ABIType t_ulong;
			LangC_ABIType t_longlong;
			LangC_ABIType t_ulonglong;
			LangC_ABIType t_float;
			LangC_ABIType t_double;
			LangC_ABIType t_ptr;
		};
		
		LangC_ABIType t[15];
	};
	
	uint8 char_bit;
	int8 index_sizet;
	int8 index_ptrdifft;
}
typedef LangC_ABI;

struct LangC_Context
{
	LangC_CompilerOptions* options;
	Arena* persistent_arena; // NOTE(ljre): Permanent arena that will live through various stages.
	Arena* stage_arena; // NOTE(ljre): This arena is used for a single stage.
	
	LangC_Lexer lex;
	LangC_Preprocessor pp;
	LangC_ABI* abi;
	
	const char* source; // NOTE(ljre): source code *before* preprocessing.
	const char* pre_source; // NOTE(ljre): source code *after* preprocessing.
	
	// NOTE(ljre): Those nodes live in 'stage_arena' during parsing.
	LangC_TypeNode* first_typenode;
	LangC_TypeNode* last_typenode;
	
	// NOTE(ljre): Those nodes live in 'persistent_arena' and are generated by the parser.
	LangC_Node* ast;
	
	LangC_SymbolStack* symbol_stack;
	LangC_SymbolStack* previous_symbol_stack;
	
	// NOTE(ljre): Warning list
	LangC_QueuedWarning* queued_warning;
	LangC_QueuedWarning* last_queued_warning;
	
	// NOTE(ljre): For finding the host statement of 'break' and 'continue'
	LangC_Node* host_switch;
	LangC_Node* host_break;
	LangC_Node* host_continue;
	
	bool8 use_stage_arena_for_warnings;
};

#endif //LANG_C_DEFINITIONS_H
