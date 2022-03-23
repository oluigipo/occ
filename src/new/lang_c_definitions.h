#ifndef LANG_C_DEFINITIONS_H
#define LANG_C_DEFINITIONS_H

#define C_IsKeyword(kind) ((kind) >= C_TokenKind__FirstKeyword && (kind) <= C_TokenKind__LastKeyword)

struct C_Macro typedef C_Macro;

enum C_TokenKind
{
	C_TokenKind_Eof = 0,
	
	//- NOTE(ljre): Keywords
	C_TokenKind__FirstKeyword,
	C_TokenKind_Auto = C_TokenKind__FirstKeyword,
	C_TokenKind_Break,
	C_TokenKind_Case,
	C_TokenKind_Char,
	C_TokenKind_Const,
	C_TokenKind_Continue,
	C_TokenKind_Default,
	C_TokenKind_Do,
	C_TokenKind_Double,
	C_TokenKind_Else,
	C_TokenKind_Enum,
	C_TokenKind_Extern,
	C_TokenKind_Float,
	C_TokenKind_For,
	C_TokenKind_Goto,
	C_TokenKind_If,
	C_TokenKind_Inline,
	C_TokenKind_Int,
	C_TokenKind_Long,
	C_TokenKind_Register,
	C_TokenKind_Restrict,
	C_TokenKind_Return,
	C_TokenKind_Short,
	C_TokenKind_Signed,
	C_TokenKind_Sizeof,
	C_TokenKind_Static,
	C_TokenKind_Struct,
	C_TokenKind_Switch,
	C_TokenKind_Typedef,
	C_TokenKind_Union,
	C_TokenKind_Unsigned,
	C_TokenKind_Void,
	C_TokenKind_Volatile,
	C_TokenKind_While,
	C_TokenKind_Bool,
	C_TokenKind_Complex,
	
	// NOTE(ljre): GCC stuff
	C_TokenKind_GccAttribute, // __attribute __attribute__
	C_TokenKind_GccAsm, // __asm__ asm
	C_TokenKind_GccExtension, // __extension__
	C_TokenKind_GccTypeof, // __typeof__
	C_TokenKind_GccAutoType, // __auto_type
	
	// NOTE(ljre): MSVC stuff
	C_TokenKind_MsvcDeclspec, // __declspec
	C_TokenKind_MsvcAsm, // __asm
	C_TokenKind_MsvcForceinline, // __forceinline
	C_TokenKind_MsvcCdecl, // __cdecl
	C_TokenKind_MsvcStdcall, // __stdcall
	C_TokenKind_MsvcVectorcall, // __vectorcall
	C_TokenKind_MsvcFastcall, // __fastcall
	C_TokenKind_MsvcPragma, // __pragma
	
	C_TokenKind__LastKeyword = C_TokenKind_MsvcPragma,
	
	//- NOTE(ljre): Rest
	// NOTE(ljre): those 3 below shall be in order
	C_TokenKind_IntLiteral,
	C_TokenKind_LIntLiteral,
	C_TokenKind_LLIntLiteral,
	
	// NOTE(ljre): same thing for those 3
	C_TokenKind_UintLiteral,
	C_TokenKind_LUintLiteral,
	C_TokenKind_LLUintLiteral,
	
	C_TokenKind_StringLiteral,
	C_TokenKind_WideStringLiteral,
	C_TokenKind_FloatLiteral,
	C_TokenKind_DoubleLiteral,
	C_TokenKind_CharLiteral,
	
	C_TokenKind_UnclosedQuote, // NOTE(ljre): Error state. Needed because #warning what's up
	
	C_TokenKind_Identifier,
	
