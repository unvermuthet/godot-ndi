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
	name = p_name;
	create_sender();
}

String NDIOutput::get_name() const {
	return name;
}

void NDIOutput::set_groups(const PackedStringArray p_groups) {
	groups = p_groups;
	create_sender();
}

PackedStringArray NDIOutput::get_groups() const {
	return groups;
}

void NDIOutput::set_output_editor(const bool p_state) {
	output_editor = p_state;
	create_sender();
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
		case Node::NOTIFICATION_POST_ENTER_TREE: {
			create_sender();
		} break;

		case Node::NOTIFICATION_EXIT_TREE: {
			destroy_sender();
		} break;
	}
}

void NDIOutput::create_sender() {
	destroy_sender();

	if (Engine::get_singleton()->is_editor_hint() && !is_outputting_editor()) {
		return;
	}

	if (get_name().is_empty()) {
		return;
	}

	if (!is_inside_tree()) {
		return;
	}

	NDIlib_send_create_t send_desc = {};
	send_desc.clock_video = true;
	send_desc.clock_audio = false;

	if (Engine::get_singleton()->is_editor_hint()) {
		send_desc.p_ndi_name = (get_name() + " (Editor)").utf8();
	} else {
		send_desc.p_ndi_name = get_name().utf8();
	}

	if (!get_groups().is_empty()) {
		send_desc.p_groups = String(",").join(groups).utf8();
	}

	mtx_send->lock();
	mtx_send_instance = ndi->send_create(&send_desc);
	mtx_sending = true;
	mtx_send->unlock();

	register_viewport();
}

void NDIOutput::destroy_sender() {
	if (!mtx_sending || !is_inside_tree()) {
		return;
	}

	unregister_viewport();

	mtx_send->lock();
	mtx_sending = false;
	ndi->send_destroy(mtx_send_instance);
	mtx_send->unlock();
}

void NDIOutput::register_viewport() {
	ViewportTextureRouter* vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");

	vp_texture_router->connect("texture_arrived", callable_mp(this, &NDIOutput::receive_texture));
	vp_texture_router->add_viewport(get_viewport());
}

void NDIOutput::unregister_viewport() {
	ViewportTextureRouter* vp_texture_router = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	ERR_FAIL_NULL_MSG(get_viewport(), "No viewport found");
	ERR_FAIL_NULL_MSG(vp_texture_router, "No viewport texture router found");
	
	vp_texture_router->disconnect("texture_arrived", callable_mp(this, &NDIOutput::receive_texture));
	vp_texture_router->remove_viewport(get_viewport());
}

void NDIOutput::receive_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, Viewport *viewport) {
	if (viewport != get_viewport() || !is_inside_tree()) {
		return; // Not my request
	}

	mtx_texture->lock();
	mtx_texture_data = p_data;
	mtx_texture_format = p_format;
	mtx_texture->unlock();
	sem->post();
}

void NDIOutput::send_video_thread() {
	PackedByteArray texture_buffer;
	Ref<RDTextureFormat> texture_format;
	NDIlib_video_frame_v2_t video_frame = {};

	while (true) {
		sem->wait();

		if (mtx_exit_thread) {
			break;
		}

		mtx_texture->lock();
		texture_buffer = mtx_texture_data;
		texture_format = mtx_texture_format;
		mtx_texture->unlock();

		ERR_CONTINUE_MSG(texture_format.is_null() || texture_buffer.is_empty(), "Viewport texture invalid");
		ERR_CONTINUE_MSG(texture_format->get_format() != RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM, "Viewport texture format isn't DATA_FORMAT_R8G8B8A8_UNORM");
		ERR_CONTINUE_MSG(texture_format->get_width() * texture_format->get_height() * 4 != texture_buffer.size(), "Unexpected texture size");

		video_frame.xres = texture_format->get_width();
		video_frame.yres = texture_format->get_height();
		video_frame.frame_rate_N = (int)Engine::get_singleton()->get_frames_per_second();
		video_frame.frame_rate_D = 1;
		video_frame.FourCC = NDIlib_FourCC_type_RGBA;
		video_frame.p_data = (uint8_t *)texture_buffer.ptr();

		mtx_send->lock();
		if (mtx_sending) {
			ndi->NDIlib_send_send_video_v2(mtx_send_instance, &video_frame);
		}
		mtx_send->unlock();
	}
}