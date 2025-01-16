#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include "Processing.NDI.Lib.h"
#include "ndi_connection.h"
#include "ndi_source.h"

using namespace godot;

class NDI : public RefCounted {
    GDCLASS(NDI, RefCounted)

    private:
        NDIlib_find_instance_t find;

    protected:
        static void _bind_methods();

    public:
        NDI();
        ~NDI();
        void set_sources_settings(const bool show_local_sources, const PackedStringArray groups, const PackedStringArray extra_ips);
        TypedArray<NDISource> get_sources() const;
        Ref<NDIConnection> connect(const String source_name, const bool low_bandwidth) const;
};