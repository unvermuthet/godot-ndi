#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "Processing.NDI.Lib.h"
#include "ndi.h"
#include "ndi_source.h"

using namespace godot;

class NDIFind : public NDI {
    GDCLASS(NDIFind, NDI)

    private:
        NDIlib_find_instance_t find;
        NDIlib_find_create_t find_desc;

    protected:
        static void _bind_methods();

    public:
        NDIFind();
        ~NDIFind();

        void set_show_local_sources(const bool state);
        bool get_show_local_sources() const;

        void set_groups(const PackedStringArray groups);
        PackedStringArray get_groups() const;

        void set_extra_ips(const PackedStringArray extra_ips);
        PackedStringArray get_extra_ips() const;

        TypedArray<NDISource> get_sources() const;
};

// Call me Mao how she Leap Forward on Zedong and calls it Great