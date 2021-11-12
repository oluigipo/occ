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
	
	// NOTE(ljre): MSVC stuff
	LangC_TokenKind_MsvcDeclspec, // __declspec
	LangC_TokenKind_MsvcAsm, // __asm
	
	LangC_TokenKind__LastKeyword = LangC_TokenKind_MsvcAsm,
	
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
	[LangC_TokenKind_MsvcDeclspec] = "__declspec",
	[LangC_TokenKind_MsvcAsm] = "__asm",
	
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
	LangC_NodeKind_Null = 0, // special
	LangC_NodeKind_BaseType, // flags = LangC_Node_BaseType_*
	LangC_NodeKind_FunctionType, // flags = LangC_Node_FunctionType_*
	LangC_NodeKind_PointerType, // flags = 
	LangC_NodeKind_ArrayType, // flags = 
	LangC_NodeKind_VariableLengthArrayType, // flags = 
	LangC_NodeKind_Ident,
	LangC_NodeKind_Decl,
	LangC_NodeKind_EmptyStmt,
	LangC_NodeKind_IfStmt,
	LangC_NodeKind_DoWhileStmt,
	LangC_NodeKind_WhileStmt,
	LangC_NodeKind_ForStmt,
	LangC_NodeKind_SwitchStmt,
	LangC_NodeKind_ReturnStmt,
	LangC_NodeKind_GotoStmt,
	LangC_NodeKind_CompoundStmt,
	LangC_NodeKind_Expr,
	LangC_NodeKind_IntConstant,
	LangC_NodeKind_LIntConstant,
	LangC_NodeKind_LLIntConstant,
	LangC_NodeKind_UintConstant,
	LangC_NodeKind_LUintConstant,
	LangC_NodeKind_LLUintConstant,
	LangC_NodeKind_FloatConstant,
	LangC_NodeKind_DoubleConstant,
	LangC_NodeKind_StringConstant,
	LangC_NodeKind_WideStringConstant,
	LangC_NodeKind_InitializerEntry,
	LangC_NodeKind_EnumEntry,
	LangC_NodeKind_Label,
	LangC_NodeKind_CaseLabel,
	LangC_NodeKind_Attribute,
}
typedef LangC_NodeKind;

// Flags for any kind of node
enum
{
	LangC_Node_Poisoned = 1 << 31,
	LangC_Node_Const = 1 << 30,
	LangC_Node_Volatile = 1 << 29,
	LangC_Node_Register = 1 << 28,
	LangC_Node_Restrict = 1 << 27,
	LangC_Node_Static = 1 << 26,
	LangC_Node_Extern = 1 << 25,
	LangC_Node_Auto = 1 << 24,
	LangC_Node_Typedef = 1 << 23,
	LangC_Node_Inline = 1 << 22,
	LangC_Node_Implicit = 1 << 21,
	
	LangC_Node_LowerBits = (1 << 21) - 1,
};

enum
{
	LangC_Node_FunctionType_VarArgs = 1,
};

enum
{
	LangC_Node_Attribute_Packed = 1,
	LangC_Node_Attribute_Bitfield = 2,
};

enum
{
	LangC_Node_BaseType_Char = 1,
	LangC_Node_BaseType_Int = 1 << 1,
	LangC_Node_BaseType_Float = 1 << 2,
	LangC_Node_BaseType_Double = 1 << 3,
	LangC_Node_BaseType_Typename = 1 << 4,
	LangC_Node_BaseType_Struct = 1 << 5,
	LangC_Node_BaseType_Union = 1 << 6,
	LangC_Node_BaseType_Enum = 1 << 7,
	LangC_Node_BaseType_Void = 1 << 8,
	LangC_Node_BaseType_Bool = 1 << 9,
	
	LangC_Node_BaseType_Signed = 1 << 10,
	LangC_Node_BaseType_Unsigned = 1 << 11,
	LangC_Node_BaseType_Long = 1 << 12,
	LangC_Node_BaseType_Short = 1 << 13,
	LangC_Node_BaseType_Complex = 1 << 14,
	
	// combination of Short and Long
	LangC_Node_BaseType_LongLong = LangC_Node_BaseType_Long | LangC_Node_BaseType_Short,
};

enum
{
	// unary
	LangC_Node_Expr__FirstUnary,
	LangC_Node_Expr_Negative = LangC_Node_Expr__FirstUnary, // -expr
	LangC_Node_Expr_Not, // ~expr
	LangC_Node_Expr_LogicalNot, // !expr
	LangC_Node_Expr_Deref, // *expr
	LangC_Node_Expr_Ref, // &expr
	LangC_Node_Expr_PrefixInc, // ++expr
	LangC_Node_Expr_PrefixDec, // --expr
	LangC_Node_Expr_PostfixInc, // expr++
	LangC_Node_Expr_PostfixDec, // expr--
	LangC_Node_Expr_Sizeof, // sizeof expr // sizeof (type)
	
