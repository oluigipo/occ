#ifndef LANG_C_DEFINITIONS_H
#define LANG_C_DEFINITIONS_H

struct C_Context typedef C_Context;
struct C_OperatorPrecedence typedef C_OperatorPrecedence;
struct C_TokenList typedef C_TokenList;
struct C_LexerFile typedef C_LexerFile;
struct C_TokenStream typedef C_TokenStream;
struct C_SourceFileTrace typedef C_SourceFileTrace;
struct C_SourceTrace typedef C_SourceTrace;
struct C_Macro typedef C_Macro;

struct C_AstNode typedef C_AstNode;
struct C_AstDecl typedef C_AstDecl;
struct C_AstType typedef C_AstType;
struct C_AstStmt typedef C_AstStmt;
struct C_AstExpr typedef C_AstExpr;
struct C_AstAttribute typedef C_AstAttribute;

struct C_Symbol typedef C_Symbol;
struct C_SymbolScope typedef C_SymbolScope;

#define C_IsKeyword(kind) ((kind) >= C_TokenKind__FirstKeyword && (kind) <= C_TokenKind__LastKeyword)

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
	
	C_TokenKind__LastKeyword = C_TokenKind_MsvcFastcall,
	
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
	C_TokenKind_HashtagPragma, // #pragma -- field .value_str will have pragma string
	
	// only used when 'C_Lexer.preprocessor' is true
	C_TokenKind_NewLine,
	C_TokenKind_DoubleHashtag, // ##
	
	C_TokenKind__Count,
} typedef C_TokenKind;

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
};

struct C_OperatorPrecedence
{
	// from lower to higher
	int8 level; // if <= 0, it's not a binary operator
	bool8 right2left;
};

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

#define C_ValueUnion union\
{\
int64 value_int;\
uint64 value_uint;\
String value_str;\
String value_ident;\
float value_float;\
double value_double;\
}

struct C_SourceFileTrace
{
	String path;
	
	C_SourceFileTrace* included_from;
	uint32 included_line;
	bool32 is_system_file;
};

struct C_SourceTrace
{
	uint32 line, col;
	C_SourceFileTrace* file;
	
	C_SourceTrace* invocation;
	C_Macro* macro_def;
};

struct C_Token
{
	C_TokenKind kind;
	
	C_SourceTrace trace;
	String as_string;
	String leading_spaces;
	
	C_ValueUnion;
}
typedef C_Token;

struct C_TokenList
{
	C_TokenList* next;
	C_Token token;
};

struct C_TokenStream
{
	C_TokenStream* next;
	
	uint32 len;
	C_Token tokens[1024];
};

struct C_Lexer
{
	C_Token token;
	Arena* arena;
	
	// NOTE(ljre): Source trace information that will be assigned to generated tokens.
	C_SourceTrace trace;
	
	// NOTE(ljre): The lexer is special because we need multiple of those to run within a single context.
	//             It can also be NULL.
	C_Context* ctx;
	
	C_TokenList* waiting_token;
	C_TokenList* last_waiting_token;
	
	const char* head;
	const char* previous_head;
	
	// When this bool is set, the following happens:
	//     - newlines and hashtags are tokens;
	//     - keywords are going to be C_TokenKind_Identifier;
	bool16 preprocessor;
	
	bool16 token_was_pushed;
}
typedef C_Lexer;

//~ NOTE(ljre): Preprocessor
struct C_Macro
{
	String name;
	const char* def;
	
	uint32 line, col;
	C_SourceFileTrace* file;
	
	uint32 param_count;
	bool8 persistent; // cant #undef it if true
};

struct C_MacroParameter
{
	String name;
	const char* expands_to;
}
typedef C_MacroParameter;

struct C_PPLoadedFile typedef C_PPLoadedFile;
struct C_PPLoadedFile
{
	C_PPLoadedFile* next;
	uint64 hash;
	const char* contents;
	String path;
	bool8 relative;
	bool8 pragma_onced;
};

struct C_Preprocessor
{
	Map obj_macros;
	Map func_macros;
	C_PPLoadedFile* loaded_files;
	
	C_TokenStream* stream;
}
typedef C_Preprocessor;

