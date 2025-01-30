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
	mtx_exit_thread = true;
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

void NDIFinder::_process(double p_delta) {
	sem->post();
}

void NDIFinder::process_thread() {
	mtx->lock(); // make sure find_desc doesn't change during use
	NDIlib_find_instance_t find = ndi->find_create_v2(&mtx_find_desc);
	mtx->unlock();

	Ref<HashingContext> hasher = memnew(HashingContext);
	hasher->start(HashingContext::HASH_SHA256);
	PackedByteArray previous_hash = hasher->finish(); // Start with default hash

	while (true) {
		sem->wait();

		mtx->lock();
		bool exit_thread = mtx_exit_thread;
		bool rebuild_find = mtx_rebuild_find;
		mtx->unlock();

		if (exit_thread) {
			break;
		}

		if (rebuild_find) {
			ndi->find_destroy(find);

			mtx->lock(); // make sure find_desc doesn't change during use
			mtx_rebuild_find = false;
			NDIlib_find_create_t find_desc = mtx_find_desc;
			mtx->unlock();

			find = ndi->find_create_v2(&mtx_find_desc);
		}

		uint32_t num_sources = 0;
		const NDIlib_source_t *sources_pointer = ndi->find_get_current_sources(find, &num_sources);
		hasher->start(HashingContext::HASH_SHA256);

		for (int i = 0; i < num_sources; i++) {
			hasher->update(String::utf8(sources_pointer[i].p_ndi_name).to_utf8_buffer());
			hasher->update(String::utf8(sources_pointer[i].p_url_address).to_utf8_buffer());
		}

		PackedByteArray current_hash = hasher->finish();
		if (current_hash == previous_hash) {
			continue;
		}

		previous_hash = current_hash;

		TypedArray<VideoStreamNDI> new_sources;
		for (int i = 0; i < num_sources; i++) {
			new_sources.push_back(memnew(VideoStreamNDI(sources_pointer[i])));
		}

		mtx->lock();
		mtx_sources.assign(new_sources);
		mtx->unlock();

		call_deferred("emit_signal", "sources_changed");
	}

	ndi->find_destroy(find);
	hasher.unref();
}

void NDIFinder::set_show_local_sources(const bool p_state) {
	mtx->lock();
	mtx_find_desc.show_local_sources = p_state;
	mtx_rebuild_find = true;
	mtx->unlock();
}

bool NDIFinder::get_show_local_sources() const {
	return mtx_find_desc.show_local_sources;
}

void NDIFinder::set_groups(const PackedStringArray p_groups) {
	if (p_groups.is_empty()) {
		groups = NULL;
	} else {
		groups = String(",").join(p_groups).utf8();
	}

	mtx->lock();
	mtx_find_desc.p_groups = groups;
	mtx_rebuild_find = true;
	mtx->unlock();
}

PackedStringArray NDIFinder::get_groups() const {
	if (mtx_find_desc.p_groups == NULL) { // no need for mutex, thread doesn't write
		return PackedStringArray();
	}

	return String::utf8(mtx_find_desc.p_groups).split(",");
}

void NDIFinder::set_extra_ips(const PackedStringArray p_extra_ips) {
	if (p_extra_ips.is_empty()) {
		extra_ips = NULL;
	} else {
		extra_ips = String(",").join(p_extra_ips).utf8();
	}

	mtx->lock();
	mtx_find_desc.p_extra_ips = extra_ips;
	mtx_rebuild_find = true;
	mtx->unlock();
}

PackedStringArray NDIFinder::get_extra_ips() const {
	if (mtx_find_desc.p_extra_ips == NULL) {
		return PackedStringArray();
	}

	return String::utf8(mtx_find_desc.p_extra_ips).split(",");
}
