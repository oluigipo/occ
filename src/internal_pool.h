#ifndef INTERNAL_POOL_H
#define INTERNAL_POOL_H

#define Pool_PAGE_SIZE 128
#define Pool_HEADER_ALIGNMENT_MASK 3

struct Pool_Header typedef Pool_Header;
struct Pool_Header
{
	uint32 next_free;
};

struct Pool
{
	uintsize commited;
	uintsize reserved;
	uint32 count;
	uint32 obj_size;
	uint32 obj_alignment_mask;
	uint32 free_list;
	
	uint8 memory[];
}
typedef Pool;

internal inline uintsize
Pool_ObjSize_(Pool* pool)
{
	return (AlignUp(pool->obj_size, pool->obj_alignment_mask) +
			AlignUp(sizeof(Pool_Header), pool->obj_alignment_mask));
}

internal inline Pool_Header*
Pool_AtIndexHeader_(Pool* pool, uint32 index)
{
	return (void*)(pool->memory + Pool_ObjSize_(pool) * index);
}

internal Pool*
Pool_Create(uint32 obj_size, uint32 obj_alignment_mask, uintsize to_reserve)
{
	if (obj_alignment_mask < Pool_HEADER_ALIGNMENT_MASK)
		obj_alignment_mask = Pool_HEADER_ALIGNMENT_MASK;
	
	to_reserve = AlignUp(to_reserve, Pool_PAGE_SIZE - 1);
	
	uintsize count = 0;
	count += AlignUp(obj_size,            obj_alignment_mask);
	count += AlignUp(sizeof(Pool_Header), obj_alignment_mask);
	
	Pool* result = OS_ReserveMemory(count * to_reserve + sizeof(*result));
	if (result)
	{
		if (!OS_CommitMemory(result, count * Pool_PAGE_SIZE + sizeof(*result)))
		{
			OS_FreeMemory(result, count * to_reserve + sizeof(*result));
			result = NULL;
		}
		else
		{
			result->commited = Pool_PAGE_SIZE;
			result->reserved = to_reserve;
			result->count = 0;
			result->obj_size = obj_size;
			result->obj_alignment_mask = obj_alignment_mask;
			result->free_list = UINT32_MAX;
		}
	}
	
	return result;
}

internal void
Pool_CommitAtLeast(Pool* pool, uintsize obj_count)
{
	if (Unlikely(obj_count > pool->commited))
	{
		uintsize plus = obj_count - pool->commited;
		plus = AlignUp(plus, Pool_PAGE_SIZE - 1);
		
		Assert(plus < pool->reserved);
		
		uintsize objsize = Pool_ObjSize_(pool);
		
		if (!OS_CommitMemory(pool->memory + objsize * pool->commited, objsize * plus))
			Unreachable();
	}
}

internal inline void*
Pool_AtIndex(Pool* pool, uint32 index)
{
	return Pool_AtIndexHeader_(pool, index) + 1;
}

internal uint32
Pool_AllocIndex(Pool* pool)
{
	uint32 index;
	
	if (pool->free_list == UINT32_MAX)
	{
		Pool_CommitAtLeast(pool, pool->count + 1);
		index = pool->count++;
	}
	else
	{
		index = pool->free_list;
		pool->free_list = Pool_AtIndexHeader_(pool, index)->next_free;
	}
	
	return index;
}

internal void*
Pool_Alloc(Pool* pool)
{
	return Pool_AtIndex(pool, Pool_AllocIndex(pool));
}

internal void
Pool_FreeIndex(Pool* pool, uint32 index)
{
	Trace();
	
	Pool_Header* header = Pool_AtIndexHeader_(pool, index);
	header->next_free = pool->free_list;
	pool->free_list = index;
}

internal bool32
Pool_Owns(Pool* pool, void* ptr)
{
	uint8* p = ptr;
	return (p >= pool->memory && p < pool->memory + pool->commited * Pool_ObjSize_(pool));
}

internal void
Pool_Free(Pool* pool, void* obj)
{
	Assert(Pool_Owns(pool, obj));
	
	Pool_FreeIndex(pool, ((uint8*)obj - pool->memory) / Pool_ObjSize_(pool));
}

#endif //INTERNAL_POOL_H