//~ NOTE(ljre): AST
enum C_AstKind
{
	C_AstKind_Null = 0,
	C_AstKind__Category = 12,
	C_AstKind__CategoryMask = ~((1<<C_AstKind__Category)-1),
	C_AstKind__CategoryCount = 8,
	
	C_AstKind_Type = 1 << C_AstKind__Category,
	C_AstKind_TypeChar, // first
	C_AstKind_TypeInt,
	C_AstKind_TypeFloat,
	C_AstKind_TypeDouble,
	C_AstKind_TypeVoid,
	C_AstKind_TypeBool,
	C_AstKind_TypeTypename,
	C_AstKind_TypeStruct,
	C_AstKind_TypeUnion,
	C_AstKind_TypeEnum, // last
	C_AstKind_TypeFunction,
	C_AstKind_TypePointer,
	C_AstKind_TypeArray,
	C_AstKind_TypeVlaArray,
	
	C_AstKind_Decl = 2 << C_AstKind__Category,
	C_AstKind_DeclStatic,
	C_AstKind_DeclExtern,
	C_AstKind_DeclAuto,
	C_AstKind_DeclTypedef,
	C_AstKind_DeclRegister,
	C_AstKind_DeclEnumEntry,
	
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
	C_AstKind_ExprCompoundLiteral,
	C_AstKind_ExprInitializer,
	C_AstKind_ExprInitializerMember,
	C_AstKind_ExprInitializerIndex,
	
	C_AstKind_Expr1 = 5 << C_AstKind__Category,
	C_AstKind_Expr1Plus,
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
	
	C_AstKind_Attribute = 8 << C_AstKind__Category,
	C_AstKind_AttributePacked,
	C_AstKind_AttributeAlignas,
	C_AstKind_AttributeBitfield,
	C_AstKind_AttributeCallconv,
	C_AstKind_AttributeMsvcForceinline,
}
typedef C_AstKind;

// as uint32
enum C_AstFlags
{
	C_AstFlags_Poisoned = (int32)(1u<< 31),
	C_AstFlags_Implicit = 1<< 30,
	C_AstFlags_ComptimeKnown = 1<< 29,
	C_AstFlags_Decayed = 1<< 28,
	
	C_AstFlags_Volatile = 1<< 10,
	C_AstFlags_Restrict = 1<< 9,
	C_AstFlags_Const = 1<< 8,
	
	C_AstFlags_VarArgs = 1<< 1,
	C_AstFlags_Inline = 1<< 0,
	
	C_AstFlags_Signed = 1<< 0,
	C_AstFlags_Unsigned = 1<< 1,
	C_AstFlags_Long = 1<< 2,
	C_AstFlags_LongLong = 1<< 3,
	C_AstFlags_Short = 1<< 4,
	C_AstFlags_Complex = 1<< 5,
	
	C_AstFlags_GccAsmVolatile = 1<< 0,
	C_AstFlags_GccAsmInline = 1<< 1,
	C_AstFlags_GccAsmGoto = 1<< 2,
	
	C_AstFlags_MsvcCdecl = 1<< 0,
	C_AstFlags_MsvcStdcall = 1<< 1,
	C_AstFlags_MsvcVectorcall = 1<< 2,
	C_AstFlags_MsvcFastcall = 1<< 3,
};

// NOTE(ljre): Because warnings... we can typedef to C_AstNode if clang supports recognizing
//             inheritance-like structs layouts warning suppresion.
typedef void C_Node;

// NOTE(ljre): Base type for AST Nodes.
struct C_AstNode
{
	C_AstKind kind;
	uint32 flags;
	C_AstNode* next;
	
	C_SourceTrace* trace;
	
	C_Symbol* symbol;
	C_AstAttribute* attributes;
};

struct C_AstDecl
{
	C_AstNode h;
	
	C_AstType* type;
	C_AstExpr* init;
	C_AstStmt* body;
	String name;
};

struct C_AstType
{
	C_AstNode h;
	
	uint64 size;
	uint16 alignment_mask;
	// NOTE(ljre): there's padding here, so add more stuff if needed.
	
	union
	{
		struct { C_AstType* of; C_AstExpr* length; } vla_array;
		struct { C_AstType* of; uint64 length; } array;
		struct { C_AstType* to; } ptr;
		struct { C_AstType* ret; C_AstDecl* params; } function;
		struct { C_AstType* base; } not_base; // NOTE(ljre): This should match the beginning of the above.
		
