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
	C_TokenKind_HashtagPragma, // #pragma -- field .value_str will have pragma string
	
	// only used when 'C_Lexer.preprocessor' is true
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

struct C_PpLoadedFile typedef C_PpLoadedFile;
struct C_PpLoadedFile
{
	String fullpath;
	const char* contents;
	C_TokenSlice tokens; // NOTE(ljre): Lazily set
	
	bool16 relative;
	bool8 pragma_onced;
	bool8 is_system_file;
};

struct C_Preprocessor
{
	Map* func_macros;
	Map* obj_macros;
	
	Map* loaded_files;
	
	C_Token* out;
}
typedef C_Preprocessor;

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
	
	String* include_dirs;
	uintsize include_dirs_count;
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
C_IgnoreWhitespaces(const char** p, bool32 newline)
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

#endif //LANG_C_DEFINITIONS_H
