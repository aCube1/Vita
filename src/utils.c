#include "utils.h"

#include <assert.h>
#include <string.h>

void vt_clear_mem(void *ptr, usize size) {
	assert(ptr && size > 0);
	memset(ptr, 0, size);
}
