#include "ndi_find.h"
#include "register_types.h"

void NDIFind::_bind_methods() {
}

NDIFind::NDIFind() {
	NDIlib_find_create_t find_desc;
	find = ndi->NDIlib_find_create_v2(&find_desc);
}

NDIFind::~NDIFind() {
	ndi->NDIlib_find_destroy(find);
}

void NDIFind::set_sources_settings(const bool show_local_sources, const PackedStringArray groups, const PackedStringArray extra_ips) {
	NDIlib_find_create_t find_desc = { show_local_sources, NULL, NULL };
	
	if (groups.size() > 0) {
		find_desc.p_groups = String(",").join(groups).utf8().ptr();
	}

	if (extra_ips.size() > 0) {
		find_desc.p_extra_ips = String(",").join(extra_ips).utf8().ptr();
	}

	ndi->NDIlib_find_destroy(find);
	find = ndi->NDIlib_find_create(&find_desc);
}

TypedArray<NDISource> NDIFind::get_sources() const {
	TypedArray<NDISource> sources;

	uint32_t num_sources = 0;
	const NDIlib_source_t *sources_pointer = ndi->NDIlib_find_get_current_sources(find, &num_sources);

	for (int i = 0; i < num_sources; i++) {
		NDISource source = NDISource(sources_pointer[i]);
		sources.push_back(&source);
	}

	return sources;
}