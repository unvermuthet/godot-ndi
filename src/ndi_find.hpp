/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "ndi.hpp"
#include "video_stream_ndi.hpp"

using namespace godot;

class NDIFind : public RefCounted {
	GDCLASS(NDIFind, RefCounted)

	CharString groups;
	CharString extra_ips;

	private:
		NDIlib_find_create_t find_desc;
		NDIlib_find_instance_t find;

	protected:
		static void _bind_methods();

	public:
		NDIFind();
		~NDIFind();

		void set_show_local_sources(const bool state);
		bool get_show_local_sources() const;

		void set_groups(const PackedStringArray groups);
		PackedStringArray get_groups() const;

		void set_extra_ips(const PackedStringArray extra_ips);
		PackedStringArray get_extra_ips() const;

		TypedArray<VideoStreamNDI> get_sources() const;
};
