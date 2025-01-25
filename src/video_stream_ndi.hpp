/*
godot-ndi
	Copyright 2025 Henry Muth - http://github.com/unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "ndi.hpp"
#include "video_stream_playback_ndi.hpp"

using namespace godot;

class VideoStreamNDI : public VideoStream {
    GDCLASS(VideoStreamNDI, VideoStream)

    NDIlib_recv_create_v3_t recv_desc;
    
    CharString name;
    CharString url;

    protected:
        static void _bind_methods();

    public:
        VideoStreamNDI();
        VideoStreamNDI(NDIlib_source_t source);
        ~VideoStreamNDI();
        void set_name(const String _name);
        String get_name() const;
        void set_url(const String _url);
        String get_url() const;
        void set_bandwidth(const NDIlib_recv_bandwidth_e bandwidth);
        NDIlib_recv_bandwidth_e get_bandwidth() const;
        Ref<VideoStreamPlayback> _instantiate_playback() override;
};

VARIANT_ENUM_CAST(NDIlib_recv_bandwidth_e);
