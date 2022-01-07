#ifndef INTERNAL_LITTLEMAP_H
#define INTERNAL_LITTLEMAP_H

// NOTE(ljre): Little HashMap: linear probing, immutable entries, exponencial growth.

struct LittleMap_Entry
{
	uint64 hash;
	String key;
	void* value;
}
typedef LittleMap_Entry;

struct LittleMap typedef LittleMap;
struct LittleMap
{
	LittleMap* next;
	Arena* arena;
	
	uint32 cap; // always a power of 2
	uint32 len;
	
	LittleMap_Entry entries[];
};

internal LittleMap*
LittleMap_Create(Arena* arena, uint32 cap)
{
	Assert((cap-1 & cap) == 0);
	
	LittleMap* map = Arena_Push(arena, sizeof(*map) + sizeof(*map->entries) * cap);
	
	map->next = NULL;
	map->arena = arena;
	map->cap = cap;
	map->len = 0;
	
	return map;
}

internal bool32
LittleMap_InsertWithHash(LittleMap* map, String key, void* value, uint64 hash)
{
	while (map && map->len == map->cap)
	{
		if (!map->next)
			map->next = LittleMap_Create(map->arena, map->cap << 1);
		
		map = map->next;
	}
	
	uint32 start_index = hash & (map->cap-1);
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

internal bool32
LittleMap_Insert(LittleMap* map, String key, void* value)
{
	return LittleMap_InsertWithHash(map, key, value, SimpleHash(key));
}

internal void*
LittleMap_FetchWithCachedHash(LittleMap* map, String key, uint64 hash)
{
	for (; map; map = map->next)
	{
		uint32 start_index = hash & (map->cap-1);
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
			
			if (!map->entries[index].value)
				break;
		}
	}
	
	return NULL;
}

internal void*
LittleMap_Fetch(LittleMap* map, String key)
{
	return LittleMap_FetchWithCachedHash(map, key, SimpleHash(key));
}

struct LittleMap_Iterator
{
	LittleMap* map;
	uintsize index;
}
typedef LittleMap_Iterator;

internal bool32
LittleMap_Next(LittleMap_Iterator* iter, String* opt_key, void** opt_value)
{
	beginning:;
	
	if (!iter->map || iter->index >= iter->map->cap)
		return false;
	
	LittleMap_Entry* cur = &iter->map->entries[iter->index];
	LittleMap_Entry* end = &iter->map->entries[iter->map->cap];
	
	while (cur->hash == 0 && cur->key.size == 0)
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

#endif //INTERNAL_LITTLEMAP_H
