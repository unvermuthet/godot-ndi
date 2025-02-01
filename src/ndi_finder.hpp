/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "includes.hpp"

using namespace godot;

class NDIFinder : public Node {
	GDCLASS(NDIFinder, Node)

	Ref<Thread> thr;
	Ref<Mutex> mtx;
	Ref<Semaphore> sem;

	TypedArray<VideoStreamNDI> mtx_sources;
	bool mtx_exit_thread = false;
	bool mtx_rebuild_find = false;
	NDIlib_find_create_t mtx_find_desc;

	CharString groups;
	CharString extra_ips;

	void process_thread();

	protected:
		static void _bind_methods();

	public:
		NDIFinder();
		~NDIFinder();

		void _process(double p_delta) override;

		TypedArray<VideoStreamNDI> get_sources() const;

		void set_show_local_sources(const bool state);
		bool get_show_local_sources() const;

		void set_groups(const PackedStringArray groups);
		PackedStringArray get_groups() const;

		void set_extra_ips(const PackedStringArray extra_ips);
		PackedStringArray get_extra_ips() const;
};
