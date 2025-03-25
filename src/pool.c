#include "pool.h"

#include "log.h"
#include "utils.h"
#include <stdlib.h>

enum {
	_VT_SLOT_INDEX_BITS = 16, // Size of a slot index
	_VT_SLOT_GEN_BITS = 16,	  // Size of a slot generation
	_VT_SLOT_INDEX_MASK = (1 << _VT_SLOT_INDEX_BITS) - 1,
	_VT_DEFAULT_SLOT_COUNT = 64,
};

vt_error vt_init_pool(vt_pool *pool, u16 size) {
	assert(pool);

	// Slot 0 is reserved, so add +1 to match required count
	pool->size = VT_DEF(size, _VT_DEFAULT_SLOT_COUNT) + 1;
	pool->queue_top = 0;

	pool->slots = calloc(pool->size, sizeof(vt_slot_id));
	pool->gen_counters = calloc(pool->size, sizeof(u16));
	pool->free_queue = calloc(pool->size, sizeof(i32));
	if (!pool->slots || !pool->gen_counters || !pool->free_queue) {
		LOG_ERROR("[MEM] > Failed to alloc pool resources");
		vt_discard_pool(pool);
		return VT_ERROR_OUT_OF_MEMORY;
	}

	vt_reset_pool(pool);
	return VT_ERROR_NONE;
}

void vt_discard_pool(vt_pool *pool) {
	if (!pool) {
		return;
	}

	if (pool->slots) {
		free(pool->slots);
		pool->slots = nullptr;
	}
	if (pool->gen_counters) {
		free(pool->gen_counters);
		pool->gen_counters = nullptr;
	}
	if (pool->free_queue) {
		free(pool->free_queue);
		pool->slots = nullptr;
	}
}

void vt_reset_pool(vt_pool *pool) {
	assert(pool);

	// Push all valid indexes to the queue, from end to start
	for (u32 i = pool->size - 1; i >= 1; i -= 1) {
		pool->free_queue[pool->queue_top] = i;
		pool->slots[i] = VT_INVALID_SLOT_ID;
		pool->queue_top += 1;
	}
}

vt_slot_id vt_pool_alloc(vt_pool *pool) {
	assert(pool && pool->queue_top && pool->slots && pool->gen_counters);

	// Try to get a unused index from the queue
	if (pool->queue_top <= 0) {
		LOG_WARN("[POOL] > Slots exhausted");
		return VT_INVALID_SLOT_ID;
	}
	pool->queue_top -= 1;
	i32 idx = pool->free_queue[pool->queue_top];
	assert(idx > 0 && idx < pool->size);

	// Bump generation and wrap around on overflow
	pool->gen_counters[idx] += 1;
	u16 gen = pool->gen_counters[idx];

	vt_slot_id id = (gen << _VT_SLOT_GEN_BITS) | (idx & _VT_SLOT_INDEX_MASK);
	pool->slots[idx] = id;

	return id;
}

void vt_pool_free(vt_pool *pool, vt_slot_id id) {
	assert(pool && pool->free_queue && pool->gen_counters);
	assert(pool->queue_top < pool->size);

	if (id == VT_INVALID_SLOT_ID) {
		return;
	}
	i32 idx = id & _VT_SLOT_INDEX_MASK;

#ifndef NDEBUG
	// Don't allow double-free
	for (i32 i = 0; i < pool->queue_top; i += 1) {
		assert(pool->free_queue[i] != idx);
	}
#endif

	// Add index to the top of queue for reusage
	pool->free_queue[pool->queue_top] = idx;
	pool->queue_top += 1;
}

vt_slot_index vt_get_slot_index(vt_slot_id id) {
	vt_slot_index idx = id & _VT_SLOT_INDEX_MASK;
	assert(idx != 0);
	return idx;
}
