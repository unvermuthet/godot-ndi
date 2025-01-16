#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include "Processing.NDI.Lib.h"

using namespace godot;

class NDISource : public RefCounted {
    GDCLASS(NDISource, RefCounted)

    private:
        NDIlib_source_t source;

    protected:
        static void _bind_methods();

    public:
        NDISource();
        void set_name(const String name);
        String get_name() const;
        void set_url(const String url);
        String get_url() const;
};