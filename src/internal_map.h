#ifndef INTERNAL_MAP_H
#define INTERNAL_MAP_H

// NOTE(ljre): Little HashMap: linear probing, immutable entries, exponencial growth.

struct Map_Entry
{
	uint64 hash;
	String key;
	void* value;
}
typedef Map_Entry;

struct Map typedef Map;
struct Map
{
	Map* next;
	Arena* arena;
	
	uint32 cap; // always a power of 2
	uint32 len;
	
	Map_Entry entries[];
};

internal inline bool32
Map_IsEntryDead_(const Map_Entry* entry)
{ return entry->hash == UINT64_MAX && entry->value == NULL; }

internal inline bool32
Map_IsEntryEmpty_(const Map_Entry* entry)
{ return entry->hash == 0 && entry->value == NULL; }

internal inline uint64
Map_MaxLenForBlock_(uint64 cap)
{ return cap>>1 | cap>>2; }

internal inline uint64
Map_IndexFromHash_(uint64 cap, uint64 hash)
{ return hash * 11400714819323198485llu & cap-1; }

internal Map*
Map_Create(Arena* arena, uint32 cap)
{
	Assert((cap-1 & cap) == 0);
	
	Map* map = Arena_Push(arena, sizeof(*map) + sizeof(*map->entries) * cap);
	
	map->next = NULL;
	map->arena = arena;
	map->cap = cap;
	map->len = 0;
	
	return map;
}

internal bool32
Map_InsertWithHash(Map* map, String key, void* value, uint64 hash)
{
	TraceName(key);
	
	while (map && map->len >= Map_MaxLenForBlock_(map->cap))
	{
		if (!map->next)
			map->next = Map_Create(map->arena, map->cap << 1);
		
		map = map->next;
	}
	
	uint32 start_index = Map_IndexFromHash_(map->cap, hash);
	uint32 index = start_index;
	
	while ((index+1 & map->cap-1) != start_index)
	{
		if (map->entries[index].hash == hash && CompareStringFast(key, map->entries[index].key) == 0)
			Unreachable();
		
		if (!map->entries[index].value)
		{
			map->entries[index].key = key;
			map->entries[index].value = value;
			map->entries[index].hash = hash;
			
			++map->len;
			return true;
		}
		
		index = (index + 1) & map->cap-1;
	}
	
	Unreachable();
	return false;
}

internal void*
Map_FetchWithHash(Map* map, String key, uint64 hash)
{
	TraceName(key);
	
	for (; map; map = map->next)
	{
		uint32 start_index = Map_IndexFromHash_(map->cap, hash);
		uint32 index = start_index;
		
		for (;
			 (index+1 & map->cap-1) != start_index;
			 index = (index + 1) & map->cap-1)
		{
			if (map->entries[index].hash == hash)
			{
				if (CompareStringFast(map->entries[index].key, key) == 0)
					return map->entries[index].value;
				else
					continue;
			}
			
			if (Map_IsEntryDead_(&map->entries[index]))
				continue;
			if (!map->entries[index].value)
				break;
		}
	}
	
	return NULL;
}

internal bool32
Map_RemoveWithHash(Map* map, String key, uint64 hash)
{
	TraceName(key);
	
	for (; map; map = map->next)
	{
		uint32 start_index = Map_IndexFromHash_(map->cap, hash);
		uint32 index = start_index;
		
		for (;
			 (index+1 & map->cap-1) != start_index;
			 index = (index + 1) & map->cap-1)
		{
			if (map->entries[index].hash == hash)
			{
				if (CompareStringFast(map->entries[index].key, key) == 0)
				{
					map->entries[index].value = NULL;
					map->entries[index].hash = UINT64_MAX;
					
					return true;
				}
				else
					continue;
			}
			
			if (Map_IsEntryDead_(&map->entries[index]))
				continue;
			if (!map->entries[index].value)
				break;
		}
	}
	
	return false;
}

internal inline bool32
Map_Insert(Map* map, String key, void* value)
{ return Map_InsertWithHash(map, key, value, SimpleHash(key)); }

internal inline void*
Map_Fetch(Map* map, String key)
{ return Map_FetchWithHash(map, key, SimpleHash(key)); }

internal inline bool32
Map_Remove(Map* map, String key)
{ return Map_RemoveWithHash(map, key, SimpleHash(key)); }

struct Map_Iterator
{
	Map* map;
	uintsize index;
}
typedef Map_Iterator;

internal bool32
Map_Next(Map_Iterator* iter, String* opt_key, void** opt_value)
{
	beginning:;
	
	if (!iter->map || iter->index >= iter->map->cap)
		return false;
	
	Map_Entry* cur = &iter->map->entries[iter->index];
	Map_Entry* end = &iter->map->entries[iter->map->cap];
	
	while (cur->hash == 0 && cur->key.size == 0 || Map_IsEntryDead_(cur))
	{
		if (++cur >= end)
		{
			iter->map = iter->map->next;
			goto beginning;
		}
	}
	
	if (opt_key)
		*opt_key = cur->key;
	if (opt_value)
		*opt_value = cur->value;
	
	return true;
}

#endif //INTERNAL_MAP_H
