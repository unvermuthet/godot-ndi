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

class NDIOutput : public Node {
	GDCLASS(NDIOutput, Node)

public:
	NDIOutput();
	~NDIOutput();

	void set_name(const String p_name);
	String get_name() const;

	void set_groups(const PackedStringArray p_groups);
	PackedStringArray get_groups() const;

	void set_output_editor(const bool p_state);
	bool is_outputting_editor() const;

protected:
	static void _bind_methods();
	void _notification(int what);

private:
	CharString name;
	CharString groups;
	bool output_editor;

	Ref<Thread> thr;
	Ref<Semaphore> sem;
	bool mtx_exit_thread;

	Ref<Mutex> mtx_send;
	NDIlib_send_create_t mtx_send_desc;
	bool mtx_rebuild_send;

	void register_viewport();
	void unregister_viewport();
	void receive_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, int64_t p_viewport_rid);

	Ref<Mutex> mtx_texture;
	PackedByteArray mtx_texture_data;
	Ref<RDTextureFormat> mtx_texture_format;

	void send_video_thread();
};