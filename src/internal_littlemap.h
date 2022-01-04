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
LittleMap_Insert(LittleMap* map, String key, void* value)
{
	while (map && map->len == map->cap)
	{
		if (!map->next)
			map->next = LittleMap_Create(map->arena, map->cap << 1);
		
		map = map->next;
	}
	
	uint64 hash = SimpleHash(key);
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
			
			++map->len;
			return true;
		}
		
		index = (index + 1) & map->cap-1;
	}
	
	Unreachable();
	return false;
}

internal void*
LittleMap_Fetch(LittleMap* map, String key)
{
	uint64 hash = SimpleHash(key);
	
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

#endif //INTERNAL_LITTLEMAP_H
