/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ndi.hpp"

#include <godot_cpp/classes/audio_effect_capture.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/rd_texture_format.hpp>

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

	void set_audio_bus(const StringName &p_bus);
	StringName get_audio_bus() const;

	void set_output_video(const bool p_state);
	bool is_outputting_video() const;

	void set_output_editor(const bool p_state);
	bool is_outputting_editor() const;

	PackedStringArray _get_configuration_warnings() const;

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _validate_property(PropertyInfo &p_property) const;

private:
	CharString name;
	CharString groups;
	StringName audio_bus = StringName("None");
	bool output_video = true;
	bool output_editor = false;

	NDIlib_send_instance_t send = nullptr;
	NDIlib_send_create_t send_desc = NDIlib_send_create_t(nullptr, nullptr, false, false);

	void rebuild_sender();
	void create_sender();
	void destroy_sender();

	void register_hooks();
	void unregister_hooks();

	void busses_changed();
	Ref<AudioEffectCapture> get_audio_capture() const;
	PackedVector2Array audio_interleaved;
	PackedFloat32Array audio_planar;

	void send_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, int64_t p_viewport_rid);
	void send_audio();
};