	C_TokenKind_LeftParen, // (
	C_TokenKind_RightParen, // )
	C_TokenKind_LeftBrkt, // [
	C_TokenKind_RightBrkt, // ]
	C_TokenKind_LeftCurl, // {
	C_TokenKind_RightCurl, // }
	C_TokenKind_Dot, // .
	C_TokenKind_VarArgs, // ...
	C_TokenKind_Arrow, // ->
	C_TokenKind_Comma, // ,
	C_TokenKind_Colon, // :
	C_TokenKind_Semicolon, // ;
	C_TokenKind_QuestionMark, // ?
	C_TokenKind_Plus, // +
	C_TokenKind_Minus, // -
	C_TokenKind_Mul, // *
	C_TokenKind_Div, // /
	C_TokenKind_Mod, // %
	C_TokenKind_LThan, // <
	C_TokenKind_GThan, // >
	C_TokenKind_LEqual, // <=
	C_TokenKind_GEqual, // >=
	C_TokenKind_Equals, // ==
	C_TokenKind_Inc, // ++
	C_TokenKind_Dec, // --
	C_TokenKind_LNot, // !
	C_TokenKind_LAnd, // &&
	C_TokenKind_LOr, // ||
	C_TokenKind_Not, // ~
	C_TokenKind_And, // &
	C_TokenKind_Or, // |
	C_TokenKind_Xor, // ^
	C_TokenKind_LeftShift, // <<
	C_TokenKind_RightShift, // >>
	C_TokenKind_NotEquals, // !=
	C_TokenKind_Assign, // =
	C_TokenKind_PlusAssign, // +=
	C_TokenKind_MinusAssign, // -=
	C_TokenKind_MulAssign, // *=
	C_TokenKind_DivAssign, // /=
	C_TokenKind_ModAssign, // %=
	C_TokenKind_LeftShiftAssign, // <<=
	C_TokenKind_RightShiftAssign, // >>=
	C_TokenKind_AndAssign, // &=
	C_TokenKind_OrAssign, // |=
	C_TokenKind_XorAssign, // ^=
	
	C_TokenKind_Hashtag, // #
	C_TokenKind_HashtagPragma, // #pragma
	C_TokenKind_NewLine,
	C_TokenKind_DoubleHashtag, // ##
	
	C_TokenKind__Count,
}
typedef C_TokenKind;

