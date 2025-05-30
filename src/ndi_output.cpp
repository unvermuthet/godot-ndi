/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ndi_output.hpp"

#include "editor_plugin_ndi.hpp"
#include "viewport_texture_router.hpp"

#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/viewport.hpp>

using namespace godot;

class ViewportTextureRouter;

NDIOutput::NDIOutput() {
	send_desc.clock_video = false;
	send_desc.clock_audio = false;
	output_editor = false;

	AudioServer::get_singleton()->connect("bus_layout_changed", callable_mp(this, &NDIOutput::busses_changed));
}

NDIOutput::~NDIOutput() {
	AudioServer::get_singleton()->disconnect("bus_layout_changed", callable_mp(this, &NDIOutput::busses_changed));
}

void NDIOutput::set_name(const String p_name) {
	if (p_name.is_empty()) {
		name.resize(0);
	} else {
		String suffix = Engine::get_singleton()->is_editor_hint() ? " [Editor]" : "";
		name = (p_name + suffix).utf8();
	}

	send_desc.p_ndi_name = name.ptr();
	rebuild_sender();
}

String NDIOutput::get_name() const {
	if (name.ptr() == nullptr) {
		return String("");
	} else {
		return String::utf8(name.ptr(), name.length()).replace(" [Editor]", "");
	}
}

void NDIOutput::set_groups(const PackedStringArray p_groups) {
	if (p_groups.is_empty()) {
		groups.resize(0);
	} else {
		groups = String(",").join(p_groups).utf8();
	}

	send_desc.p_groups = groups.ptr();
	rebuild_sender();
}

PackedStringArray NDIOutput::get_groups() const {
	if (groups.ptr() == nullptr) {
		return PackedStringArray();
	} else {
		return String::utf8(groups.ptr(), groups.length()).split(",", true);
	}
}

void NDIOutput::set_output_video(const bool p_state) {
	if (p_state == output_video) {
		return;
	}

	output_video = p_state;
	rebuild_sender();
}

bool NDIOutput::is_outputting_video() const {
	return output_video;
}

void NDIOutput::set_output_editor(const bool p_state) {
	if (p_state == output_editor) {
		return;
	}

	output_editor = p_state;
	rebuild_sender();
}

bool NDIOutput::is_outputting_editor() const {
	return output_editor;
}

void NDIOutput::set_audio_bus(const StringName &p_bus) {
	AudioServer::get_singleton()->lock();
	audio_bus = p_bus;
	AudioServer::get_singleton()->unlock();

	update_configuration_warnings();
	rebuild_sender();
}

StringName NDIOutput::get_audio_bus() const {
	// Avoid returning bus that doesn't exist
	for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
		if (AudioServer::get_singleton()->get_bus_name(i) == audio_bus) {
			return audio_bus;
		}
	}

	// Default and fallback if bus was deleted
	return StringName("None");
}

PackedStringArray NDIOutput::_get_configuration_warnings() const {
	auto warnings = PackedStringArray();

	if (get_audio_bus() != StringName("None") && get_audio_capture().is_null()) {
		warnings.append("The selected Audio Bus needs to have a Capture Effect added");
	}

	return warnings;
}

void NDIOutput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_name", "p_name"), &NDIOutput::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &NDIOutput::get_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

	ClassDB::bind_method(D_METHOD("set_groups", "p_groups"), &NDIOutput::set_groups);
	ClassDB::bind_method(D_METHOD("get_groups"), &NDIOutput::get_groups);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "groups"), "set_groups", "get_groups");

	ClassDB::bind_method(D_METHOD("set_audio_bus", "p_bus"), &NDIOutput::set_audio_bus);
	ClassDB::bind_method(D_METHOD("get_audio_bus"), &NDIOutput::get_audio_bus);
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "audio_bus", PROPERTY_HINT_ENUM, ""), "set_audio_bus", "get_audio_bus");

	ClassDB::bind_method(D_METHOD("set_output_video", "p_state"), &NDIOutput::set_output_video);
	ClassDB::bind_method(D_METHOD("is_outputting_video"), &NDIOutput::is_outputting_video);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_video_output"), "set_output_video", "is_outputting_video");

	ClassDB::bind_method(D_METHOD("set_output_editor", "p_state"), &NDIOutput::set_output_editor);
	ClassDB::bind_method(D_METHOD("is_outputting_editor"), &NDIOutput::is_outputting_editor);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_editor_output"), "set_output_editor", "is_outputting_editor");

	ADD_NDI_BUTTON
}

void NDIOutput::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			create_sender();
		} break;
		case NOTIFICATION_EXIT_TREE: {
			destroy_sender();
		} break;
	}
}

void NDIOutput::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == StringName("audio_bus")) {
		String options = "None,";
		for (int i = 0; i < AudioServer::get_singleton()->get_bus_count(); i++) {
			options += AudioServer::get_singleton()->get_bus_name(i);
			options += ",";
		}

		p_property.hint_string = options;
	}
}

void NDIOutput::busses_changed() {
	notify_property_list_changed();
}

Ref<AudioEffectCapture> NDIOutput::get_audio_capture() const {
	if (get_audio_bus() == StringName("None")) {
		return Ref<AudioEffectCapture>();
	}

	int bus_idx = AudioServer::get_singleton()->get_bus_index(get_audio_bus());

	for (int i = 0; i < AudioServer::get_singleton()->get_bus_effect_count(bus_idx); i++) {
		if (AudioServer::get_singleton()->get_bus_effect(bus_idx, i)->is_class("AudioEffectCapture")) {
			return AudioServer::get_singleton()->get_bus_effect(bus_idx, i);
		}
	}

	return Ref<AudioEffectCapture>();
}

