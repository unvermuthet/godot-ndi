#pragma once

#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <godot_cpp/classes/ref_counted.hpp>
#include "Processing.NDI.Lib.h"

using namespace godot;

class NDI : public RefCounted {
    GDCLASS(NDI, RefCounted)

    private:

    protected:
        static void _bind_methods();

    public:
        NDI();
        ~NDI();
        const NDIlib_v5* lib;
};