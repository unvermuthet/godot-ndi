#include "video_stream_playback_ndi.h"

void VideoStreamPlaybackNDI::_bind_methods() { }

VideoStreamPlaybackNDI::VideoStreamPlaybackNDI() {
    playing = false;
    paused = false;
}

VideoStreamPlaybackNDI::VideoStreamPlaybackNDI(NDIlib_recv_create_v3_t _recv_desc) : VideoStreamPlaybackNDI() {
    recv_desc = _recv_desc;
}

VideoStreamPlaybackNDI::~VideoStreamPlaybackNDI() { }

void VideoStreamPlaybackNDI::_stop() {
    if (playing) {
        playing = false;
        texture.unref();
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

void VideoStreamPlaybackNDI::_seek(double p_time) {
}

void VideoStreamPlaybackNDI::_set_audio_track(int32_t p_idx) {
}

Ref<Texture2D> VideoStreamPlaybackNDI::_get_texture() const {
	return texture;
}

int32_t VideoStreamPlaybackNDI::_get_channels() const {
    return 2;
}

int32_t VideoStreamPlaybackNDI::_get_mix_rate() const {
    return ProjectSettings::get_singleton()->get_setting("audio/driver/mix_rate", 48000);
}

void VideoStreamPlaybackNDI::_update(double p_delta) {
    // render_video();
    render_audio(p_delta);
}

void VideoStreamPlaybackNDI::render_video() {
    NDIlib_video_frame_v2_t video_frame;
    ndi->framesync_capture_video(sync, &video_frame, NDIlib_frame_format_type_progressive);

    if (video_frame.p_data == NULL) {
        ndi->framesync_free_video(sync, &video_frame);
        return;
    }

    Image::Format format;
    switch (video_frame.FourCC)
    {
        case NDIlib_FourCC_type_RGBA:
            format = Image::Format::FORMAT_RGBA8;
            break;

        case NDIlib_FourCC_type_RGBX:
            format = Image::Format::FORMAT_RGBA8;
            break;
        
        default:
            ndi->framesync_free_video(sync, &video_frame);
            return;
    }

    PackedByteArray data;
    data.resize(video_frame.line_stride_in_bytes * video_frame.yres);

    memcpy(data.ptrw(), video_frame.p_data, data.size());
    ndi->framesync_free_video(sync, &video_frame);

    Ref<Image> img = Image::create_from_data(video_frame.xres, video_frame.yres, false, format, data);
    texture->set_image(img);

    data.resize(0);
}

void VideoStreamPlaybackNDI::render_audio(double p_delta) {
    int no_samples = (double)_get_mix_rate() * p_delta;

    NDIlib_audio_frame_v3_t audio_frame;
    ndi->framesync_capture_audio_v2(sync, &audio_frame, _get_mix_rate(), _get_channels(), no_samples);

    if (audio_frame.p_data != NULL)
    {
        audio_p.resize(audio_frame.no_channels * audio_frame.no_samples);
        audio_i.resize(audio_p.size());

        memcpy(audio_p.ptrw(), audio_frame.p_data, audio_p.size()*4);

        for (int64_t i = 0; i < audio_i.size(); i++)
        {
            int channel = i % audio_frame.no_channels;
            int stride_index = channel * audio_frame.no_samples;
            int stride_offset = i / audio_frame.no_channels;
            
            audio_i.set(i, audio_p[stride_index + stride_offset]);
        }

        mix_audio(audio_frame.no_samples, audio_i, 0);
    }

    ndi->framesync_free_audio_v2(sync, &audio_frame);
}