	// binary
	LangC_Node_Expr__FirstBinary,
	LangC_Node_Expr_Add = LangC_Node_Expr__FirstBinary, // left + right
	LangC_Node_Expr_Sub, // left - right
	LangC_Node_Expr_Mul, // left * right
	LangC_Node_Expr_Div, // left / right
	LangC_Node_Expr_Mod, // left % right
	LangC_Node_Expr_LThan, // left < right
	LangC_Node_Expr_GThan, // left > right
	LangC_Node_Expr_LEqual, // left <= right
	LangC_Node_Expr_GEqual, // left >= right
	LangC_Node_Expr_Equals, // left == right
	LangC_Node_Expr_NotEquals, // left != right
	LangC_Node_Expr_LeftShift, // left << right
	LangC_Node_Expr_RightShift, // left >> right
	LangC_Node_Expr_And, // left & right
	LangC_Node_Expr_Or, // left | right
	LangC_Node_Expr_Xor, // left ^ right
	LangC_Node_Expr_LogicalAnd, // left && right
	LangC_Node_Expr_LogicalOr, // left || right
	LangC_Node_Expr_Assign, // left = right
	LangC_Node_Expr_AssignAdd, // left += right
	LangC_Node_Expr_AssignSub, // left -= right
	LangC_Node_Expr_AssignMul, // left *= right
	LangC_Node_Expr_AssignDiv, // left /= right
	LangC_Node_Expr_AssignMod, // left %= right
	LangC_Node_Expr_AssignLeftShift, // left <<= right
	LangC_Node_Expr_AssignRightShift, // left >>= right
	LangC_Node_Expr_AssignAnd, // left &= right
	LangC_Node_Expr_AssignOr, // left |= right
	LangC_Node_Expr_AssignXor, // left ^= right
	LangC_Node_Expr_Comma, // left, right
	
	// NOTE(ljre): maybe???? cool GCC extension
	//LangC_Node_Expr_Optional, // left ?: right
	
	// special
	LangC_Node_Expr__FirstSpecial,
	LangC_Node_Expr_Call = LangC_Node_Expr__FirstSpecial, // left(right, right->next, ...)
	LangC_Node_Expr_Index, // left[right]
	LangC_Node_Expr_Cast, // (type)expr
	LangC_Node_Expr_Ternary, // left ? middle : right
	LangC_Node_Expr_CompoundLiteral, // (type) { init, init->next, ... }
	LangC_Node_Expr_Initializer, // { init, init->next, ... }
	LangC_Node_Expr_Access, // expr.name
	LangC_Node_Expr_DerefAccess, // expr->name
};

enum
{
	LangC_Node_InitializerEntry_JustValue = 0,
	LangC_Node_InitializerEntry_ArrayIndex,
	LangC_Node_InitializerEntry_Field,
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
	
	LangC_SymbolKind_GlobalVar,
	LangC_SymbolKind_GlobalStaticVar,
	LangC_SymbolKind_GlobalVarDecl,
	LangC_SymbolKind_GlobalFunction,
	LangC_SymbolKind_GlobalFunctionDecl,
	LangC_SymbolKind_Parameter,
	LangC_SymbolKind_Field,
	LangC_SymbolKind_LocalVar,
	LangC_SymbolKind_LocalFunctionDecl,
	LangC_SymbolKind_EnumConstant,
	
	LangC_SymbolKind_Typename,
	LangC_SymbolKind_Struct,
	LangC_SymbolKind_Union,
	LangC_SymbolKind_Enum,
}
typedef LangC_SymbolKind;

struct LangC_Symbol
{
	LangC_Symbol* next;
	
	LangC_Node* type;
	String name;
	uint64 name_hash;
	
	LangC_SymbolKind kind;
	
	uintsize size;
	uint64 flags; // LangC_Node_* flags
	
	union
	{
		// NOTE(ljre): For functions.
		struct
		{
			LangC_SymbolStack* locals;
			uintsize stack_needed;
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
	// { .left[middle] = right, .name = expr, expr, [middle] = right, }
	LangC_Node* init;
	LangC_Node* iter;
	union
	{
		struct
		{
			LangC_Node* stmt;
			LangC_Node* stmt2;
			LangC_Node* expr;
		};
		
		struct
		{
			LangC_Node* left;
			LangC_Node* middle;
			LangC_Node* right;
		};
		
		struct
		{
			LangC_Node* body;
			LangC_Node* params;
		};
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
		struct
		{
			LangC_ABIType t_char;
			LangC_ABIType t_schar;
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
		
		LangC_ABIType t[14];
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
	
	bool8 use_stage_arena_for_warnings;
};

#endif //LANG_C_DEFINITIONS_H