internal const char* C_token_str_table[C_TokenKind__Count] = {
	[C_TokenKind_Eof] = "(EOF)",
	
	[C_TokenKind_Auto] = "auto",
	[C_TokenKind_Break] = "break",
	[C_TokenKind_Case] = "case",
	[C_TokenKind_Char] = "char",
	[C_TokenKind_Const] = "const",
	[C_TokenKind_Continue] = "continue",
	[C_TokenKind_Default] = "default",
	[C_TokenKind_Do] = "do",
	[C_TokenKind_Double] = "double",
	[C_TokenKind_Else] = "else",
	[C_TokenKind_Enum] = "enum",
	[C_TokenKind_Extern] = "extern",
	[C_TokenKind_Float] = "float",
	[C_TokenKind_For] = "for",
	[C_TokenKind_Goto] = "goto",
	[C_TokenKind_If] = "if",
	[C_TokenKind_Inline] = "inline",
	[C_TokenKind_Int] = "int",
	[C_TokenKind_Long] = "long",
	[C_TokenKind_Register] = "register",
	[C_TokenKind_Restrict] = "restrict",
	[C_TokenKind_Return] = "return",
	[C_TokenKind_Short] = "short",
	[C_TokenKind_Signed] = "signed",
	[C_TokenKind_Sizeof] = "sizeof",
	[C_TokenKind_Static] = "static",
	[C_TokenKind_Struct] = "struct",
	[C_TokenKind_Switch] = "switch",
	[C_TokenKind_Typedef] = "typedef",
	[C_TokenKind_Union] = "union",
	[C_TokenKind_Unsigned] = "unsigned",
	[C_TokenKind_Void] = "void",
	[C_TokenKind_Volatile] = "volatile",
	[C_TokenKind_While] = "while",
	[C_TokenKind_Bool] = "_Bool",
	[C_TokenKind_Complex] = "_Complex",
	
	[C_TokenKind_GccAttribute] = "__attribute__",
	[C_TokenKind_GccAsm] = "__asm__",
	[C_TokenKind_GccExtension] = "__extension__",
	[C_TokenKind_GccTypeof] = "__typeof__",
	[C_TokenKind_GccAutoType] = "__auto_type",
	
	[C_TokenKind_MsvcDeclspec] = "__declspec",
	[C_TokenKind_MsvcAsm] = "__asm",
	[C_TokenKind_MsvcForceinline] = "__forceinline",
	[C_TokenKind_MsvcCdecl] = "__cdecl",
	[C_TokenKind_MsvcStdcall] = "__stdcall",
	[C_TokenKind_MsvcVectorcall] = "__vectorcall",
	[C_TokenKind_MsvcFastcall] = "__fastcall",
	[C_TokenKind_MsvcPragma] = "__pragma",
	
	[C_TokenKind_IntLiteral] = "(int literal)",
	[C_TokenKind_LIntLiteral] = "(long literal)",
	[C_TokenKind_LLIntLiteral] = "(long long literal)",
	[C_TokenKind_UintLiteral] = "(unsigned literal)",
	[C_TokenKind_LUintLiteral] = "(unsigned long literal)",
	[C_TokenKind_LLUintLiteral] = "(unsigned long long literal)",
	[C_TokenKind_StringLiteral] = "(const char[] literal)",
	[C_TokenKind_WideStringLiteral] = "(const wchar_t[] literal)",
	[C_TokenKind_FloatLiteral] = "(float literal)",
	[C_TokenKind_DoubleLiteral] = "(double literal)",
	[C_TokenKind_CharLiteral] = "(char literal)",
	[C_TokenKind_UnclosedQuote] = "(unclosed quote)",
	
	[C_TokenKind_Identifier] = "(identifier)",
	
	[C_TokenKind_LeftParen] = "(",
	[C_TokenKind_RightParen] = ")",
	[C_TokenKind_LeftBrkt] = "[",
	[C_TokenKind_RightBrkt] = "]",
	[C_TokenKind_LeftCurl] = "{",
	[C_TokenKind_RightCurl] = "}",
	[C_TokenKind_Dot] = ".",
	[C_TokenKind_VarArgs] = "...",
	[C_TokenKind_Arrow] = "->",
	[C_TokenKind_Comma] = ",",
	[C_TokenKind_Colon] = ":",
	[C_TokenKind_Semicolon] = ";",
	[C_TokenKind_QuestionMark] = "?",
	[C_TokenKind_Plus] = "+",
	[C_TokenKind_Minus] = "-",
	[C_TokenKind_Mul] = "*",
	[C_TokenKind_Div] = "/",
	[C_TokenKind_Mod] = "%",
	[C_TokenKind_LThan] = "<",
	[C_TokenKind_GThan] = ">",
	[C_TokenKind_LEqual] = "<=",
	[C_TokenKind_GEqual] = ">=",
	[C_TokenKind_Equals] = "==",
	[C_TokenKind_Inc] = "++",
	[C_TokenKind_Dec] = "--",
	[C_TokenKind_LNot] = "!",
	[C_TokenKind_LAnd] = "&&",
	[C_TokenKind_LOr] = "||",
	[C_TokenKind_Not] = "~",
	[C_TokenKind_And] = "&",
	[C_TokenKind_Or] = "|",
	[C_TokenKind_Xor] = "^",
	[C_TokenKind_LeftShift] = "<<",
	[C_TokenKind_RightShift] = ">>",
	[C_TokenKind_NotEquals] = "!=",
	[C_TokenKind_Assign] = "=",
	[C_TokenKind_PlusAssign] = "+=",
	[C_TokenKind_MinusAssign] = "-=",
	[C_TokenKind_MulAssign] = "*=",
	[C_TokenKind_DivAssign] = "/=",
	[C_TokenKind_ModAssign] = "%=",
	[C_TokenKind_LeftShiftAssign] = "<<=",
	[C_TokenKind_RightShiftAssign] = ">>=",
	[C_TokenKind_AndAssign] = "&=",
	[C_TokenKind_OrAssign] = "|=",
	[C_TokenKind_XorAssign] = "^=",
	
	[C_TokenKind_NewLine] = "(EOL)",
	[C_TokenKind_DoubleHashtag] = "##",
	[C_TokenKind_Hashtag] = "#",
	[C_TokenKind_HashtagPragma] = "#pragma",
};

struct C_OperatorPrecedence
{
	// from lower to higher
	int8 level; // if <= 0, it's not a binary operator
	bool8 right2left;
}
typedef C_OperatorPrecedence;

