struct alignas(1) BackFormat_PeCoffHeader
{
	uint16 machine; // IMAGE_FILE_MACHINE_AMD64 = 0x8664
	uint16 number_of_sections;
	uint32 time_date_stamp;
	uint32 ptr_to_symbol_table;
	uint32 number_of_symbols;
	uint16 size_of_optional_header;
	uint16 characteristics;
}
typedef BackFormat_PeCoffHeader;

struct alignas(1) BackFormat_PeSectionHeader
{
	uint8 name[8];
	uint32 virtual_size;
	uint32 virtual_address;
	uint32 size_of_raw_data;
	uint32 ptr_to_raw_data;
	uint32 ptr_to_relocations;
	uint32 ptr_to_linenumbers;
	uint16 number_of_relocations;
	uint16 number_of_linenumbers;
	uint32 characteristics;
}
typedef BackFormat_PeSectionHeader;

struct alignas(1) BackFormat_PeRelocation
{
	uint32 virtual_address;
	uint32 symbol_table_index;
	uint16 type;
}
typedef BackFormat_PeRelocation;

struct alignas(1) BackFormat_PeSymbolTable
{
	union
	{
		uint8 short_name[8];
		struct
		{
			uint8 zeroes[4];
			uint32 offset;
		};
	}//
	name;
	
	uint32 value;
	uint16 section_number;
	uint16 type;
	uint8 storage_class;
	uint8 number_of_aux_symbols;
}
typedef BackFormat_PeSymbolTable;

struct alignas(1) BackFormat_PeAuxFormat1
{
	uint32 tag_index;
	uint32 total_size;
	uint32 ptr_to_linenumber;
	uint32 ptr_to_next_function;
	uint16 unused;
}
typedef BackFormat_PeAuxFormat1;

struct alignas(1) BackFormat_PeAuxFormat2
{
	uint32 unused;
	uint16 linenumber;
	uint8 unused2[6];
	uint32 ptr_to_next_function;
	uint16 unused3;
}
typedef BackFormat_PeAuxFormat2;

struct alignas(1) BackFormat_PeAuxFormat3
{
	uint32 tag_index;
	uint32 characteristics;
	uint8 unused[10];
}
typedef BackFormat_PeAuxFormat3;

struct alignas(1) BackFormat_PeAuxFormat4
{
	uint8 file_name[18];
}
typedef BackFormat_PeAuxFormat4;

struct alignas(1) BackFormat_PeAuxFormat5
{
	uint32 length;
	uint16 number_of_relocations;
	uint16 number_of_linenumbers;
	uint32 checksum;
	uint16 number;
	uint8 selection;
	uint8 unused[3];
}
typedef BackFormat_PeAuxFormat5;
