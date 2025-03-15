#include "vita/gpu/uniform.h"

#include "log.h"
#include <stdlib.h>
#include <string.h>

enum {
	_VT_DEFAULT_UNIFORM_BLOCKS = 256,
	_VT_DEFAULT_UNIFORM_BLOCK_SIZE = 16,
};

vt_error vt_init_uniforms_pool(vt_uniforms_pool *pool, u16 size) {
	assert(pool);
	assert(!pool->data && !pool->blocks);

	u16 pool_size = VT_DEF(size, _VT_DEFAULT_UNIFORM_BLOCKS);

	pool->capacity = pool_size * _VT_DEFAULT_UNIFORM_BLOCK_SIZE;
	pool->lenght = 0;
	pool->used_count = 0;

	vt_error err = vt_init_pool(&pool->pool, pool_size);
	if (err != VT_ERROR_NONE) {
		LOG_ERROR("[SHADER] > Unable to initialize pool pool");
		vt_destroy_uniforms_pool(pool);
		return VT_ERROR_GENERIC;
	}

	pool->data = calloc(pool->capacity, sizeof(u8));
	pool->used = calloc(pool->pool.size, sizeof(vt_uniform));
	pool->blocks = calloc(pool->pool.size, sizeof(_vt_uniform_block));
	if (!pool->data || !pool->used || !pool->blocks) {
		LOG_ERROR("[SHADER] > Failed alloc pool pool resources");
		vt_destroy_uniforms_pool(pool);
		return VT_ERROR_OUT_OF_MEMORY;
	}

	return VT_ERROR_NONE;
}

void vt_destroy_uniforms_pool(vt_uniforms_pool *pool) {
	if (!pool) {
		return;
	}

	if (pool->data) {
		free(pool->data);
		pool->data = nullptr;
	}
	if (pool->used) {
		free(pool->used);
		pool->used = nullptr;
	}
	if (pool->blocks) {
		free(pool->blocks);
		pool->blocks = nullptr;
	}

	vt_discard_pool(&pool->pool);
}

void vt_reset_uniforms_pool(vt_uniforms_pool *pool) {
	assert(pool);

	// Free all used uniforms to bump slot generation
	for (i32 i = 0; i < pool->used_count; i += 1) {
		vt_pool_free(&pool->pool, pool->used[i]);
		pool->used[i] = VT_INVALID_SLOT_ID;
	}

	pool->lenght = 0;
	pool->used_count = 0;
}

static vt_slot_id _vt_try_reuse_uniform(vt_uniforms_pool *pool, const sg_range *data) {
	for (i32 i = 0; i < pool->used_count; i += 1) {
		vt_slot_id id = pool->used[i];
		if (id == VT_INVALID_SLOT_ID) {
			break;
		}

		// Check if block has the same data and lenght
		_vt_uniform_block *block = &pool->blocks[vt_get_slot_index(id)];
		if (block->size == data->size
			&& !memcmp(pool->data + block->offset, data->ptr, data->size)) {
			return id;
		}
	}

	return VT_INVALID_SLOT_ID;
}

vt_uniform vt_create_uniform(vt_uniforms_pool *pool, const sg_range *data) {
	assert(pool && data);

	if (data->size == 0 || !data->ptr) {
		return (vt_uniform) { VT_INVALID_SLOT_ID };
	}

	// Try to reuse previous uniform slot if its has the same data
	vt_slot_id id = _vt_try_reuse_uniform(pool, data);
	if (id != VT_INVALID_SLOT_ID) {
		return (vt_uniform) { id };
	}

	id = vt_pool_alloc(&pool->pool);
	assert(id != SG_INVALID_ID);

	vt_slot_index idx = vt_get_slot_index(id);
	_vt_uniform_block *block = &pool->blocks[idx];
	block->offset = pool->lenght;
	block->size = data->size;

	// Append uniform memory
	memcpy(pool->data + pool->lenght, data->ptr, data->size);
	pool->lenght += data->size;

	// If shared uniforms memory overflow, alloc a bigger memory block
	if (pool->lenght >= pool->capacity) {
		pool->capacity *= 2;
		void *tmp = realloc(pool->blocks, pool->capacity);
		if (!tmp) {
			abort(); // This should be unreachable
		}
		pool->blocks = tmp;
	}

	// Register uniform as used
	pool->used[pool->used_count] = id;
	pool->used_count += 1;

	return (vt_uniform) { id };
}

vt_error vt_apply_uniform(vt_uniforms_pool *pool, i32 slot, vt_uniform uniform) {
	assert(pool);
	if (uniform.id == VT_INVALID_SLOT_ID) {
		return VT_ERROR_GENERIC;
	}

	usize idx = vt_get_slot_index(uniform.id);
	if (pool->pool.slots[idx] != uniform.id) {
		return VT_ERROR_GENERIC;
	}

	_vt_uniform_block *block = &pool->blocks[idx];
	if (block->size > 0) {
		sg_range data_range = { pool->data + block->offset, block->size };
		sg_apply_uniforms(slot, &data_range);
	}

	return VT_ERROR_NONE;
}