void NDIOutput::rebuild_sender() {
	if (!is_inside_tree()) {
		return;
	}

	destroy_sender();
	create_sender();
}

void NDIOutput::create_sender() {
	if (Engine::get_singleton()->is_editor_hint() && !is_outputting_editor()) {
		return;
	}

	if (send != nullptr || send_desc.p_ndi_name == nullptr) {
		return;
	}

	send = ndi->send_create(&send_desc);

	// TODO: Expose this error somehow to enable user code to react to it. Signal?
	ERR_FAIL_NULL_EDMSG(send, "NDI: Source creation failed. Perhaps name is already in use.");

	register_hooks();
}

void NDIOutput::destroy_sender() {
	if (send != nullptr) {
		unregister_hooks();
		ndi->send_destroy(send);
		send = nullptr;
	}
}

void NDIOutput::register_hooks() {
	if (get_audio_bus() != StringName("None")) {
		RenderingServer::get_singleton()->connect("frame_post_draw", callable_mp(this, &NDIOutput::send_audio));
	}

	if (is_outputting_video()) {
		auto *vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
		ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");
		ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");

		vp_texture_router->connect("texture_arrived", callable_mp(this, &NDIOutput::send_texture));
		vp_texture_router->add_viewport(get_viewport());
	}

	DisplayServer::get_singleton()->register_additional_output(this);
}

void NDIOutput::unregister_hooks() {
	SIGNAL_DISCONNECT(RenderingServer::get_singleton(), "frame_post_draw", callable_mp(this, &NDIOutput::send_audio));

	auto *vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");

	SIGNAL_DISCONNECT(vp_texture_router, "texture_arrived", callable_mp(this, &NDIOutput::send_texture));
	vp_texture_router->remove_viewport(get_viewport());

	DisplayServer::get_singleton()->unregister_additional_output(this);
}

void NDIOutput::send_audio() {
	ERR_FAIL_NULL_MSG(send, "Send instance unconfigured");
	ERR_FAIL_COND_MSG(get_audio_bus() == StringName("None"), "No audio bus selected");

	Ref<AudioEffectCapture> audio_capture = get_audio_capture();

	if (audio_capture.is_null()) {
		return;
	}

	if (AudioServer::get_singleton()->get_speaker_mode() != AudioServer::SPEAKER_MODE_STEREO) {
		WARN_PRINT_ONCE_ED("NDI: Audio devices with more than two channels are unsupported because Godot's AudioEffectCapture breaks.");
		return;
	}

	NDIlib_audio_frame_v3_t frame;
	frame.no_channels = 2;
	frame.sample_rate = AudioServer::get_singleton()->get_mix_rate(); // AudioEffectCapture doesn't resample to sample rate in project settings
	frame.no_samples = Math::clamp<int>(audio_capture->get_frames_available(), 0, 4096);
	frame.channel_stride_in_bytes = frame.no_samples * sizeof(float);

	// Convert from interleaved samples to planar. SDK has a util for this but results in a slightly panned signal.

	audio_interleaved = audio_capture->get_buffer(frame.no_samples);
	audio_planar.resize(frame.no_samples * frame.no_channels);

	for (int i = 0; i < audio_planar.size(); i++) {
		audio_planar[i] = audio_interleaved[i % frame.no_samples][i / frame.no_samples];
	}

	frame.p_data = (uint8_t *)audio_planar.ptr();
	ndi->send_send_audio_v3(send, &frame);
}

void NDIOutput::send_texture(PackedByteArray p_texture_data, const Ref<RDTextureFormat> &p_texture_format, int64_t p_viewport_rid) {
	if (!is_inside_tree() || p_viewport_rid != get_viewport()->get_viewport_rid().get_id()) {
		return; // Not my request
	}

	ERR_FAIL_NULL_MSG(send, "Send instance unconfigured");
	ERR_FAIL_COND_MSG(p_texture_format.is_null() || p_texture_data.is_empty(), "Empty viewport texture received");
	ERR_FAIL_COND_MSG(p_texture_format->get_format() != RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM, "Viewport texture format isn't DATA_FORMAT_R8G8B8A8_UNORM");
	ERR_FAIL_COND_MSG(p_texture_format->get_width() * p_texture_format->get_height() * 4 != p_texture_data.size(), "Unexpected viewport texture size");

	if (Engine::get_singleton()->get_frames_per_second() > 65.0 && ProjectSettings::get_singleton()->get_setting("godot_ndi/enable_fps_warning", true)) {
		if (Engine::get_singleton()->is_editor_hint()) {
			WARN_PRINT_ONCE_ED("NDI doesn't support frame rates higher than 60 FPS. Consider lowering your refresh rate when outputting from the Editor.");
		} else {
			WARN_PRINT_ONCE_ED("NDI doesn't support frame rates higher than 60 FPS. Consider setting Max FPS to 60 in Project Settings.");
		}
	}

	NDIlib_video_frame_v2_t frame;
	frame.xres = p_texture_format->get_width();
	frame.yres = p_texture_format->get_height();
	frame.frame_rate_N = (int)Engine::get_singleton()->get_frames_per_second();
	frame.frame_rate_D = 1;
	frame.FourCC = get_viewport()->has_transparent_background() ? NDIlib_FourCC_type_RGBA : NDIlib_FourCC_type_RGBX;
	frame.p_data = (uint8_t *)p_texture_data.ptr();

	ndi->send_send_video_v2(send, &frame);
	p_texture_data.resize(0);
}