internal C_OperatorPrecedence C_operators_precedence[C_TokenKind__Count] = {
	[C_TokenKind_Comma] = { 1, false, },
	
	[C_TokenKind_Assign] = { 2, true, },
	[C_TokenKind_PlusAssign] = { 2, true, },
	[C_TokenKind_MinusAssign] = { 2, true, },
	[C_TokenKind_MulAssign] = { 2, true, },
	[C_TokenKind_DivAssign] = { 2, true, },
	[C_TokenKind_ModAssign] = { 2, true, },
	[C_TokenKind_LeftShiftAssign] = { 2, true, },
	[C_TokenKind_RightShiftAssign] = { 2, true, },
	[C_TokenKind_AndAssign] = { 2, true, },
	[C_TokenKind_OrAssign] = { 2, true, },
	[C_TokenKind_XorAssign] = { 2, true, },
	
	[C_TokenKind_QuestionMark] = { 3, true, },
	
	[C_TokenKind_LOr] = { 3, false, },
	[C_TokenKind_LAnd] = { 4, false, },
	[C_TokenKind_Or] = { 5, false, },
	[C_TokenKind_Xor] = { 6, false, },
	[C_TokenKind_And] = { 7, false, },
	
	[C_TokenKind_Equals] = { 8, false, },
	[C_TokenKind_NotEquals] = { 8, false, },
	
	[C_TokenKind_LThan] = { 9, false, },
	[C_TokenKind_GThan] = { 9, false, },
	[C_TokenKind_LEqual] = { 9, false, },
	[C_TokenKind_GEqual] = { 9, false, },
	
	[C_TokenKind_LeftShift] = { 10, false, },
	[C_TokenKind_RightShift] = { 10, false, },
	
	[C_TokenKind_Plus] = { 11, false, },
	[C_TokenKind_Minus] = { 11, false, },
	
	[C_TokenKind_Mul] = { 12, false, },
	[C_TokenKind_Div] = { 12, false, },
	[C_TokenKind_Mod] = { 12, false, },
};

struct C_SourceFileTrace typedef C_SourceFileTrace;
struct C_SourceFileTrace
{
	String path;
	
	C_SourceFileTrace* included_from;
	uint32 included_line;
	bool32 is_system_file;
};

struct C_SourceTrace typedef C_SourceTrace;
struct C_SourceTrace
{
	// NOTE(ljre): *IMPORTANT*
	//             These start at 0. They have a -1 bias. Because ZII.
	uint32 line, col;
	C_SourceFileTrace* file;
	
	// NOTE(ljre): If 'invocation' is not NULL but 'macro_def' is, then 'invocation' came from a
	//             parameter of 'invocation->macro_def'.
	C_SourceTrace* invocation;
	C_Macro* macro_def;
};

struct C_Token
{
	C_TokenKind kind; // 4 bytes
	
	// NOTE(ljre): How many whitespaces there are after this token.
	uint32 leading_spaces;
	
	C_SourceTrace trace;
	String as_string;
}
typedef C_Token;

struct C_TokenSlice
{
	uintsize size;
	C_Token* data;
}
typedef C_TokenSlice;

struct C_TokenQueue typedef C_TokenQueue;
struct C_TokenQueue
{
	C_TokenQueue* next;
	C_Token token;
};

struct C_TokenReader
{
	C_Token* head;
	
	C_TokenQueue* queue;
	C_Token* slice_head;
	C_Token* slice_end;
	
	C_SourceFileTrace* file_trace;
}
typedef C_TokenReader;

//~ NOTE(ljre): Preprocessor
struct C_Macro
{
	String name;
	C_TokenSlice tokens;
	
	int32 param_count; // if < 0, then it's an object-like macro.
	
	uint32 line, col;
	C_SourceFileTrace* file;
}
typedef C_Macro;

struct C_PpLoadedFile
{
	String fullpath;
	const char* contents;
	C_TokenSlice tokens; // NOTE(ljre): Lazily set
	
	bool16 relative;
	bool8 pragma_onced;
	bool8 is_system_file;
}
typedef C_PpLoadedFile;

struct C_Preprocessor
{
	Map* func_macros;
	Map* obj_macros;
	Map* predefined_macros;
	
	Map* loaded_files;
	
	C_Token* out;
}
typedef C_Preprocessor;

//~ NOTE(ljre): Parser
enum C_AstKind
{
	C_AstKind_Null = 0,
	C_AstKind__Category = 12,
	C_AstKind__CategoryMask = ~((1<<C_AstKind__Category)-1),
	C_AstKind__CategoryCount = 8,
	
	C_AstKind_Type = 1 << C_AstKind__Category,
	C_AstKind_TypeBaseChar,
	C_AstKind_TypeBaseInt,
	C_AstKind_TypeBaseFloat,
	C_AstKind_TypeBaseDouble,
	C_AstKind_TypeBaseVoid,
	C_AstKind_TypeBaseBool,
	C_AstKind_TypeBaseTypename,
	C_AstKind_TypeBaseStruct,
	C_AstKind_TypeBaseUnion,
	C_AstKind_TypeBaseEnum,
	C_AstKind_TypeFunction,
	C_AstKind_TypePointer,
	C_AstKind_TypeArray,
	
	C_AstKind_Decl = 2 << C_AstKind__Category,
	C_AstKind_DeclStatic,
	C_AstKind_DeclExtern,
	C_AstKind_DeclAuto,
	C_AstKind_DeclTypedef,
	C_AstKind_DeclRegister,
	
