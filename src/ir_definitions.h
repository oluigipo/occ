#ifndef IR_DEFINITIONS_H
#define IR_DEFINITIONS_H

struct Ir_Inst typedef Ir_Inst;
struct Ir_Block typedef Ir_Block;
struct Ir_Function typedef Ir_Function;
struct Ir_Locals typedef Ir_Locals;

enum Ir_InstKind
{
	Ir_InstKind_Ret,
	Ir_InstKind_InlineAsm,
	
	//- NOTE(ljre): Loads & Stores
	Ir_InstKind_PushLocal8,
	Ir_InstKind_PushLocal16,
	Ir_InstKind_PushLocal32,
	Ir_InstKind_PushLocal64,
	Ir_InstKind_PushParam8,
	Ir_InstKind_PushParam16,
	Ir_InstKind_PushParam32,
	Ir_InstKind_PushParam64,
	Ir_InstKind_PushLocalRef8,
	Ir_InstKind_PushLocalRef16,
	Ir_InstKind_PushLocalRef32,
	Ir_InstKind_PushLocalRef64,
	Ir_InstKind_PushParamRef8,
	Ir_InstKind_PushParamRef16,
	Ir_InstKind_PushParamRef32,
	Ir_InstKind_PushParamRef64,
	Ir_InstKind_PushImm8,
	Ir_InstKind_PushImm16,
	Ir_InstKind_PushImm32,
	Ir_InstKind_PushImm64,
	
	Ir_InstKind_Pop8,
	Ir_InstKind_Pop16,
	Ir_InstKind_Pop32,
	Ir_InstKind_Pop64,
	Ir_InstKind_PopLocal8,
	Ir_InstKind_PopLocal16,
	Ir_InstKind_PopLocal32,
	Ir_InstKind_PopLocal64,
	Ir_InstKind_PopDeref8,
	Ir_InstKind_PopDeref16,
	Ir_InstKind_PopDeref32,
	Ir_InstKind_PopDeref64,
	Ir_InstKind_PopRet8,
	Ir_InstKind_PopRet16,
	Ir_InstKind_PopRet32,
	Ir_InstKind_PopRet64,
	
	Ir_InstKind_StoreLocal8,
	Ir_InstKind_StoreLocal16,
	Ir_InstKind_StoreLocal32,
	Ir_InstKind_StoreLocal64,
	Ir_InstKind_StoreDeref8,
	Ir_InstKind_StoreDeref16,
	Ir_InstKind_StoreDeref32,
	Ir_InstKind_StoreDeref64,
	Ir_InstKind_StoreRet8,
	Ir_InstKind_StoreRet16,
	Ir_InstKind_StoreRet32,
	Ir_InstKind_StoreRet64,
	
	Ir_InstKind_MemoryCopy8, // dst, src, len
	Ir_InstKind_MemoryCopy16,
	Ir_InstKind_MemoryCopy32,
	Ir_InstKind_MemoryCopy64,
	Ir_InstKind_MemorySet8, // dst, val, len
	Ir_InstKind_MemorySet16,
	Ir_InstKind_MemorySet32,
	Ir_InstKind_MemorySet64,
	
	//- NOTE(ljre): Control Flow
	Ir_InstKind_Jmp,
	Ir_InstKind_BranchEqual, // ==
	Ir_InstKind_BranchNotEqual, // !=
	Ir_InstKind_BranchLessThan, // <
	Ir_InstKind_BranchGreaterThan, // >
	Ir_InstKind_BranchLessEqual, // <=
	Ir_InstKind_BranchGreaterEqual, // >=
	
	Ir_InstKind_Call,
	
	//- NOTE(ljre): Comparisions
	Ir_InstKind_CmpI8, // NOTE(ljre): These compare a number with 0.
	Ir_InstKind_CmpI16,
	Ir_InstKind_CmpI32,
	Ir_InstKind_CmpI64,
	Ir_InstKind_CmpU8,
	Ir_InstKind_CmpU16,
	Ir_InstKind_CmpU32,
	Ir_InstKind_CmpU64,
	Ir_InstKind_CmpF32,
	Ir_InstKind_CmpF64,
	
