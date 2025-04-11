// clang-format Language: C
#ifndef _VT_POOL_H
#define _VT_POOL_H

#include "types.h"

enum {
	VT_INVALID_SLOT_ID = 0,
};

typedef u32 vt_slot_id;
typedef u16 vt_slot_index;

typedef struct vt_pool {
	u16 size;
	i32 queue_top;
	vt_slot_id *slots;
	u16 *gen_counters; // Generation counters
	i32 *free_queue;
} vt_pool;

vt_error vt_init_pool(vt_pool *pool, u16 size);
void vt_discard_pool(vt_pool *pool);
void vt_reset_pool(vt_pool *pool);

vt_slot_id vt_pool_alloc(vt_pool *pool);
void vt_pool_free(vt_pool *pool, vt_slot_id id);

vt_slot_index vt_get_slot_index(vt_slot_id id);

#endif
