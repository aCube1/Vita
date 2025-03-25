#ifndef _VT_CORE_H
#define _VT_CORE_H

#include "vita/gpu/renderer.h"
#include "vita/window.h"

typedef struct vt_core {
	bool is_active;
	vt_window window;
	vt_renderer *renderer;
} vt_core;

vt_error vt_setup_core(void);
void vt_shutdown_core(void);

vt_core *vt_get_engine_core(void);

#endif
