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

	NDIlib_send_instance_t send;
	NDIlib_send_create_t send_desc;

	void update_sender();

	uint64_t timestamp = 0;

	void register_viewport();
	void unregister_viewport();

	void receive_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, int64_t p_viewport_rid);
};
