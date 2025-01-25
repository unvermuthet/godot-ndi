/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "video_stream_playback_ndi.hpp"

void VideoStreamPlaybackNDI::_bind_methods() { }

VideoStreamPlaybackNDI::VideoStreamPlaybackNDI() {
    texture.instantiate();
    texture->set_image(Image::create_empty(100, 100, false, Image::FORMAT_RGBA8));
    playing = false;
    paused = false;
}

VideoStreamPlaybackNDI::VideoStreamPlaybackNDI(NDIlib_recv_create_v3_t _recv_desc) : VideoStreamPlaybackNDI() {
    recv_desc = _recv_desc;
}

VideoStreamPlaybackNDI::~VideoStreamPlaybackNDI() {
    _stop();
}

void VideoStreamPlaybackNDI::_stop() {
    if (playing) {
        playing = false;
        ndi->framesync_destroy(sync);
        ndi->recv_destroy(recv);
    }
}

void VideoStreamPlaybackNDI::_play() {
    if (!playing) {
        recv = ndi->recv_create_v3(&recv_desc);
        sync = ndi->framesync_create(recv);
        playing = true;
    }
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

void VideoStreamPlaybackNDI::_seek(double p_time) { }

void VideoStreamPlaybackNDI::_set_audio_track(int32_t p_idx) { }

Ref<Texture2D> VideoStreamPlaybackNDI::_get_texture() const {
	return texture;
}

int32_t VideoStreamPlaybackNDI::_get_channels() const {
    return 2;
}

int32_t VideoStreamPlaybackNDI::_get_mix_rate() const {
    return ProjectSettings::get_singleton()->get_setting("audio/driver/mix_rate", 41400);
}

void VideoStreamPlaybackNDI::_update(double p_delta) {
    render_video();
    render_audio(p_delta);
}

void VideoStreamPlaybackNDI::render_video() {
    ndi->framesync_capture_video(sync, &video_frame, NDIlib_frame_format_type_progressive);

    if (video_frame.p_data != NULL && (video_frame.FourCC == NDIlib_FourCC_type_RGBA || video_frame.FourCC == NDIlib_FourCC_type_RGBX))
    {
        video_buffer.resize(video_frame.line_stride_in_bytes * video_frame.yres);
        memcpy(video_buffer.ptrw(), video_frame.p_data, video_buffer.size());
        Ref<Image> img = Image::create_from_data(video_frame.xres, video_frame.yres, false, Image::Format::FORMAT_RGBA8, video_buffer);

        Vector2i new_resolution = {video_frame.xres, video_frame.yres};
        if (resolution == new_resolution) {
            texture->update(img);
        } else {
            texture->set_image(img);
            resolution = new_resolution;
        }

        img.unref();
    }

    ndi->framesync_free_video(sync, &video_frame);
}

void VideoStreamPlaybackNDI::render_audio(double p_delta) {
    int no_samples = (double)_get_mix_rate() * p_delta;

    ndi->framesync_capture_audio_v2(sync, &audio_frame, _get_mix_rate(), _get_channels(), no_samples);

    if (audio_frame.p_data != NULL)
    {
        audio_buffer_planar.resize(audio_frame.no_channels * audio_frame.no_samples);
        audio_buffer_interleaved.resize(audio_buffer_planar.size());

        memcpy(audio_buffer_planar.ptrw(), audio_frame.p_data, audio_buffer_planar.size()*4);
        
        for (int64_t i = 0; i < audio_buffer_interleaved.size(); i++)
        {
            int channel = i % audio_frame.no_channels;
            int stride_index = channel * audio_frame.no_samples;
            int stride_offset = i / audio_frame.no_channels;
            
            audio_buffer_interleaved.set(i, audio_buffer_planar[stride_index + stride_offset]);
        }

        mix_audio(audio_frame.no_samples, audio_buffer_interleaved, 0);
    }

    ndi->framesync_free_audio_v2(sync, &audio_frame);
}