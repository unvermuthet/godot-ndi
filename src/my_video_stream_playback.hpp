#pragma once

#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>

using namespace godot;

class MyVideoStreamPlayback : public VideoStreamPlayback {
    GDCLASS(MyVideoStreamPlayback, VideoStreamPlayback)

    public:
        MyVideoStreamPlayback();
        ~MyVideoStreamPlayback();
        void _play() override;
        void _stop() override;
        bool _is_playing() const override;
        void _set_paused(bool p_paused) override;
        bool _is_paused() const override;
        Ref<Texture2D> _get_texture() const override;
        void _update(double p_delta) override;

    protected:
        static void _bind_methods();

    private:
        bool playing = false;
        bool paused = false;

        Ref<ImageTexture> texture;
};