	C_AstKind_Stmt = 3 << C_AstKind__Category,
	C_AstKind_StmtEmpty,
	C_AstKind_StmtExpr,
	C_AstKind_StmtIf,
	C_AstKind_StmtDoWhile,
	C_AstKind_StmtWhile,
	C_AstKind_StmtFor,
	C_AstKind_StmtSwitch,
	C_AstKind_StmtReturn,
	C_AstKind_StmtGoto,
	C_AstKind_StmtCompound,
	C_AstKind_StmtLabel,
	C_AstKind_StmtCase,
	C_AstKind_StmtBreak,
	C_AstKind_StmtContinue,
	C_AstKind_StmtGccAsm,
	
	C_AstKind_ExprFactor = 4 << C_AstKind__Category,
	C_AstKind_ExprIdent,
	C_AstKind_ExprInt,
	C_AstKind_ExprLInt,
	C_AstKind_ExprLLInt,
	C_AstKind_ExprUInt,
	C_AstKind_ExprULInt,
	C_AstKind_ExprULLInt,
	C_AstKind_ExprFloat,
	C_AstKind_ExprDouble,
	C_AstKind_ExprString,
	C_AstKind_ExprWideString,
	C_AstKind_ExprChar,
	C_AstKind_ExprCompoundLiteral,
	C_AstKind_ExprInitializer,
	
	C_AstKind_Expr1 = 5 << C_AstKind__Category,
	C_AstKind_Expr1Positive,
	C_AstKind_Expr1Negative,
	C_AstKind_Expr1Not,
	C_AstKind_Expr1LogicalNot,
	C_AstKind_Expr1Deref,
	C_AstKind_Expr1Ref,
	C_AstKind_Expr1PrefixInc,
	C_AstKind_Expr1PrefixDec,
	C_AstKind_Expr1PostfixInc,
	C_AstKind_Expr1PostfixDec,
	C_AstKind_Expr1Sizeof,
	C_AstKind_Expr1SizeofType,
	C_AstKind_Expr1Cast,
	
	C_AstKind_Expr2 = 6 << C_AstKind__Category,
	C_AstKind_Expr2Add,
	C_AstKind_Expr2Sub,
	C_AstKind_Expr2Mul,
	C_AstKind_Expr2Div,
	C_AstKind_Expr2Mod,
	C_AstKind_Expr2LThan,
	C_AstKind_Expr2GThan,
	C_AstKind_Expr2LEqual,
	C_AstKind_Expr2GEqual,
	C_AstKind_Expr2Equals,
	C_AstKind_Expr2NotEquals,
	C_AstKind_Expr2LeftShift,
	C_AstKind_Expr2RightShift,
	C_AstKind_Expr2And,
	C_AstKind_Expr2Or,
	C_AstKind_Expr2Xor,
	C_AstKind_Expr2LogicalAnd,
	C_AstKind_Expr2LogicalOr,
	C_AstKind_Expr2Assign,
	C_AstKind_Expr2AssignAdd,
	C_AstKind_Expr2AssignSub,
	C_AstKind_Expr2AssignMul,
	C_AstKind_Expr2AssignDiv,
	C_AstKind_Expr2AssignMod,
	C_AstKind_Expr2AssignLeftShift,
	C_AstKind_Expr2AssignRightShift,
	C_AstKind_Expr2AssignAnd,
	C_AstKind_Expr2AssignOr,
	C_AstKind_Expr2AssignXor,
	C_AstKind_Expr2Comma,
	C_AstKind_Expr2Call,
	C_AstKind_Expr2Index,
	C_AstKind_Expr2Access,
	C_AstKind_Expr2DerefAccess,
	
	C_AstKind_Expr3 = 7 << C_AstKind__Category,
	C_AstKind_Expr3Condition,
}
typedef C_AstKind;

enum C_AstFlags
{
	C_AstFlags_Poisoned = (int32)(1u << 31),
	C_AstFlags_Implicit = 1 << 30,
	C_AstFlags_ComptimeKnown = 1 << 29,
	C_AstFlags_Decayed = 1 << 28,
	
	// NOTE(ljre): For C_AstKind_Type*
	C_AstFlags_Volatile = 1 << 10,
	C_AstFlags_Restrict = 1 << 9,
	C_AstFlags_Const = 1 << 8,
	
	// NOTE(ljre): For C_AstKind_Decl*
	C_AstFlags_Inline = 1 << 0,
	
