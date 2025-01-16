#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/image.hpp>
#include "Processing.NDI.Lib.h"

using namespace godot;

class NDIConnection : public RefCounted {
    GDCLASS(NDIConnection, RefCounted)

    private:
        NDIlib_recv_instance_t *recv;
        NDIlib_framesync_instance_t *framesync;

    protected:
        static void _bind_methods();

    public:
        NDIConnection();
        ~NDIConnection();
        Ref<Image> get_frame() const;
};