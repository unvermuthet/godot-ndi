#pragma once

#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "Processing.NDI.Lib.h"
#include "video_stream_playback_ndi.h"

using namespace godot;

class VideoStreamNDI : public VideoStream {
    GDCLASS(VideoStreamNDI, VideoStream)

    NDIlib_recv_create_v3_t recv_desc;
    
    CharString name;
    CharString url;

    protected:
        static void _bind_methods() {
            ClassDB::bind_method(D_METHOD("set_name", "name"), &VideoStreamNDI::set_name);
            ClassDB::bind_method(D_METHOD("get_name"), &VideoStreamNDI::get_name);

            ClassDB::bind_method(D_METHOD("set_url", "url"), &VideoStreamNDI::set_url);
            ClassDB::bind_method(D_METHOD("get_url"), &VideoStreamNDI::get_url);

            ClassDB::bind_method(D_METHOD("set_bandwidth", "bandwidth"), &VideoStreamNDI::set_bandwidth);
            ClassDB::bind_method(D_METHOD("get_bandwidth"), &VideoStreamNDI::get_bandwidth);

            ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
            ADD_PROPERTY(PropertyInfo(Variant::STRING, "url"), "set_url", "get_url");
            ADD_PROPERTY(PropertyInfo(Variant::INT, "bandwidth", PROPERTY_HINT_ENUM, "Metadata only:-10,Audio only:10,Lowest:0,Highest:100"), "set_bandwidth", "get_bandwidth");

            BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_metadata_only);
            BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_audio_only);
            BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_lowest);
            BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_highest);
        }

    public:
        VideoStreamNDI() {
            recv_desc.allow_video_fields = false;
            recv_desc.color_format = NDIlib_recv_color_format_RGBX_RGBA;
            recv_desc.bandwidth = NDIlib_recv_bandwidth_highest;
        }

        VideoStreamNDI(NDIlib_source_t source) : VideoStreamNDI() {
            recv_desc.source_to_connect_to = source;
        }

        ~VideoStreamNDI() { }

        void set_name(const String _name) {
            name = _name.utf8();
            recv_desc.source_to_connect_to.p_ndi_name = name;
        }

        String get_name() const {
            if (recv_desc.source_to_connect_to.p_ndi_name == NULL) {
                return String();
            }

            return String::utf8(recv_desc.source_to_connect_to.p_ndi_name);
        }

        void set_url(const String _url) {
            if (_url.is_empty()) {
                recv_desc.source_to_connect_to.p_url_address = NULL;
                return;
            }
            url = _url.utf8();
            recv_desc.source_to_connect_to.p_url_address = url;
        }

        String get_url() const {
            if (recv_desc.source_to_connect_to.p_url_address == NULL) {
                return String();
            }

            return String::utf8(recv_desc.source_to_connect_to.p_url_address);
        }

        void set_bandwidth(const NDIlib_recv_bandwidth_e bandwidth) {
            recv_desc.bandwidth = bandwidth;
        }

        NDIlib_recv_bandwidth_e get_bandwidth() const {
	        return recv_desc.bandwidth;
        }

        Ref<VideoStreamPlayback> _instantiate_playback() override {
            Ref<VideoStreamPlaybackNDI> pb = memnew(VideoStreamPlaybackNDI(recv_desc));
            return pb;
        }
};

VARIANT_ENUM_CAST(NDIlib_recv_bandwidth_e);