	// NOTE(ljre): For C_AstKind_TypeFunction
	C_AstFlags_VarArgs = 1 << 0,
	
	// NOTE(ljre): For C_AstKind_TypeBase*
	C_AstFlags_Signed = 1 << 0,
	C_AstFlags_Unsigned = 1 << 1,
	C_AstFlags_Long = 1 << 2,
	C_AstFlags_LongLong = 1 << 3,
	C_AstFlags_Short = 1 << 4,
	C_AstFlags_Complex = 1 << 5,
	
	// NOTE(ljre): For C_AstKind_GccAsm
	C_AstFlags_GccAsmVolatile = 1 << 0,
	C_AstFlags_GccAsmInline = 1 << 1,
	C_AstFlags_GccAsmGoto = 1 << 2,
	
	// NOTE(ljre): For ???
	C_AstFlags_MsvcCdecl = 1 << 0,
	C_AstFlags_MsvcStdcall = 1 << 1,
	C_AstFlags_MsvcVectorcall = 1 << 2,
	C_AstFlags_MsvcFastcall = 1 << 3,
};

struct C_AstNode typedef C_AstNode;
struct C_AstDecl typedef C_AstDecl;
struct C_AstType typedef C_AstType;
struct C_AstExpr typedef C_AstExpr;
struct C_AstStmt typedef C_AstStmt;

#define C_NODE_HEADER(Name,...) \
struct __VA_ARGS__\
{\
C_AstKind kind;\
uint32 flags;\
Name* next;\
C_SourceTrace* trace;\
}

C_NODE_HEADER(C_AstNode, C_AstNode);

struct C_AstDecl
{
	union
	{
		C_AstNode header;
		C_NODE_HEADER(C_AstDecl);
	};
	
	String name;
	C_AstType* type;
	
	union
	{
		C_AstExpr* init;
		C_AstStmt* body;
	};
};

struct C_AstType
{
	union
	{
		C_AstNode header;
		C_NODE_HEADER(C_AstDecl);
	};
	
	uint64 size;
	uint32 alignment;
	bool32 is_unsigned;
	
	union
	{
		C_AstType* base;
		
		struct { C_AstType* of; C_AstExpr* length; } array;
		C_AstType* ptr;
		struct { C_AstType* ret; C_AstDecl* params; } function;
		
		struct { C_AstDecl* body; String name; } structure;
		struct { C_AstType* base; String name; } typedefed;
		struct { C_AstDecl* entries; String name; } enumerator;
	};
};

struct C_AstStmt
{
	union
	{
		C_AstNode header;
		C_NODE_HEADER(C_AstDecl);
	};
	
	union
	{
		C_AstStmt* compound;
		C_AstExpr* expr;
		String go_to;
		
		struct { C_AstStmt* stmt; String name; } label;
		
		struct
		{
			C_AstExpr* code;
			C_AstExpr* outputs;
			C_AstExpr* inputs;
			C_AstExpr* clobbers;
			C_AstExpr* goto_labels;
		}
		gcc_asm;
	};
};

struct C_AstExpr
{
	union
	{
		C_AstNode header;
		C_NODE_HEADER(C_AstDecl);
	};
	
	union
	{
		uint64 value_uint;
		int64 value_int;
		String value_str;
		float value_float;
		double value_double;
		
		C_AstExpr* unary;
		C_AstExpr* init;
		struct { C_AstExpr* left, * right; } binary;
		struct { C_AstExpr* left, * middle, * right; } ternary;
		struct { C_AstExpr* expr; String field; } access;
		struct { C_AstExpr* expr; C_AstType* to; } cast;
		struct { C_AstExpr* init; C_AstType* type; } compound;
		C_AstExpr* sizeof_expr;
		C_AstType* sizeof_type;
		
		struct { C_AstExpr* designated; C_AstExpr* expr; } init_entry;
		String desig_field;
		C_AstExpr* desig_index;
	};
};

#undef C_NODE_HEADER

struct C_ParserScope typedef C_ParserScope;
struct C_ParserScope
{
	C_ParserScope* up;
	Map* typedefed;
};

struct C_Parser
{
	C_TokenReader rd;
	
	C_ParserScope* scope;
	
}
typedef C_Parser;

//~ NOTE(ljre): ABI Information
struct C_ABIType
{
	uint16 size;
	uint8 alignment;
	bool8 is_unsigned;
}
typedef C_ABIType;

