#ifndef LANG_C_DEFINITIONS_H
#define LANG_C_DEFINITIONS_H

struct LangC_Context typedef LangC_Context;
struct LangC_OperatorPrecedence typedef LangC_OperatorPrecedence;
struct LangC_TokenList typedef LangC_TokenList;
struct LangC_LexerFile typedef LangC_LexerFile;

struct LangC_AstNode typedef LangC_AstNode;
struct LangC_AstDecl typedef LangC_AstDecl;
struct LangC_AstType typedef LangC_AstType;
struct LangC_AstStmt typedef LangC_AstStmt;
struct LangC_AstExpr typedef LangC_AstExpr;
struct LangC_AstAttribute typedef LangC_AstAttribute;

struct LangC_Symbol typedef LangC_Symbol;
struct LangC_SymbolScope typedef LangC_SymbolScope;

#define LangC_IsKeyword(kind) ((kind) >= LangC_TokenKind__FirstKeyword && (kind) <= LangC_TokenKind__LastKeyword)

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

#define LangC_ValueUnion union\
{\
int64 value_int;\
uint64 value_uint;\
String value_str;\
String value_ident;\
float value_float;\
double value_double;\
}

struct LangC_Token
{
	int32 line, col;
	LangC_TokenKind kind;
	String as_string;
	String leading_spaces;
	
	LangC_ValueUnion;
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
	uint32 line, col;
	
	// When this bool is set, the following happens:
	//     - newlines and hashtags are tokens;
	//     - keywords are going to be LangC_TokenKind_Identifier;
	bool16 preprocessor;
	
	bool16 token_was_pushed;
}
typedef LangC_Lexer;

//~ NOTE(ljre): AST
enum LangC_AstKind
{
	LangC_AstKind_Null = 0,
	LangC_AstKind__Category = 12,
	LangC_AstKind__CategoryMask = ~((1<<LangC_AstKind__Category)-1),
	
	LangC_AstKind_Type = 1 << LangC_AstKind__Category,
	LangC_AstKind_TypeChar, // first
	LangC_AstKind_TypeInt,
	LangC_AstKind_TypeFloat,
	LangC_AstKind_TypeDouble,
	LangC_AstKind_TypeVoid,
	LangC_AstKind_TypeBool,
	LangC_AstKind_TypeTypename,
	LangC_AstKind_TypeStruct,
	LangC_AstKind_TypeUnion,
	LangC_AstKind_TypeEnum, // last
	LangC_AstKind_TypeFunction,
	LangC_AstKind_TypePointer,
	LangC_AstKind_TypeArray,
	LangC_AstKind_TypeVlaArray,
	
	LangC_AstKind_Decl = 2 << LangC_AstKind__Category,
	LangC_AstKind_DeclStatic,
	LangC_AstKind_DeclExtern,
	LangC_AstKind_DeclAuto,
	LangC_AstKind_DeclTypedef,
	LangC_AstKind_DeclRegister,
	LangC_AstKind_DeclEnumEntry,
	
	LangC_AstKind_Stmt = 3 << LangC_AstKind__Category,
	LangC_AstKind_StmtEmpty,
	LangC_AstKind_StmtExpr,
	LangC_AstKind_StmtIf,
	LangC_AstKind_StmtDoWhile,
	LangC_AstKind_StmtWhile,
	LangC_AstKind_StmtFor,
	LangC_AstKind_StmtSwitch,
	LangC_AstKind_StmtReturn,
	LangC_AstKind_StmtGoto,
	LangC_AstKind_StmtCompound,
	LangC_AstKind_StmtLabel,
	LangC_AstKind_StmtCase,
	LangC_AstKind_StmtGccAsm,
	
	LangC_AstKind_ExprFactor = 4 << LangC_AstKind__Category,
	LangC_AstKind_ExprIdent,
	LangC_AstKind_ExprInt,
	LangC_AstKind_ExprLInt,
	LangC_AstKind_ExprLLInt,
	LangC_AstKind_ExprUInt,
	LangC_AstKind_ExprULInt,
	LangC_AstKind_ExprULLInt,
	LangC_AstKind_ExprFloat,
	LangC_AstKind_ExprDouble,
	LangC_AstKind_ExprString,
	LangC_AstKind_ExprWideString,
	LangC_AstKind_ExprCompoundLiteral,
	LangC_AstKind_ExprInitializer,
	LangC_AstKind_ExprInitializerMember,
	LangC_AstKind_ExprInitializerIndex,
	
