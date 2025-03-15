/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ndi_finder.hpp"

#include <godot_cpp/classes/hashing_context.hpp>
#include <godot_cpp/classes/timer.hpp>

using namespace godot;

NDIFinder::NDIFinder() {
	thr.instantiate();
	mtx.instantiate();
	sem.instantiate();

	thr->start(callable_mp(this, &NDIFinder::find_sources_thread));
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
	mtx->lock();

	if (p_groups.is_empty()) {
		groups.resize(0);
	} else {
		groups = String(",").join(p_groups).utf8();
	}

	mtx_find_desc.p_groups = groups.ptr();
	mtx_rebuild_find = true;

	mtx->unlock();

	sem->post();
}

PackedStringArray NDIFinder::get_groups() const {
	if (groups.ptr() == nullptr) {
		return PackedStringArray();
	} else {
		return String::utf8(mtx_find_desc.p_groups).split(",");
	}
}

void NDIFinder::set_extra_ips(const PackedStringArray p_extra_ips) {
	mtx->lock();

	if (p_extra_ips.is_empty()) {
		extra_ips.resize(0);
	} else {
		extra_ips = String(",").join(p_extra_ips).utf8();
	}

	mtx_find_desc.p_extra_ips = extra_ips;
	mtx_rebuild_find = true;

	mtx->unlock();

	sem->post();
}

PackedStringArray NDIFinder::get_extra_ips() const {
	if (extra_ips.ptr() == nullptr) {
		return PackedStringArray();
	} else {
		return String::utf8(mtx_find_desc.p_extra_ips).split(",");
	}
}

TypedArray<VideoStreamNDI> NDIFinder::get_sources() const {
	mtx->lock();
	TypedArray<VideoStreamNDI> sources = mtx_sources.duplicate(true);
	mtx->unlock();
	return sources;
}

void NDIFinder::update() {
	sem->post();
}

void NDIFinder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_sources"), &NDIFinder::get_sources);
	ADD_SIGNAL(MethodInfo("sources_changed", PropertyInfo(Variant::ARRAY, "sources")));
	ADD_SIGNAL(MethodInfo("sources_found", PropertyInfo(Variant::ARRAY, "found_sources")));
	ADD_SIGNAL(MethodInfo("sources_gone", PropertyInfo(Variant::ARRAY, "gone_sources")));

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

void NDIFinder::_notification(int what) {
	switch (what) {
		case Node::NOTIFICATION_ENTER_TREE: {
			Timer *timer = memnew(Timer); // Will be freed on tree cleanup
			add_child(timer, false, Node::INTERNAL_MODE_FRONT);
			timer->connect("timeout", callable_mp(this, &NDIFinder::update));
			timer->start(1);
		} break;
	}
}

void NDIFinder::find_sources_thread() {
	NDIlib_find_instance_t find = nullptr;

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
			if (find != nullptr) {
				ndi->find_destroy(find);
				find = nullptr;
			}

			mtx->lock(); // make sure find_desc doesn't change during use

			mtx_rebuild_find = false;
			find = ndi->find_create_v2(&mtx_find_desc);

			mtx->unlock();
		}

		if (find == nullptr) {
			continue;
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

		mtx->lock();
		TypedArray<VideoStreamNDI> sources;
		TypedArray<VideoStreamNDI> found_sources;
		TypedArray<VideoStreamNDI> gone_sources;
		TypedArray<VideoStreamNDI> existing_sources = mtx_sources.duplicate(true);
		mtx->unlock();

		for (int i = 0; i < num_sources; i++) {
			VideoStreamNDI *source = memnew(VideoStreamNDI(sources_pointer[i]));
			sources.push_back(source);
		}

		for (int i = 0; i < sources.size(); i++) {
			auto *source = Object::cast_to<VideoStreamNDI>(sources[i]);

			bool has = false;

			for (int j = 0; j < existing_sources.size(); j++) {
				auto *existing_source = Object::cast_to<VideoStreamNDI>(existing_sources[j]);

				if (VideoStreamNDI::equal(source, existing_source)) {
					has = true;
					break;
				}
			}

			if (!has) {
				found_sources.append(source);
			}
		}

		for (int j = 0; j < existing_sources.size(); j++) {
			auto *existing_source = Object::cast_to<VideoStreamNDI>(existing_sources[j]);

			bool has = false;

			for (int i = 0; i < sources.size(); i++) {
				auto *source = Object::cast_to<VideoStreamNDI>(sources[i]);

				if (VideoStreamNDI::equal(existing_source, source)) {
					has = true;
					break;
				}
			}

			if (!has) {
				gone_sources.append(existing_source);
			}
		}

		mtx->lock();
		mtx_sources.assign(sources);
		mtx->unlock();

		call_deferred("emit_signal", "sources_changed", sources);

		if (found_sources.size() != 0) {
			call_deferred("emit_signal", "sources_found", found_sources);
		}

		if (gone_sources.size() != 0) {
			call_deferred("emit_signal", "sources_gone", gone_sources);
		}
	}

	hasher.unref();
	if (find != nullptr) {
		ndi->find_destroy(find);
	}
}
