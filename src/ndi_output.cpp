#include "ndi_output.hpp"

NDIOutput::NDIOutput() {
	mtx_exit_thread = false;
	mtx_rebuild_send = false;

	thr.instantiate();
	mtx.instantiate();
	sem.instantiate();
}

NDIOutput::~NDIOutput() {
	thr.unref();
	mtx.unref();
	sem.unref();
}

void NDIOutput::_enter_tree() {
	thr->start(callable_mp(this, &NDIOutput::process_thread));

	mtx->lock();
	mtx_rebuild_send = true;
	mtx->unlock();
}

void NDIOutput::_exit_tree() {
	mtx->lock();
	mtx_exit_thread = true;
	mtx->unlock();

	sem->post();

	if (thr.is_valid() && thr->is_alive()) {
		thr->wait_to_finish();
	}
}

void NDIOutput::_process(double p_delta) {
	if (Engine::get_singleton()->is_editor_hint()) {
		return;
	}

	mtx->lock();
	mtx_img = get_viewport()->get_texture()->get_image();
	mtx->unlock();
	sem->post();
}

void NDIOutput::set_name(const String p_name) {
	mtx->lock();
	mtx_name = p_name;
	mtx_rebuild_send = true;
	mtx->unlock();
}

String NDIOutput::get_name() const {
	mtx->lock();
	String name = mtx_name;
	mtx->unlock();
	return name;
}

void NDIOutput::set_groups(const PackedStringArray p_groups) {
	mtx->lock();
	mtx_groups = p_groups;
	mtx_rebuild_send = true;
	mtx->unlock();
}

PackedStringArray NDIOutput::get_groups() const {
	mtx->lock();
	PackedStringArray groups = mtx_groups;
	mtx->unlock();
	return groups;
}

void NDIOutput::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_name", "p_name"), &NDIOutput::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &NDIOutput::get_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

	ClassDB::bind_method(D_METHOD("set_groups", "p_groups"), &NDIOutput::set_groups);
	ClassDB::bind_method(D_METHOD("get_groups"), &NDIOutput::get_groups);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "groups"), "set_groups", "get_groups");
}

void NDIOutput::process_thread() {
	bool exit_thread = false;
	bool rebuild_send = false;

	while (!exit_thread) {
		NDIlib_send_instance_t send = NULL;

		NDIlib_send_create_t send_desc = {};
		send_desc.clock_video = true;
		send_desc.clock_audio = false;

		PackedStringArray groups = get_groups();
		if (!groups.is_empty()) {
			send_desc.p_groups = String(",").join(groups).utf8();
		}

		String name = get_name();
		if (!name.is_empty()) {
			send_desc.p_ndi_name = name.utf8();
			send = ndi->send_create(&send_desc);
			UtilityFunctions::print("rebuilt ", name);

		}


		mtx->lock();
		rebuild_send = false;
		mtx_rebuild_send = false;
		mtx->unlock();

		// Sending loop
		while (!exit_thread && !rebuild_send) {
			sem->wait();

			Ref<Image> img = Ref(memnew(Image));

			mtx->lock();
			exit_thread = mtx_exit_thread;
			rebuild_send = mtx_rebuild_send;
			if (mtx_img.is_valid()) {
				img->copy_from(mtx_img);
			}
			mtx->unlock();

			if (send == NULL) {
				UtilityFunctions::print("send null");
				img.unref();
				continue;
			}

			if (img.is_null()) {
				UtilityFunctions::print("img null");
				img.unref();
				continue;
			}

			if (img->is_empty()) {
				UtilityFunctions::print("img empty");
				img.unref();
				continue;
			}

			img->convert(Image::FORMAT_RGBA8);

			NDIlib_video_frame_v2_t video_frame = {};
			video_frame.xres = img->get_width();
			video_frame.yres = img->get_height();
			video_frame.FourCC = NDIlib_FourCC_type_RGBA;
			video_frame.frame_rate_N = (int)Engine::get_singleton()->get_frames_per_second();
			video_frame.frame_rate_D = 1;
			video_frame.p_data = (uint8_t *)img->get_data().ptr();
			video_frame.line_stride_in_bytes = img->get_width() * 4;

			ndi->send_send_video_v2(send, &video_frame);
			img.unref();
		}

		if (send != NULL) {
			ndi->send_destroy(send);
		}
	}
}
