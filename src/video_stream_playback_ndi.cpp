/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

VideoStreamPlaybackNDI::VideoStreamPlaybackNDI() {
	texture.instantiate();
}

VideoStreamPlaybackNDI::~VideoStreamPlaybackNDI() {
	texture.unref();
}

void VideoStreamPlaybackNDI::_play() {
	if (!start_receiving())
		return;
	if (!start_syncing())
		return;
	playing = true;
}

void VideoStreamPlaybackNDI::_stop() {
	playing = false;
	stop_syncing();
	stop_receiving();
}

bool VideoStreamPlaybackNDI::_is_playing() const {
	return playing;
}

void VideoStreamPlaybackNDI::_set_paused(bool p_paused) {
	paused = p_paused;
}

bool VideoStreamPlaybackNDI::_is_paused() const {
	return paused;
}

double VideoStreamPlaybackNDI::_get_length() const {
	return 0.0;
}

double VideoStreamPlaybackNDI::_get_playback_position() const {
	return 0.0;
}

void VideoStreamPlaybackNDI::_seek(double p_time) {}

void VideoStreamPlaybackNDI::_set_audio_track(int32_t p_idx) {}

int32_t VideoStreamPlaybackNDI::_get_channels() const {
	return 2;
	// As far as i know this is only querried once by the video player.
	// Might get updated afer stop/play
	// Hardcoded for now :/
}

int32_t VideoStreamPlaybackNDI::_get_mix_rate() const {
	return ProjectSettings::get_singleton()->get_setting("audio/driver/mix_rate", 44100);
}

Ref<Texture2D> VideoStreamPlaybackNDI::_get_texture() const {
	return texture;
}

void VideoStreamPlaybackNDI::_update(double p_delta) {
	ERR_FAIL_COND_MSG(receiving == false || syncing == false, "VideoStreamPlaybackNDI wasn't setup properly. Call setup() before passing to VideoStreamPlayer!");

	if (p_delta == 0) { // See https://github.com/godotengine/godot/blob/b15b24b087e792335d919fd83055f50f276fbe22/scene/gui/video_stream_player.cpp#L314
		render_first_frame();
	} else {
		render_audio(p_delta);
		render_video();
	}
}

void VideoStreamPlaybackNDI::_bind_methods() {}

bool VideoStreamPlaybackNDI::start_receiving(bool force) {
	if (force || !receiving) {
		stop_receiving();
		recv = ndi->recv_create_v3(&recv_desc);
		receiving = true;
	}

	return true;
}

void VideoStreamPlaybackNDI::stop_receiving() {
	if (receiving) {
		stop_syncing();
		ndi->recv_destroy(recv);
		receiving = false;
	}
}

bool VideoStreamPlaybackNDI::start_syncing(bool force) {
	if (force || !syncing) {
		if (!receiving) {
			ERR_FAIL_COND_V_MSG(!start_receiving(), false, "Can't create a NDI Framesync without a NDI Receiver");
		}

		stop_syncing();
		sync = ndi->framesync_create(recv);
		syncing = true;
	}

	return true;
}

void VideoStreamPlaybackNDI::stop_syncing() {
	if (syncing) {
		ndi->framesync_destroy(sync);
		syncing = false;
	}
}

void VideoStreamPlaybackNDI::render_first_frame() {
	for (size_t i = 0; i < 500; i++) {
		ndi->framesync_capture_video(sync, &video_frame, NDIlib_frame_format_type_progressive);
		if (video_frame.xres != 0 && video_frame.yres != 0) {
			texture->set_image(Image::create_empty(video_frame.xres, video_frame.yres, false, Image::FORMAT_RGBA8));
			ndi->framesync_free_video(sync, &video_frame);
			return;
		}
		ndi->framesync_free_video(sync, &video_frame);
		OS::get_singleton()->delay_msec(10);
	}

	// Fallback resolution
	texture->set_image(Image::create_empty(1920, 1080, false, Image::FORMAT_RGBA8));
	ERR_FAIL_MSG("NDI Source not found at playback start. Will play once found.");
}

void VideoStreamPlaybackNDI::render_video() {
	if (img.is_valid()) {
		if (texture->get_width() == img->get_width() && texture->get_height() == img->get_height()) {
			texture->update(img);
		} else {
			texture->set_image(img);
		}
	}

	ndi->framesync_capture_video(sync, &video_frame, NDIlib_frame_format_type_progressive);

	if (video_frame.p_data != NULL && (video_frame.FourCC == NDIlib_FourCC_type_RGBA || video_frame.FourCC == NDIlib_FourCC_type_RGBX)) {
		video_buffer.resize(video_frame.line_stride_in_bytes * video_frame.yres);
		memcpy(video_buffer.ptrw(), video_frame.p_data, video_buffer.size());
		img = Image::create_from_data(video_frame.xres, video_frame.yres, false, Image::Format::FORMAT_RGBA8, video_buffer);
	}

	ndi->framesync_free_video(sync, &video_frame);
}

void VideoStreamPlaybackNDI::render_audio(double p_delta) {
	int requested_samples = Math::min((double)_get_mix_rate() * p_delta, (double)ndi->framesync_audio_queue_depth(sync));
	ndi->framesync_capture_audio_v2(sync, &audio_frame, _get_mix_rate(), _get_channels(), requested_samples);

	if (audio_frame.p_data != NULL) {
		audio_buffer_planar.resize(audio_frame.no_channels * audio_frame.no_samples);
		audio_buffer_interleaved.resize(audio_buffer_planar.size());

		memcpy((uint8_t *)audio_buffer_planar.ptrw(), audio_frame.p_data, audio_buffer_planar.size() * 4);

		for (int64_t i = 0; i < audio_buffer_interleaved.size(); i++) {
			int channel = i % audio_frame.no_channels;
			int stride_index = channel * audio_frame.no_samples;
			int stride_offset = i / audio_frame.no_channels;

			audio_buffer_interleaved.set(i, audio_buffer_planar[stride_index + stride_offset]);
		}

		int processed_samples = Math::min(audio_frame.no_samples, 8192); // FIXME: dont hardcode this
		int skipped_samples = audio_frame.no_samples - processed_samples;

		// Skip the older samples by playing the last ones in the array
		mix_audio(processed_samples, audio_buffer_interleaved, skipped_samples * _get_channels());
	}

	ndi->framesync_free_audio_v2(sync, &audio_frame);
}