		struct { C_AstDecl* body; String name; } structure;
		struct { String name; } typedefed;
		struct { C_AstDecl* entries; String name; } enumerator;
	} as[];
};

struct C_AstStmt
{
	C_AstNode h;
	
	union
	{
		struct { C_AstNode* leafs[4]; } generic; // if, for, while, do while, switch, case, etc.
		struct { C_AstExpr* expr; } expr;
		struct { C_AstStmt* stmts; } compound;
		struct { String name; C_AstStmt* stmt; } label;
		struct { String label_name; } go_to;
		struct { C_AstExpr* leafs[5]; } gcc_asm;
	} as[];
};

struct C_AstExpr
{
	C_AstNode h;
	C_AstType* type;
	
	C_ValueUnion;
	
	union
	{
		struct { C_AstExpr* expr; } unary;
		struct { C_AstExpr* left, * right; } binary;
		struct { C_AstExpr* left, * middle, * right; } ternary;
		struct { C_AstExpr* exprs; } init;
		struct { C_AstExpr* desig; C_AstExpr* expr; } desig_init;
		struct { C_AstExpr* index; String name; C_AstExpr* next; } desig_entry;
		struct { String name; void* here_for_compat; } ident;
		struct { C_AstExpr* expr; String field; } access;
		struct { C_AstType* to; C_AstExpr* expr; } cast;
		struct { C_AstExpr* init; C_AstType* type; } compound;
		struct { C_AstExpr* expr; } sizeof_expr;
		struct { C_AstType* type; } sizeof_type;
	} as[];
};

struct C_AstAttribute
{
	C_AstNode h;
	
	union
	{
		struct { C_AstExpr* expr; } bitfield;
		struct { C_AstExpr* expr; } aligned;
	} as[];
};

internal const C_AstKind C_token_to_op[C_TokenKind__Count] = {
	[C_TokenKind_Comma] = C_AstKind_Expr2Comma,
	
	[C_TokenKind_Assign] = C_AstKind_Expr2Assign,
	[C_TokenKind_PlusAssign] = C_AstKind_Expr2AssignAdd,
	[C_TokenKind_MinusAssign] = C_AstKind_Expr2AssignSub,
	[C_TokenKind_MulAssign] = C_AstKind_Expr2AssignMul,
	[C_TokenKind_DivAssign] = C_AstKind_Expr2AssignDiv,
	[C_TokenKind_LeftShiftAssign] = C_AstKind_Expr2AssignLeftShift,
	[C_TokenKind_RightShiftAssign] = C_AstKind_Expr2AssignRightShift,
	[C_TokenKind_AndAssign] = C_AstKind_Expr2AssignAnd,
	[C_TokenKind_OrAssign] = C_AstKind_Expr2AssignOr,
	[C_TokenKind_XorAssign] = C_AstKind_Expr2AssignXor,
	
	[C_TokenKind_QuestionMark] = C_AstKind_Expr3Condition,
	
	[C_TokenKind_LOr] = C_AstKind_Expr2LogicalOr,
	[C_TokenKind_LAnd] = C_AstKind_Expr2LogicalAnd,
	[C_TokenKind_Or] = C_AstKind_Expr2Or,
	[C_TokenKind_Xor] = C_AstKind_Expr2Xor,
	[C_TokenKind_And] = C_AstKind_Expr2And,
	
	[C_TokenKind_Equals] = C_AstKind_Expr2Equals,
	[C_TokenKind_NotEquals] = C_AstKind_Expr2NotEquals,
	
	[C_TokenKind_LThan] = C_AstKind_Expr2LThan,
	[C_TokenKind_GThan] = C_AstKind_Expr2GThan,
	[C_TokenKind_LEqual] = C_AstKind_Expr2LEqual,
	[C_TokenKind_GEqual] = C_AstKind_Expr2GEqual,
	
	[C_TokenKind_LeftShift] = C_AstKind_Expr2LeftShift,
	[C_TokenKind_RightShift] = C_AstKind_Expr2RightShift,
	
	[C_TokenKind_Plus] = C_AstKind_Expr2Add,
	[C_TokenKind_Minus] = C_AstKind_Expr2Sub,
	
	[C_TokenKind_Mul] = C_AstKind_Expr2Mul,
	[C_TokenKind_Div] = C_AstKind_Expr2Div,
	[C_TokenKind_Mod] = C_AstKind_Expr2Mod,
};

