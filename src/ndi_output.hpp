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

	protected:
		static void _bind_methods();
		void _notification(int what);

	private:
		NDIlib_send_instance_t send;
		bool sending = false;

		void create_sender();
		void destroy_sender();
		void send_video(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format = Ref<RDTextureFormat>());

		RenderingServer* rs;
		RenderingDevice* rd;

		String name;
		PackedStringArray groups;

};
