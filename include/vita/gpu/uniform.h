#ifndef _VT_VITA_GPU_UNIFORM_H
#define _VT_VITA_GPU_UNIFORM_H

#include "common.h"
#include "pool.h"
#include "sokol/sokol_gfx.h"

enum {
	VT_UNIFORM_BINDSLOTS_COUNT = SG_MAX_UNIFORMBLOCK_BINDSLOTS,
};

typedef struct vt_uniform {
	vt_slot_id id;
} vt_uniform;

typedef struct _vt_uniform_block {
	usize offset;
	usize size;
} _vt_uniform_block;

typedef struct vt_uniforms_pool {
	usize capacity;
	usize lenght;
	i32 used_count;
	vt_pool pool;
	u8 *data;
	vt_slot_id *used;
	_vt_uniform_block *blocks;
} vt_uniforms_pool;

vt_error vt_init_uniforms_pool(vt_uniforms_pool *pool, u16 size);
void vt_destroy_uniforms_pool(vt_uniforms_pool *pool);
void vt_reset_uniforms_pool(vt_uniforms_pool *pool);

vt_uniform vt_create_uniform(vt_uniforms_pool *pool, const sg_range *data);
vt_error vt_apply_uniform(vt_uniforms_pool *pool, i32 ubslot, vt_uniform uniform);

#endif
