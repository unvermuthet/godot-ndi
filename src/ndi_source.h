#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "Processing.NDI.Lib.h"
#include "ndi.h"

using namespace godot;

class NDISource : public NDI {
    GDCLASS(NDISource, NDI)

    private:
        NDIlib_recv_create_v3_t recv_desc;
        NDIlib_recv_instance_t recv;
        NDIlib_framesync_instance_t sync;

    protected:
        static void _bind_methods();

    public:
        NDISource();
        NDISource(NDIlib_source_t source);

        void set_name(const String name);
        String get_name() const;

        void set_url(const String url);
        String get_url() const;

        void set_bandwidth(const NDIlib_recv_bandwidth_e bandwidth);
        NDIlib_recv_bandwidth_e get_bandwidth() const;

        void receive();
        void stop_receiving();
};

VARIANT_ENUM_CAST(NDIlib_recv_bandwidth_e);
