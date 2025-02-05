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



		void receive_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format);

	protected:
		static void _bind_methods();
		void _notification(int what);

	private:
		Ref<Thread> thr;
		Ref<Semaphore> sem;
		bool mtx_exit_thread = false;

		String name;
		PackedStringArray groups;
		bool preview = false;

		Ref<Mutex> mtx_send;
		NDIlib_send_instance_t mtx_send_instance;
		bool mtx_sending = false;

		void create_sender();
		void destroy_sender();

		void set_preview(const bool p_state);
		bool get_preview() const;

		RenderingServer* rs;
		RenderingDevice* rd;

		Ref<Mutex> mtx_texture;
		PackedByteArray mtx_texture_data;
		Ref<RDTextureFormat> mtx_texture_format;

		Ref<Mutex> mtx_callback;
		void request_texture();
		void send_video_thread();
};