	LangC_AstKind_Expr1 = 5 << LangC_AstKind__Category,
	LangC_AstKind_Expr1Plus,
	LangC_AstKind_Expr1Negative,
	LangC_AstKind_Expr1Not,
	LangC_AstKind_Expr1LogicalNot,
	LangC_AstKind_Expr1Deref,
	LangC_AstKind_Expr1Ref,
	LangC_AstKind_Expr1PrefixInc,
	LangC_AstKind_Expr1PrefixDec,
	LangC_AstKind_Expr1PostfixInc,
	LangC_AstKind_Expr1PostfixDec,
	LangC_AstKind_Expr1Sizeof,
	LangC_AstKind_Expr1SizeofType,
	LangC_AstKind_Expr1Cast,
	
	LangC_AstKind_Expr2 = 6 << LangC_AstKind__Category,
	LangC_AstKind_Expr2Add,
	LangC_AstKind_Expr2Sub,
	LangC_AstKind_Expr2Mul,
	LangC_AstKind_Expr2Div,
	LangC_AstKind_Expr2Mod,
	LangC_AstKind_Expr2LThan,
	LangC_AstKind_Expr2GThan,
	LangC_AstKind_Expr2LEqual,
	LangC_AstKind_Expr2GEqual,
	LangC_AstKind_Expr2Equals,
	LangC_AstKind_Expr2NotEquals,
	LangC_AstKind_Expr2LeftShift,
	LangC_AstKind_Expr2RightShift,
	LangC_AstKind_Expr2And,
	LangC_AstKind_Expr2Or,
	LangC_AstKind_Expr2Xor,
	LangC_AstKind_Expr2LogicalAnd,
	LangC_AstKind_Expr2LogicalOr,
	LangC_AstKind_Expr2Assign,
	LangC_AstKind_Expr2AssignAdd,
	LangC_AstKind_Expr2AssignSub,
	LangC_AstKind_Expr2AssignMul,
	LangC_AstKind_Expr2AssignDiv,
	LangC_AstKind_Expr2AssignMod,
	LangC_AstKind_Expr2AssignLeftShift,
	LangC_AstKind_Expr2AssignRightShift,
	LangC_AstKind_Expr2AssignAnd,
	LangC_AstKind_Expr2AssignOr,
	LangC_AstKind_Expr2AssignXor,
	LangC_AstKind_Expr2Comma,
	LangC_AstKind_Expr2Call,
	LangC_AstKind_Expr2Index,
	LangC_AstKind_Expr2Access,
	LangC_AstKind_Expr2DerefAccess,
	
	LangC_AstKind_Expr3 = 7 << LangC_AstKind__Category,
	LangC_AstKind_Expr3Condition,
	
	LangC_AstKind_Attribute = 8 << LangC_AstKind__Category,
	LangC_AstKind_AttributePacked,
	LangC_AstKind_AttributeAlignas,
	LangC_AstKind_AttributeBitfield,
	LangC_AstKind_AttributeCallconv,
}
typedef LangC_AstKind;

// as uint32
enum LangC_AstFlags
{
	LangC_AstFlags_Poisoned = (int32)(1u<< 31),
	LangC_AstFlags_Implicit = 1<< 30,
	LangC_AstFlags_ComptimeKnown = 1<< 29,
	LangC_AstFlags_Decayed = 1<< 28,
	
	LangC_AstFlags_Volatile = 1<< 10,
	LangC_AstFlags_Restrict = 1<< 9,
	LangC_AstFlags_Const = 1<< 8,
	
	LangC_AstFlags_VarArgs = 1<< 1,
	LangC_AstFlags_Inline = 1<< 0,
	
	LangC_AstFlags_Signed = 1<< 0,
	LangC_AstFlags_Unsigned = 1<< 1,
	LangC_AstFlags_Long = 1<< 2,
	LangC_AstFlags_LongLong = 1<< 3,
	LangC_AstFlags_Short = 1<< 4,
	LangC_AstFlags_Complex = 1<< 5,
	
	LangC_AstFlags_GccAsmVolatile = 1<< 0,
	LangC_AstFlags_GccAsmInline = 1<< 1,
	LangC_AstFlags_GccAsmGoto = 1<< 2,
	
	LangC_AstFlags_MsvcCdecl = 1<< 0,
	LangC_AstFlags_MsvcStdcall = 1<< 1,
	LangC_AstFlags_MsvcVectorcall = 1<< 2,
	LangC_AstFlags_MsvcFastcall = 1<< 3,
};

// NOTE(ljre): Because warnings.
typedef void LangC_Node;

