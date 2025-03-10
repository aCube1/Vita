#include "vita/gpu.h"

#include "common.h"
#include "log.h"

struct vt_gpu {
	sg_image white_img;
	sg_sampler nearest_smp;
};

static struct vt_gpu _gpu = { 0 };

vt_error vt_gpu_init_resources(void) {
	_gpu.white_img = vt_gpu_get_white_image();
	_gpu.nearest_smp = vt_gpu_get_nearest_sampler();

	if (_gpu.white_img.id == SG_INVALID_ID || _gpu.nearest_smp.id == SG_INVALID_ID) {
		return VT_ERROR_GENERIC;
	}

	return VT_ERROR_NONE;
}

void vt_gpu_clean_resources(void) {
	if (sg_query_image_state(_gpu.white_img) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_image(_gpu.white_img);
	}
	if (sg_query_sampler_state(_gpu.nearest_smp) != SG_RESOURCESTATE_INVALID) {
		sg_destroy_sampler(_gpu.nearest_smp);
	}
}

sg_image vt_gpu_get_white_image(void) {
	if (sg_query_image_state(_gpu.white_img) == SG_RESOURCESTATE_VALID) {
		return _gpu.white_img;
	}

	u32 pixels[4] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	sg_image_desc imgdesc = { 0 };
	imgdesc.width = 2;
	imgdesc.height = 2;
	imgdesc.pixel_format = SG_PIXELFORMAT_RGBA8;
	imgdesc.data.subimage[0][0] = SG_RANGE(pixels);
	imgdesc.label = "vt_gpu.white_img";

	_gpu.white_img = sg_make_image(&imgdesc);
	if (sg_query_image_state(_gpu.white_img) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make default white image");
		sg_destroy_image(_gpu.white_img);
		_gpu.white_img.id = SG_INVALID_ID;
	}

	return _gpu.white_img;
}

sg_sampler vt_gpu_get_nearest_sampler(void) {
	if (sg_query_sampler_state(_gpu.nearest_smp) == SG_RESOURCESTATE_VALID) {
		return _gpu.nearest_smp;
	}

	sg_sampler_desc smpdesc = { 0 };
	smpdesc.min_filter = SG_FILTER_NEAREST;
	smpdesc.mag_filter = SG_FILTER_NEAREST;
	smpdesc.mipmap_filter = SG_FILTER_NEAREST;
	smpdesc.label = "vt_gpu.nearest_smp";

	_gpu.nearest_smp = sg_make_sampler(&smpdesc);
	if (sg_query_sampler_state(_gpu.nearest_smp) != SG_RESOURCESTATE_VALID) {
		LOG_WARN("[GPU] > Failed to make default nearest sampler");
		sg_destroy_sampler(_gpu.nearest_smp);
		_gpu.nearest_smp.id = SG_INVALID_ID;
	}

	return _gpu.nearest_smp;
}
