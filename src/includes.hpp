#pragma once

#include <gdextension_interface.h>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/thread.hpp>
#include <godot_cpp/classes/mutex.hpp>
#include <godot_cpp/classes/semaphore.hpp>
#include <godot_cpp/classes/hashing_context.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#endif

#include "Processing.NDI.Lib.h"
extern const NDIlib_v5* ndi;

using namespace godot;

class NDIFinder;
class VideoStreamNDI;
class VideoStreamPlaybackNDI;

#include "ndi_finder.hpp"
#include "video_stream_ndi.hpp"
#include "video_stream_playback_ndi.hpp"
#include "ndi_output.hpp"
