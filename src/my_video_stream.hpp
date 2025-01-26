#pragma once

#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "my_video_stream_playback.hpp"

using namespace godot;

class MyVideoStream : public VideoStream {
    GDCLASS(MyVideoStream, VideoStream)

    protected:
        static void _bind_methods();

    public:
        MyVideoStream();
        ~MyVideoStream();
        Ref<VideoStreamPlayback> _instantiate_playback() override;
};