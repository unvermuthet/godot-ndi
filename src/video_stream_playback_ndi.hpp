/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ndi.hpp"

#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/texture2drd.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>

using namespace godot;

class VideoStreamPlaybackNDI : public VideoStreamPlayback {
	GDCLASS(VideoStreamPlaybackNDI, VideoStreamPlayback)

public:
	VideoStreamPlaybackNDI();
	VideoStreamPlaybackNDI(NDIlib_recv_create_v3_t p_recv_desc);
	~VideoStreamPlaybackNDI();
	void _play() override;
	void _stop() override;
	bool _is_playing() const override;
	void _set_paused(bool p_paused) override;
	bool _is_paused() const override;
	double _get_length() const override;
	double _get_playback_position() const override;
	void _seek(double p_time) override;
	void _set_audio_track(int32_t p_idx) override;
	int32_t _get_channels() const override;
	int32_t _get_mix_rate() const override;
	Ref<Texture2D> _get_texture() const override;
	void _update(double p_delta) override;

protected:
	static void _bind_methods();

private:
	bool playing = false;
	bool paused = false;

	NDIlib_recv_create_v3_t recv_desc = {};
	NDIlib_recv_instance_t recv = nullptr;
	NDIlib_framesync_instance_t sync = nullptr;

	RenderingDevice *rd;
	Ref<Texture2DRD> texture;
	Vector2i texture_size = Vector2i(0, 0);
	void update_texture(NDIlib_video_frame_v2_t p_video_frame);

	void wait_for_non_empty_frame();
	bool render_video();

	NDIlib_audio_frame_v3_t audio_frame;
	PackedFloat32Array audio_buffer_planar;
	PackedFloat32Array audio_buffer_interleaved;
	void render_audio(double p_delta);
};
