#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include "Processing.NDI.Lib.h"
#include "ndi.h"
#include "ndi_source.h"

using namespace godot;

class NDIFind : public NDI {
    GDCLASS(NDIFind, NDI)

    private:
        NDIlib_find_instance_t find;

    protected:
        static void _bind_methods();

    public:
        NDIFind();
        ~NDIFind();
        void set_sources_settings(const bool show_local_sources, const PackedStringArray groups, const PackedStringArray extra_ips);
        TypedArray<NDISource> get_sources() const;
};

// Call me Mao how she Leap Forward on Zedong and calls it Great