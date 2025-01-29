/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/semaphore.hpp>
#include <godot_cpp/classes/hashing_context.hpp>
#include "ndi.hpp"
#include "video_stream_ndi.hpp"

using namespace godot;

class NDIFinder : public Node {
	GDCLASS(NDIFinder, Node)

	NDIlib_find_create_t find_desc;
	CharString groups;
	CharString extra_ips;

	Ref<Thread> thr;
	Ref<Mutex> mtx;
	Ref<Semaphore> sem;

	bool exit_thread = false;
	bool rebuild_find = false;

	void process_thread();

	TypedArray<VideoStreamNDI> mtx_sources;

	protected:
		static void _bind_methods();

	public:
		NDIFinder();
		~NDIFinder();

		void _ready() override;
		void _exit_tree() override;
		void _process(double delta) override;

		TypedArray<VideoStreamNDI> get_sources() const;

		void set_show_local_sources(const bool state);
		bool get_show_local_sources() const;

		void set_groups(const PackedStringArray groups);
		PackedStringArray get_groups() const;

		void set_extra_ips(const PackedStringArray extra_ips);
		PackedStringArray get_extra_ips() const;
};