struct C_ABI
{
	union
	{
		struct
		{
			C_ABIType t_bool;
			C_ABIType t_schar;
			C_ABIType t_char;
			C_ABIType t_uchar;
			C_ABIType t_short;
			C_ABIType t_ushort;
			C_ABIType t_int;
			C_ABIType t_uint;
			C_ABIType t_long;
			C_ABIType t_ulong;
			C_ABIType t_longlong;
			C_ABIType t_ulonglong;
			C_ABIType t_float;
			C_ABIType t_double;
			C_ABIType t_ptr;
		};
		
		C_ABIType t[15];
	};
	
	uint8 char_bit;
	int8 index_sizet;
	int8 index_ptrdifft;
}
typedef C_ABI;

//~ NOTE(ljre): Compiler Context
enum C_Warning
{
	C_Warning_Null = 0,
	
	C_Warning_ImplicitInt,
	C_Warning_MissingMacroArguments, // when macro is invoked with less arguments than it was declared with
	C_Warning_ExcessiveMacroArguments,
	
	C_Warning__Count,
}
typedef C_Warning;

struct C_CompilerOptions
{
	uint64 warnings[(C_Warning__Count + 63) / 64];
	
	const String* include_dirs;
	uintsize include_dirs_count;
	
	const char** predefined_macros;
	uintsize predefined_macros_count;
	
	C_ABI abi;
}
typedef C_CompilerOptions;

struct alignas(64) C_Context
{
	Arena* array_arena;
	Arena* tree_arena;
	Arena* scratch_arena;
	
	const C_CompilerOptions* options;
}
typedef C_Context;

//~ NOTE(ljre): Functions
internal inline bool32
C_IsNumeric(char ch, int32 base)
{
	return (base == 2 && (ch == '0' || ch == '1') ||
			(ch >= '0' && ch <= '9' || (base == 16 && (ch >= 'a' && ch <= 'f' || ch >= 'A' && ch <= 'F'))));
}

internal inline bool32
C_IsAlpha(char ch)
{
	return (ch >= 'A' & ch <= 'Z') | (ch >= 'a' & ch <= 'z');
}

internal inline bool32
C_IsIdentChar(char ch)
{
	return ch == '_' || (ch >= '0' && ch <= '9') || C_IsAlpha(ch)
		|| (unsigned char)ch >= 128; // NOTE(ljre): This makes every multibyte UTF-8 codepoint a valid char for identifiers.
}

internal C_TokenKind
C_FindKeywordByString(String str)
{
	for (int32 keyword = C_TokenKind__FirstKeyword; keyword <= C_TokenKind__LastKeyword; ++keyword)
	{
		if (MatchCString(C_token_str_table[keyword], str))
			return keyword;
	}
	
	return C_TokenKind_Identifier;
}

internal void
C_IgnoreWhitespaces(const char** p, bool newline)
{
	for (;;)
	{
		if (!**p)
			break;
		
		// NOTE(ljre): C++-style comments
		if (**p == '/' && (*p)[1] == '/')
		{
			*p += 2;
			
			while (**p && (**p != '\n' || (*p)[-1] == '\\'))
				++*p;
		}
		// NOTE(ljre): C-style comments
		else if (**p == '/' && (*p)[1] == '*')
		{
			*p += 2;
			
			// TODO(ljre): Maybe nesting?
			while (**p && !((*p)[-2] == '*' && (*p)[-1] == '/'))
				++*p;
		}
		// NOTE(ljre): General whitespaces & newlines
		else if (**p == ' ' || **p == '\t' || (newline && **p == '\n'))
		{
			++*p;
		}
		else if (newline && **p == '\r' && (*p)[1] == '\n')
		{
			*p += 2;
		}
		// NOTE(ljre): Treating a \ followed by a newline as a whitespace. Yes, this is incorrect by the standard.
		else if (**p == '\\' && (*p)[1] == '\n')
		{
			*p += 2;
		}
		else if (**p == '\\' && (*p)[1] == '\r')
		{
			*p += 2 + ((*p)[2] == '\n');
		}
		else
		{
			break;
		}
	}
}

