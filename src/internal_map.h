#ifndef INTERNAL_MAP_H
#define INTERNAL_MAP_H

//#define Map_OBJS_FIRST

struct Map
{
	Arena* arena;
	uintsize obj_size;
	uintsize count;
	uintsize capacity;
}
typedef Map;

internal inline uintsize
Map_ObjSize_(Map* map)
{
	return AlignUp(map->obj_size, 7);
}

#ifdef Map_OBJS_FIRST

internal inline uint64*
Map_Hashes_(Map* map)
{
	return (uint64*)map->arena->memory + Map_ObjSize_(map) * map->capacity;
}

internal inline uint8*
Map_Objs_(Map* map)
{
	return map->arena->memory;
}

#else

internal inline uint64*
Map_Hashes_(Map* map)
{
	return (uint64*)map->arena->memory;
}

internal inline uint8*
Map_Objs_(Map* map)
{
	return map->arena->memory + sizeof(uint64) * map->capacity;
}

#endif

internal inline uintsize
Map_FindHash_(Map* map, uint64 search_hash)
{
	if (map->count == 0)
		return SIZE_MAX;
	
	uint64* hashes = Map_Hashes_(map);
	
	uintsize min = 0;
	uintsize max = map->count;
	uintsize index = map->count / 2;
	
	while (min < max)
	{
		if (hashes[index] > search_hash)
			min = index + 1;
		else if (hashes[index] < search_hash)
			max = index;
		else
			return index;
		
		index = min + (max - min) / 2;
	}
	
	return SIZE_MAX;
}

internal inline void
Map_MemMove_(uint64* dst, const uint64* src, uintsize count)
{
	//Assume(count > 0 && dst == src + 1);
	
	while (count--)
		dst[count] = src[count];
}

internal void
Map_Reserve(Map* map, uintsize newcap)
{
	Trace();
	
	if (newcap > map->capacity)
	{
		Arena_CommitAtLeast(map->arena, newcap * (Map_ObjSize_(map) + sizeof(uint64)));
		
		void* old_mem;
		void* new_mem;
		uintsize old_cap;
		uintsize size;
		
#ifdef Map_OBJS_FIRST
		old_mem = Map_Hashes_(map);
		old_cap = map->capacity;
		size = sizeof(uint64);
		map->capacity = newcap;
		new_mem = Map_Hashes_(map);
#else
		old_mem = Map_Objs_(map);
		old_cap = map->capacity;
		size = Map_ObjSize_(map);
		map->capacity = newcap;
		new_mem = Map_Objs_(map);
#endif
		
		size = size * old_cap;
		size >>= 3;
		Map_MemMove_(new_mem, old_mem, size);
	}
}

internal void
Map_Init(Map* map, uintsize obj_size, uintsize arena_size)
{
	map->arena = Arena_Create(arena_size);
	map->obj_size = obj_size;
	map->count = 0;
	map->capacity = 8;
	
	Arena_CommitAtLeast(map->arena, 8 * (Map_ObjSize_(map) + sizeof(uint64)));
}

internal void*
Map_CreateEntry(Map* map, uint64 hash, void* obj)
{
	Trace();
	
	if (map->count + 1 > map->capacity)
		Map_Reserve(map, map->capacity + (map->capacity >> 1) + 1);
	
	uint64* hashes = Map_Hashes_(map);
	uint8* objs = Map_Objs_(map);
	uintsize objsize = Map_ObjSize_(map);
	
	uintsize min = 0;
	uintsize max = map->count;
	uintsize index = map->count / 2;
	
	while (min < max)
	{
		if (hashes[index] > hash)
			min = index + 1;
		else if (hashes[index] < hash)
			max = index;
		else
			Unreachable();
		
		index = min + (max - min) / 2;
	}
	
	index = min;
	
	uintsize remaining = map->count - index;
	if (remaining > 0)
	{
		Map_MemMove_(hashes + index + 1, hashes + index, remaining);
		Map_MemMove_((uint64*)(objs + (index+1) * objsize),
					 (uint64*)(objs + (index  ) * objsize),
					 (remaining * objsize) >> 3);
	}
	
	hashes[index] = hash;
	uint8* result = objs + index * objsize;
	OurMemCopy(result, obj, map->obj_size);
	
	++map->count;
	return result;
}

internal bool32
Map_DeleteEntry(Map* map, uint64 hash)
{
	Trace();
	
	if (map->count == 0)
		return false;
	
	uint64* hashes = Map_Hashes_(map);
	uint8* objs = Map_Objs_(map);
	uintsize objsize = Map_ObjSize_(map);
	
	uintsize min = 0;
	uintsize max = map->count;
	uintsize index = map->count / 2;
	
	while (min < max)
	{
		if (hashes[index] > hash)
			min = index + 1;
		else if (hashes[index] < hash)
			max = index;
		else
		{
			uintsize infront = map->count - index - 1;
			if (infront > 0)
			{
				OurMemCopy(hashes + index, hashes + index + 1, infront * sizeof(uint64));
				OurMemCopy(objs + (index  ) * objsize,
						   objs + (index+1) * objsize,
						   infront * objsize);
			}
			
			--map->count;
			
			return true;
		}
		
		index = min + (max - min) / 2;
	}
	
	return false;
}

internal void*
Map_FetchEntry(Map* map, uint64 hash)
{
	Trace();
	
	uintsize index = Map_FindHash_(map, hash);
	
	return (index != SIZE_MAX ?
			Map_Objs_(map) + index * Map_ObjSize_(map) :
			NULL);
}

internal void
Map_SetEntry(Map* map, uint64 hash, void* obj)
{
	Trace();
	
	uintsize index = Map_FindHash_(map, hash);
	uintsize objsize = Map_ObjSize_(map);
	
	if (index == SIZE_MAX)
	{
		Map_CreateEntry(map, hash, obj);
	}
	else
	{
		uint8* objs = Map_Objs_(map);
		objs += index * objsize;
		OurMemCopy(objs, obj, map->obj_size);
	}
}

internal bool32
Map_Owns(Map* map, void* ptr)
{
	uint8* p = ptr;
	
	return (p >= map->arena->memory && p < map->arena->memory + map->arena->reserved);
}

#endif //INTERNAL_MAP_H