//~ NOTE(ljre): Symbols
struct C_SymbolScope
{
	C_SymbolScope* up;
	C_SymbolScope* down;
	C_SymbolScope* next;
	
	LittleMap* names;
	
	// NOTE(ljre): null by default
	LittleMap* types;
	LittleMap* structs;
	LittleMap* unions;
	LittleMap* enums;
};

enum C_SymbolKind
{
	C_SymbolKind_Null = 0,
	
	C_SymbolKind_Var,
	C_SymbolKind_VarDecl,
	C_SymbolKind_StaticVar,
	C_SymbolKind_Function,
	C_SymbolKind_FunctionDecl,
	C_SymbolKind_Parameter,
	C_SymbolKind_Field,
	C_SymbolKind_EnumConstant,
	C_SymbolKind_Typename,
	
	C_SymbolKind_Struct,
	C_SymbolKind_Union,
	C_SymbolKind_Enum,
}
typedef C_SymbolKind;

struct C_Symbol
{
	C_SymbolKind kind;
	uint32 flags;
	
	C_AstDecl* decl;
	String name;
	
	union
	{
		struct { C_SymbolScope* locals; } function;
		struct { C_SymbolScope* fields; } structure;
		struct { C_SymbolScope* entries; } enumerator;
		struct { int32 value; } enum_const;
		struct { uint64 offset; } field;
	} as[];
};

//~ NOTE(ljre): Basic ABI information
struct C_ABIType
{
	uint16 size;
	uint8 alignment_mask;
	bool8 unsig;
}
typedef C_ABIType;

struct C_ABI
{
	union
	{
		// NOTE(ljre): You *can* assume that 'index+1' is 'index' but unsigned, but only for types that allow it.
		
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

//~ NOTE(ljre): Compiler configuration
enum C_Warning
{
	C_Warning_Null = 0,
	
	C_Warning_ImplicitInt, // implicit type "int" in declarations
	C_Warning_RegisterIgnored, // "register" keyword ignored for now
	C_Warning_ImplicitLengthOf1, // when a global array has no length, it has a length of 1
	C_Warning_UserWarning, // #warning directive
	
	C_Warning__Count,
}
typedef C_Warning;

struct C_CompilerOptions
{
	uint64 enabled_warnings[(C_Warning__Count + 63) / 64];
	StringList* include_dirs;
}
typedef C_CompilerOptions;

struct C_QueuedWarning typedef C_QueuedWarning;
struct C_QueuedWarning
{
	C_QueuedWarning* next;
	const char* to_print;
	C_Warning warning;
};

//~ NOTE(ljre): Translation Unit Context
struct C_Context
{
	const C_CompilerOptions* options;
	const C_ABI* abi;
	Arena* persistent_arena; // NOTE(ljre): Permanent arena that will live through various stages.
	Arena* stage_arena; // NOTE(ljre): This arena is used for a single stage.
	
	// NOTE(ljre): Warning list
	C_QueuedWarning* queued_warning;
	C_QueuedWarning* last_queued_warning;
	
	// NOTE(ljre): Passes stuff
	C_Preprocessor pp;
	
	const char* source; // NOTE(ljre): source code *before* preprocessing.
	
	const char* pre_source; // NOTE(ljre): source code *after* preprocessing if in text mode (-E flag).
	C_TokenStream* tokens; // NOTE(ljre): tokens stream after preprocessing if in normal mode.
	// NOTE(ljre): /\ Check if this is null to know if we are in text mode (-E flag) or stream mode (normal).
	C_Token* token; // NOTE(ljre): Parser lookahead
	
	// NOTE(ljre): Those nodes live in 'persistent_arena' and are generated by the parser.
	C_AstDecl* ast;
	C_SymbolScope* scope;
	C_SymbolScope* previous_scope;
	
	// NOTE(ljre): For finding the host statement of 'break' and 'continue'
	C_AstStmt* host_switch;
	C_AstStmt* host_break;
	C_AstStmt* host_continue;
	
	// misc
	uint32 error_count;
	bool8 use_stage_arena_for_warnings; // :clown:
};

#endif //LANG_C_DEFINITIONS_H
