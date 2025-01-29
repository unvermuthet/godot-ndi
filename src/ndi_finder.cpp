#include "includes.hpp"

void NDIFinder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_sources"), &NDIFinder::get_sources);
	ADD_SIGNAL(MethodInfo("sources_changed"));

	ClassDB::bind_method(D_METHOD("set_show_local_sources", "show_local_sources"), &NDIFinder::set_show_local_sources);
	ClassDB::bind_method(D_METHOD("get_show_local_sources"), &NDIFinder::get_show_local_sources);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_local_sources"), "set_show_local_sources", "get_show_local_sources");

	ClassDB::bind_method(D_METHOD("set_groups", "groups"), &NDIFinder::set_groups);
	ClassDB::bind_method(D_METHOD("get_groups"), &NDIFinder::get_groups);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "groups"), "set_groups", "get_groups");

	ClassDB::bind_method(D_METHOD("set_extra_ips", "extra_ips"), &NDIFinder::set_extra_ips);
	ClassDB::bind_method(D_METHOD("get_extra_ips"), &NDIFinder::get_extra_ips);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "extra_ips"), "set_extra_ips", "get_extra_ips");
	
}

NDIFinder::NDIFinder() {
	thr.instantiate();
	mtx.instantiate();
	sem.instantiate();

	thr->start(callable_mp(this, &NDIFinder::process_thread));
}

NDIFinder::~NDIFinder() {
	exit_thread = true;
	sem->post();

	if (thr.is_valid() && thr->is_alive()) {
		thr->wait_to_finish();
	}

	thr.unref();
	mtx.unref();
	sem.unref();
}

TypedArray<VideoStreamNDI> NDIFinder::get_sources() const {
	mtx->lock();
	TypedArray<VideoStreamNDI> sources = mtx_sources.duplicate(true);
	mtx->unlock();
	return sources;
}

void NDIFinder::_ready() {}

void NDIFinder::_exit_tree() {}

void NDIFinder::_process(double delta) {
	sem->post();
}

void NDIFinder::process_thread() {
	NDIlib_find_instance_t find = ndi->find_create_v2(&find_desc);

	Ref<HashingContext> hasher = memnew(HashingContext);
	hasher->start(HashingContext::HASH_SHA256);
	PackedByteArray previous_hash = hasher->finish(); // Start with default hash

	while (true) {
		sem->wait();

		if (exit_thread) {
			break;
		}

		if (rebuild_find) {
			ndi->find_destroy(find);
			find = ndi->find_create_v2(&find_desc);
			rebuild_find = false;
		}

		TypedArray<VideoStreamNDI> new_sources;
		hasher->start(HashingContext::HASH_SHA256);

		uint32_t num_sources = 0;
		const NDIlib_source_t *sources_pointer = ndi->find_get_current_sources(find, &num_sources);

		for (int i = 0; i < num_sources; i++) {
			Ref<VideoStreamNDI> source = memnew(VideoStreamNDI(sources_pointer[i]));
			hasher->update(source->get_name().to_utf8_buffer());
			hasher->update(source->get_url().to_utf8_buffer());
			new_sources.push_back(source);
		}

		PackedByteArray current_hash = hasher->finish();
		if (current_hash != previous_hash) {
			previous_hash = current_hash;

			mtx->lock();
			mtx_sources.assign(new_sources);
			mtx->unlock();

			call_deferred("emit_signal", "sources_changed");
		}

	}

	ndi->find_destroy(find);
	hasher.unref();
}

void NDIFinder::set_show_local_sources(const bool p_state) {
	find_desc.show_local_sources = p_state;
	rebuild_find = true;
}

bool NDIFinder::get_show_local_sources() const {
	return find_desc.show_local_sources;
}

void NDIFinder::set_groups(const PackedStringArray p_groups) {
	if (p_groups.is_empty()) {
		groups = NULL;
	} else {
		groups = String(",").join(p_groups).utf8();
	}

	find_desc.p_groups = groups;
	rebuild_find = true;
}

PackedStringArray NDIFinder::get_groups() const {
	if (find_desc.p_groups == NULL) {
		return PackedStringArray();
	}

	return String::utf8(find_desc.p_groups).split(",");
}

void NDIFinder::set_extra_ips(const PackedStringArray p_extra_ips) {
	if (p_extra_ips.is_empty()) {
		extra_ips = NULL;
	} else {
		extra_ips = String(",").join(p_extra_ips).utf8();
	}

	find_desc.p_extra_ips = extra_ips;
	rebuild_find = true;
}

PackedStringArray NDIFinder::get_extra_ips() const {
	if (find_desc.p_extra_ips == NULL) {
		return PackedStringArray();
	}

	return String::utf8(find_desc.p_extra_ips).split(",");
}