// NOTE(ljre): Base type for AST Nodes.
struct LangC_AstNode
{
	LangC_AstKind kind;
	uint32 flags;
	LangC_AstNode* next;
	
	LangC_LexerFile* lexfile;
	uint32 line, col;
	
	LangC_Symbol* symbol;
	LangC_AstAttribute* attributes;
};

struct LangC_AstDecl
{
	LangC_AstNode h;
	
	LangC_AstType* type;
	LangC_AstExpr* init;
	LangC_AstStmt* body;
	String name;
};

struct LangC_AstType
{
	LangC_AstNode h;
	
	uint64 size;
	uint16 alignment;
	
	union
	{
		struct { LangC_AstType* of; LangC_AstExpr* length; } vla_array;
		struct { LangC_AstType* of; uint64 length; } array;
		struct { LangC_AstType* to; } ptr;
		struct { LangC_AstType* ret; LangC_AstDecl* params; } function;
		struct { LangC_AstDecl* body; String name; } structure;
		struct { String name; } typedefed;
		struct { LangC_AstDecl* entries; String name; } enumerator;
	} as[];
};

struct LangC_AstStmt
{
	LangC_AstNode h;
	
	union
	{
		struct { LangC_AstNode* leafs[4]; } generic; // if, for, while, do while, switch, case, etc.
		struct { LangC_AstExpr* expr; } expr;
		struct { LangC_AstStmt* stmts; } compound;
		struct { String name; LangC_AstStmt* stmt; } label;
		struct { String label_name; } go_to;
		struct { LangC_AstExpr* leafs[5]; } gcc_asm;
	} as[];
};

struct LangC_AstExpr
{
	LangC_AstNode h;
	LangC_AstType* type;
	
	LangC_ValueUnion;
	
	union
	{
		struct { LangC_AstExpr* expr; } unary;
		struct { LangC_AstExpr* left, * right; } binary;
		struct { LangC_AstExpr* left, * middle, * right; } ternary;
		struct { LangC_AstExpr* exprs; } init;
		struct { LangC_AstExpr* desig; LangC_AstExpr* expr; } desig_init;
		struct { LangC_AstExpr* index; String name; LangC_AstExpr* next; } desig_entry;
		struct { String name; void* here_for_compat; } ident;
		struct { LangC_AstExpr* expr; String field; } access;
		struct { LangC_AstType* to; LangC_AstExpr* expr; } cast;
		struct { LangC_AstExpr* init; LangC_AstType* type; } compound;
		struct { LangC_AstExpr* expr; } sizeof_expr;
		struct { LangC_AstType* type; } sizeof_type;
	} as[];
};

struct LangC_AstAttribute
{
	LangC_AstNode h;
	
	union
	{
		struct { LangC_AstExpr* expr; } bitfield;
		struct { LangC_AstExpr* expr; } aligned;
	} as[];
};

internal const LangC_AstKind LangC_token_to_op[LangC_TokenKind__Count] = {
	[LangC_TokenKind_Comma] = LangC_AstKind_Expr2Comma,
	
	[LangC_TokenKind_Assign] = LangC_AstKind_Expr2Assign,
	[LangC_TokenKind_PlusAssign] = LangC_AstKind_Expr2AssignAdd,
	[LangC_TokenKind_MinusAssign] = LangC_AstKind_Expr2AssignSub,
	[LangC_TokenKind_MulAssign] = LangC_AstKind_Expr2AssignMul,
	[LangC_TokenKind_DivAssign] = LangC_AstKind_Expr2AssignDiv,
	[LangC_TokenKind_LeftShiftAssign] = LangC_AstKind_Expr2AssignLeftShift,
	[LangC_TokenKind_RightShiftAssign] = LangC_AstKind_Expr2AssignRightShift,
	[LangC_TokenKind_AndAssign] = LangC_AstKind_Expr2AssignAnd,
	[LangC_TokenKind_OrAssign] = LangC_AstKind_Expr2AssignOr,
	[LangC_TokenKind_XorAssign] = LangC_AstKind_Expr2AssignXor,
	
	[LangC_TokenKind_QuestionMark] = LangC_AstKind_Expr3Condition,
	
	[LangC_TokenKind_LOr] = LangC_AstKind_Expr2LogicalOr,
	[LangC_TokenKind_LAnd] = LangC_AstKind_Expr2LogicalAnd,
	[LangC_TokenKind_Or] = LangC_AstKind_Expr2Or,
	[LangC_TokenKind_Xor] = LangC_AstKind_Expr2Xor,
	[LangC_TokenKind_And] = LangC_AstKind_Expr2And,
	
	[LangC_TokenKind_Equals] = LangC_AstKind_Expr2Equals,
	[LangC_TokenKind_NotEquals] = LangC_AstKind_Expr2NotEquals,
	
	[LangC_TokenKind_LThan] = LangC_AstKind_Expr2LThan,
	[LangC_TokenKind_GThan] = LangC_AstKind_Expr2GThan,
	[LangC_TokenKind_LEqual] = LangC_AstKind_Expr2LEqual,
	[LangC_TokenKind_GEqual] = LangC_AstKind_Expr2GEqual,
	
	[LangC_TokenKind_LeftShift] = LangC_AstKind_Expr2LeftShift,
	[LangC_TokenKind_RightShift] = LangC_AstKind_Expr2RightShift,
	
	[LangC_TokenKind_Plus] = LangC_AstKind_Expr2Add,
	[LangC_TokenKind_Minus] = LangC_AstKind_Expr2Sub,
	
	[LangC_TokenKind_Mul] = LangC_AstKind_Expr2Mul,
	[LangC_TokenKind_Div] = LangC_AstKind_Expr2Div,
	[LangC_TokenKind_Mod] = LangC_AstKind_Expr2Mod,
};