internal int32
C_ValueOfEscaped(const char** ptr, const char* end)
{
	Assert(*ptr < end);
	
	int32 value = 0;
	int32 chars_in_hex = 0;
	
	switch (**ptr)
	{
		case '\\': value = '\\'; ++*ptr; break;
		case '\'': value = '\''; ++*ptr; break;
		case '"': value = '"'; ++*ptr; break;
		case 'a': value = 0x07; ++*ptr; break;
		case 'b': value = 0x08; ++*ptr; break;
		case 'e': value = 0x1B; ++*ptr; break;
		case 'f': value = 0x0C; ++*ptr; break;
		case 'n': value = 0x0A; ++*ptr; break;
		case 'r': value = 0x0D; ++*ptr; break;
		case 't': value = 0x09; ++*ptr; break;
		case 'v': value = 0x0B; ++*ptr; break;
		case '?': value = '?'; ++*ptr; break;
		
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		{
			value = **ptr;
			++*ptr;
			
			int32 chars_in_octal = 2; // limit of 3 chars. one is already parsed
			
			while (*ptr < end && chars_in_octal > 0 && **ptr >= '0' && **ptr < '8')
			{
				value *= 8;
				value += **ptr;
				++*ptr;
				--chars_in_octal;
			}
		} break;
		
		/**/ if (0) case 'x': chars_in_hex = 2;
		else if (0) case 'u': chars_in_hex = 4;
		else        case 'U': chars_in_hex = 8;
		
		{
			++*ptr;
			
			while (*ptr < end && chars_in_hex --> 0 &&
				   ((**ptr >= '0' && **ptr <= '9') ||
					(**ptr >= 'a' && **ptr <= 'f') ||
					(**ptr >= 'A' && **ptr <= 'F')))
			{
				value *= 16;
				
				if (**ptr >= 'a')
					value += 10 + (**ptr - 'a');
				else if (**ptr >= 'A')
					value += 10 + (**ptr - 'A');
				else
					value += **ptr - '0';
				
				++*ptr;
			}
		} break;
		
		default: value = **ptr; ++*ptr; break;
	}
	
	return value;
}

internal C_AstKind
C_TokenToExprKind(C_TokenKind kind)
{
	switch (kind)
	{
		case C_TokenKind_Comma: return C_AstKind_Expr2Comma;
		
		case C_TokenKind_Assign: return C_AstKind_Expr2Assign;
		case C_TokenKind_PlusAssign: return C_AstKind_Expr2AssignAdd;
		case C_TokenKind_MinusAssign: return C_AstKind_Expr2AssignSub;
		case C_TokenKind_MulAssign: return C_AstKind_Expr2AssignMul;
		case C_TokenKind_DivAssign: return C_AstKind_Expr2AssignDiv;
		case C_TokenKind_LeftShiftAssign: return C_AstKind_Expr2AssignLeftShift;
		case C_TokenKind_RightShiftAssign: return C_AstKind_Expr2AssignRightShift;
		case C_TokenKind_AndAssign: return C_AstKind_Expr2AssignAnd;
		case C_TokenKind_OrAssign: return C_AstKind_Expr2AssignOr;
		case C_TokenKind_XorAssign: return C_AstKind_Expr2AssignXor;
		
		case C_TokenKind_QuestionMark: return C_AstKind_Expr3Condition;
		
		case C_TokenKind_LOr: return C_AstKind_Expr2LogicalOr;
		case C_TokenKind_LAnd: return C_AstKind_Expr2LogicalAnd;
		case C_TokenKind_Or: return C_AstKind_Expr2Or;
		case C_TokenKind_Xor: return C_AstKind_Expr2Xor;
		case C_TokenKind_And: return C_AstKind_Expr2And;
		
		case C_TokenKind_Equals: return C_AstKind_Expr2Equals;
		case C_TokenKind_NotEquals: return C_AstKind_Expr2NotEquals;
		
		case C_TokenKind_LThan: return C_AstKind_Expr2LThan;
		case C_TokenKind_GThan: return C_AstKind_Expr2GThan;
		case C_TokenKind_LEqual: return C_AstKind_Expr2LEqual;
		case C_TokenKind_GEqual: return C_AstKind_Expr2GEqual;
		
		case C_TokenKind_LeftShift: return C_AstKind_Expr2LeftShift;
		case C_TokenKind_RightShift: return C_AstKind_Expr2RightShift;
		
		case C_TokenKind_Plus: return C_AstKind_Expr2Add;
		case C_TokenKind_Minus: return C_AstKind_Expr2Sub;
		
		case C_TokenKind_Mul: return C_AstKind_Expr2Mul;
		case C_TokenKind_Div: return C_AstKind_Expr2Div;
		case C_TokenKind_Mod: return C_AstKind_Expr2Mod;
		
		default: Unreachable(); return 0;
	}
}

#endif //LANG_C_DEFINITIONS_H
