/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

NDIOutput::NDIOutput() {
	thr.instantiate();
	mtx_send.instantiate();
	mtx_texture.instantiate();
	sem.instantiate();

	output_editor = false;
	mtx_exit_thread = false;
	mtx_rebuild_send = true;

	mtx_send_desc.p_ndi_name = nullptr;
	mtx_send_desc.p_groups = nullptr;
	mtx_send_desc.clock_video = true;
	mtx_send_desc.clock_audio = false;

	thr->start(callable_mp(this, &NDIOutput::send_video_thread));
}

NDIOutput::~NDIOutput() {
	mtx_exit_thread = true;
	sem->post();

	if (thr.is_valid() && thr->is_alive()) {
		thr->wait_to_finish();
	}

	thr.unref();
	mtx_send.unref();
	mtx_texture.unref();
	sem.unref();
}

void NDIOutput::set_name(const String p_name) {
	mtx_send->lock();

	if (p_name.is_empty()) {
		name.resize(0);
	} else {
		String suffix = Engine::get_singleton()->is_editor_hint() ? " (Editor)" : " (Game)";
		name = (p_name + suffix).utf8();
	}

	mtx_send_desc.p_ndi_name = name.ptr();
	mtx_rebuild_send = true;

	mtx_send->unlock();

	sem->post();
}

String NDIOutput::get_name() const {
	if (name.ptr() == nullptr) {
		return String("");
	} else {
		return String::utf8(name.ptr(), name.length()).replace(" (Editor)", "").replace(" (Game)", "");
	}
}

void NDIOutput::set_groups(const PackedStringArray p_groups) {
	mtx_send->lock();

	if (p_groups.is_empty()) {
		groups.resize(0);
	} else {
		groups = String(",").join(p_groups).utf8();
	}

	mtx_send_desc.p_groups = groups.ptr();
	mtx_rebuild_send = true;

	mtx_send->unlock();

	sem->post();
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

	mtx_send->lock();
	output_editor = p_state;
	mtx_rebuild_send = true;
	mtx_send->unlock();

	sem->post();

	if (Engine::get_singleton()->is_editor_hint() && is_inside_tree()) { // state changed in the editor
		if (output_editor) { // previously unregistered
			register_viewport();
		} else { // previously registered
			unregister_viewport();
		}
	}
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
		case Node::NOTIFICATION_ENTER_TREE: {
			if (Engine::get_singleton()->is_editor_hint() && !is_outputting_editor()) {
				return;
			}

			register_viewport();
		} break;

		case Node::NOTIFICATION_EXIT_TREE: {
			if (Engine::get_singleton()->is_editor_hint() && !is_outputting_editor()) {
				return;
			}

			unregister_viewport();
		} break;
	}
}

void NDIOutput::register_viewport() {
	ViewportTextureRouter *vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");

	vp_texture_router->connect("texture_arrived", callable_mp(this, &NDIOutput::receive_texture));
	vp_texture_router->add_viewport(get_viewport());
}

void NDIOutput::unregister_viewport() {
	ViewportTextureRouter *vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");

	vp_texture_router->disconnect("texture_arrived", callable_mp(this, &NDIOutput::receive_texture));
	vp_texture_router->remove_viewport(get_viewport());
}

void NDIOutput::receive_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, int64_t p_viewport_rid) {
	if (!is_inside_tree() || p_viewport_rid != get_viewport()->get_viewport_rid().get_id()) {
		return; // Not my request
	}

	mtx_texture->lock();
	mtx_texture_data = p_data;
	mtx_texture_format = p_format;
	mtx_texture_transparent = get_viewport()->has_transparent_background();
	mtx_texture->unlock();
	sem->post();
}

void NDIOutput::send_video_thread() {
	NDIlib_send_instance_t send = nullptr;
	PackedByteArray texture_buffer;
	Ref<RDTextureFormat> texture_format;
	bool texture_transparent;
	NDIlib_video_frame_v2_t video_frame = {};

	while (true) {
		sem->wait();

		mtx_send->lock();
		bool exit_thread = mtx_exit_thread;
		bool rebuild_send = mtx_rebuild_send;
		mtx_send->unlock();

		if (exit_thread) {
			break;
		}

		if (rebuild_send) {
			if (send != nullptr) {
				ndi->send_destroy(send);
				send = nullptr;
			}

			mtx_send->lock();

			mtx_rebuild_send = false;
			if (mtx_send_desc.p_ndi_name != nullptr && (is_outputting_editor() || !Engine::get_singleton()->is_editor_hint())) {
				send = ndi->send_create(&mtx_send_desc);
			}

			mtx_send->unlock();
		}

		if (send == nullptr) {
			continue;
		}

		mtx_texture->lock();
		texture_buffer = mtx_texture_data;
		texture_format = mtx_texture_format;
		texture_transparent = mtx_texture_transparent;
		mtx_texture->unlock();

		if (texture_format.is_null() || texture_buffer.is_empty()) {
			continue;
		}

		ERR_CONTINUE_MSG(texture_format->get_format() != RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM, "Viewport texture format isn't DATA_FORMAT_R8G8B8A8_UNORM");
		ERR_CONTINUE_MSG(texture_format->get_width() * texture_format->get_height() * 4 != texture_buffer.size(), "Unexpected texture size");

		video_frame.xres = texture_format->get_width();
		video_frame.yres = texture_format->get_height();
		video_frame.frame_rate_N = (int)Engine::get_singleton()->get_frames_per_second();
		video_frame.frame_rate_D = 1;
		video_frame.FourCC = texture_transparent ? NDIlib_FourCC_type_RGBA : NDIlib_FourCC_type_RGBX;
		video_frame.p_data = (uint8_t *)texture_buffer.ptr();

		ndi->send_send_video_v2(send, &video_frame);
	}

	if (send != nullptr) {
		ndi->send_destroy(send);
		send = nullptr;
	}
}
