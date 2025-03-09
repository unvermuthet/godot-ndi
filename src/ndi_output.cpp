/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

NDIOutput::NDIOutput() {
	send = nullptr;
	send_desc.clock_video = false;
	send_desc.clock_audio = false;
	output_editor = false;
}

NDIOutput::~NDIOutput() {
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

void NDIOutput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_name", "p_name"), &NDIOutput::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &NDIOutput::get_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

	ClassDB::bind_method(D_METHOD("set_groups", "p_groups"), &NDIOutput::set_groups);
	ClassDB::bind_method(D_METHOD("get_groups"), &NDIOutput::get_groups);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "groups"), "set_groups", "get_groups");

	ClassDB::bind_method(D_METHOD("set_output_editor", "p_state"), &NDIOutput::set_output_editor);
	ClassDB::bind_method(D_METHOD("is_outputting_editor"), &NDIOutput::is_outputting_editor);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_editor_output"), "set_output_editor", "is_outputting_editor");
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
	if (send == nullptr) {
		// TODO: Expose this error somehow to enable user code to react to it. Signal?
		WARN_PRINT_ED("NDI: Source creation failed. Perhaps name is already in use.");
		return;
	}

	register_viewport();
}

void NDIOutput::destroy_sender() {
	if (send != nullptr) {
		unregister_viewport();
		ndi->send_destroy(send);
		send = nullptr;
	}
}

void NDIOutput::register_viewport() {
	ViewportTextureRouter *vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");

	vp_texture_router->connect("texture_arrived", callable_mp(this, &NDIOutput::receive_texture), CONNECT_REFERENCE_COUNTED);
	vp_texture_router->add_viewport(get_viewport());
}

void NDIOutput::unregister_viewport() {
	ViewportTextureRouter *vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");

	vp_texture_router->disconnect("texture_arrived", callable_mp(this, &NDIOutput::receive_texture));
	vp_texture_router->remove_viewport(get_viewport());
}

void NDIOutput::receive_texture(PackedByteArray p_texture_data, const Ref<RDTextureFormat> &p_texture_format, int64_t p_viewport_rid) {
	if (!is_inside_tree() || p_viewport_rid != get_viewport()->get_viewport_rid().get_id()) {
		return; // Not my request
	}

	if (send == nullptr || p_texture_format.is_null() || p_texture_data.is_empty()) {
		return;
	}

	if (Engine::get_singleton()->get_frames_per_second() > 65.0) {
		WARN_PRINT_ONCE_ED("NDI doesn't support frame rates higher than 60 FPS. Consider setting Max FPS to 60 in Project Settings or lowering your refresh rate when outputting from the Editor.");
	}

	ERR_FAIL_COND_MSG(p_texture_format->get_format() != RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM, "Viewport texture format isn't DATA_FORMAT_R8G8B8A8_UNORM");
	ERR_FAIL_COND_MSG(p_texture_format->get_width() * p_texture_format->get_height() * 4 != p_texture_data.size(), "Unexpected texture size");

	NDIlib_video_frame_v2_t video_frame = {};
	video_frame.xres = p_texture_format->get_width();
	video_frame.yres = p_texture_format->get_height();
	video_frame.frame_rate_N = (int)Engine::get_singleton()->get_frames_per_second();
	video_frame.frame_rate_D = 1;
	video_frame.FourCC = get_viewport()->has_transparent_background() ? NDIlib_FourCC_type_RGBA : NDIlib_FourCC_type_RGBX;
	video_frame.p_data = (uint8_t *)p_texture_data.ptr();

	ndi->send_send_video_v2(send, &video_frame);
	p_texture_data.resize(0);
}