	Ir_InstKind_Cmp2I8, // NOTE(ljre): These compare two numbers.
	Ir_InstKind_Cmp2I16,
	Ir_InstKind_Cmp2I32,
	Ir_InstKind_Cmp2I64,
	Ir_InstKind_Cmp2U8,
	Ir_InstKind_Cmp2U16,
	Ir_InstKind_Cmp2U32,
	Ir_InstKind_Cmp2U64,
	Ir_InstKind_Cmp2F32,
	Ir_InstKind_Cmp2F64,
	
	//- NOTE(ljre): Simple Operations
	Ir_InstKind_AddUI8,
	Ir_InstKind_AddUI16,
	Ir_InstKind_AddUI32,
	Ir_InstKind_AddUI64,
	Ir_InstKind_AddF32,
	Ir_InstKind_AddF64,
	
	Ir_InstKind_SubUI8,
	Ir_InstKind_SubUI16,
	Ir_InstKind_SubUI32,
	Ir_InstKind_SubUI64,
	Ir_InstKind_SubF32,
	Ir_InstKind_SubF64,
	
	Ir_InstKind_MulI8,
	Ir_InstKind_MulI16,
	Ir_InstKind_MulI32,
	Ir_InstKind_MulI64,
	Ir_InstKind_MulU8,
	Ir_InstKind_MulU16,
	Ir_InstKind_MulU32,
	Ir_InstKind_MulU64,
	Ir_InstKind_MulF32,
	Ir_InstKind_MulF64,
	
	Ir_InstKind_DivI8,
	Ir_InstKind_DivI16,
	Ir_InstKind_DivI32,
	Ir_InstKind_DivI64,
	Ir_InstKind_DivU8,
	Ir_InstKind_DivU16,
	Ir_InstKind_DivU32,
	Ir_InstKind_DivU64,
	Ir_InstKind_DivF32,
	Ir_InstKind_DivF64,
}
typedef Ir_InstKind;

struct Ir_Inst
{
	Ir_InstKind kind;
	// TODO(ljre): Use those 4 bytes of padding for debug info.
	
	union
	{
		uint64 imm64;
		uint32 imm32[2];
		uint16 imm16[4];
		uint8 imm8[8];
	};
};

struct Ir_Block
{
	Ir_Block* next;
	
	uint16 len;
	Ir_Inst code[128];
};

enum Ir_LocalKind
{
	Ir_LocalKind_Int8,
	Ir_LocalKind_Int16,
	Ir_LocalKind_Int32,
	Ir_LocalKind_Int64,
	
	Ir_LocalKind_UInt8,
	Ir_LocalKind_UInt16,
	Ir_LocalKind_UInt32,
	Ir_LocalKind_UInt64,
	
	Ir_LocalKind_Float32,
	Ir_LocalKind_Float64,
	
	Ir_LocalKind_Bytes,
}
typedef Ir_LocalKind;

struct Ir_Locals
{
	Ir_Locals* next;
	uint32 count;
	
	struct
	{
		Ir_LocalKind kind;
		uint32 len;
	} values[16];
};

struct Ir_Function
{
	const char* name;
	Ir_Locals* params;
	Ir_Locals* ret;
	
	// NOTE(ljre): Both nullable - external function.
	Ir_Block* code;
	Ir_Locals* locals;
};

struct Ir_Global typedef Ir_Global;
struct Ir_Global
{
	const char* name;
	void* data;
	uintsize size;
};

struct Ir_Program typedef Ir_Program;
struct Ir_Program
{
	Ir_Function* functions;
	Ir_Global* globals;
	Ir_Global* readonly;
	
	uint32 function_count;
	uint32 global_count;
	uint32 readonly_count;
};

#endif //IR_DEFINITIONS_H