//~ NOTE(ljre): Symbols
struct LangC_SymbolScope
{
	LangC_SymbolScope* up;
	LangC_SymbolScope* down;
	LangC_SymbolScope* next;
	
	LittleMap* names;
	
	// NOTE(ljre): null by default
	LittleMap* types;
	LittleMap* structs;
	LittleMap* unions;
	LittleMap* enums;
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
	
	LangC_SymbolKind_Struct,
	LangC_SymbolKind_Union,
	LangC_SymbolKind_Enum,
}
typedef LangC_SymbolKind;

struct LangC_Symbol
{
	LangC_SymbolKind kind;
	uint32 flags;
	
	LangC_AstDecl* decl;
	String name;
	
	union
	{
		struct { LangC_SymbolScope* locals; } function;
		struct { LangC_SymbolScope* fields; } structure;
		struct { LangC_SymbolScope* entries; } enumerator;
		struct { int32 value; } enum_const;
		struct { uintsize offset; } field;
	} as[];
};

//~ NOTE(ljre): Preprocessor
struct LangC_Macro typedef LangC_Macro;
struct LangC_Macro
{
	String name;
	const char* def;
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
	Map obj_macros;
	Map func_macros;
	LangC_PPLoadedFile* loaded_files;
}
typedef LangC_Preprocessor;

//~ NOTE(ljre): Basic ABI information
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
			LangC_ABIType t_schar;
			LangC_ABIType t_char;
			LangC_ABIType t_uchar;
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

//~ NOTE(ljre): Compiler configuration
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

struct LangC_CompilerOptions
{
	uint64 enabled_warnings[(LangC_Warning__Count + 63) / 64];
	StringList* include_dirs;
}
typedef LangC_CompilerOptions;

struct LangC_QueuedWarning typedef LangC_QueuedWarning;
struct LangC_QueuedWarning
{
	LangC_QueuedWarning* next;
	const char* to_print;
	LangC_Warning warning;
};

//~ NOTE(ljre): Translation Unit Context
struct LangC_Context
{
	const LangC_CompilerOptions* options;
	const LangC_ABI* abi;
	Arena* persistent_arena; // NOTE(ljre): Permanent arena that will live through various stages.
	Arena* stage_arena; // NOTE(ljre): This arena is used for a single stage.
	
	// NOTE(ljre): Warning list
	LangC_QueuedWarning* queued_warning;
	LangC_QueuedWarning* last_queued_warning;
	
	// NOTE(ljre): Passes stuff
	LangC_Lexer lex;
	LangC_Preprocessor pp;
	
	const char* source; // NOTE(ljre): source code *before* preprocessing.
	const char* pre_source; // NOTE(ljre): source code *after* preprocessing.
	
	// NOTE(ljre): Those nodes live in 'persistent_arena' and are generated by the parser.
	LangC_AstDecl* ast;
	LangC_SymbolScope* scope;
	LangC_SymbolScope* previous_scope;
	
	// NOTE(ljre): For finding the host statement of 'break' and 'continue'
	LangC_AstStmt* host_switch;
	LangC_AstStmt* host_break;
	LangC_AstStmt* host_continue;
	
	// misc
	uint32 error_count;
	bool8 use_stage_arena_for_warnings; // :clown:
};

#endif //LANG_C_DEFINITIONS_H
