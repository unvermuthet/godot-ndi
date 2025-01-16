#include "Processing.NDI.Lib.h"
#include "ndi.h"

void NDI::_bind_methods() {
}

NDI::NDI() {
	NDIlib_find_create_t find_desc = {};
	find = NDIlib_find_create(&find_desc);
}

NDI::~NDI() {
	NDIlib_find_destroy(find);
}

void NDI::set_sources_settings(const bool show_local_sources, const PackedStringArray groups, const PackedStringArray extra_ips) {
	NDIlib_find_create_t find_desc = { show_local_sources, NULL, NULL };
	
	if (groups.size() > 0) {
		find_desc.p_groups = String(",").join(groups).utf8().ptr();
	}

	if (extra_ips.size() > 0) {
		find_desc.p_extra_ips = String(",").join(extra_ips).utf8().ptr();
	}

	NDIlib_find_destroy(find);
	find = NDIlib_find_create(&find_desc);
}

TypedArray<NDISource> NDI::get_sources() const {
	return TypedArray<NDISource>();
}

Ref<NDIConnection> NDI::connect(const String source_name, const bool low_bandwidth) const {
	return Ref<NDIConnection>();
}