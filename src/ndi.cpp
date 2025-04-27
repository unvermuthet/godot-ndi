/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

Loading of library referenced from NDI SDK dynamic loading example

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ndi.hpp"

#include "editor_plugin_ndi.hpp"
#include "ndi_finder.hpp"
#include "ndi_output.hpp"
#include "ndi_runtime_dialog.hpp"
#include "ndi_version_check.hpp"
#include "video_stream_ndi.hpp"
#include "video_stream_playback_ndi.hpp"
#include "viewport_texture_router.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <stdlib.h>
#endif

#ifdef _WIN32
#define LOAD_LIBRARY(var, path) HMODULE var = LoadLibraryA(path)
#define GET_PROC_ADDRESS(var, lib, name) *((FARPROC *)&var) = GetProcAddress(lib, name);
#define FREE_LIBRARY(lib) FreeLibrary(lib)
#else
#define LOAD_LIBRARY(var, path) void *var = dlopen(path, RTLD_LAZY | RTLD_LOCAL)
#define GET_PROC_ADDRESS(var, lib, name) *((void **)&var) = dlsym(lib, name)
#define FREE_LIBRARY(lib) dlclose(lib)
#endif

using namespace godot;

class EditorPluginNDI;
class NDIFinder;
class NDIOutput;
class NDIRuntimeDialog;
class NDIVersionCheck;
class VideoStreamNDI;
class VideoStreamPlaybackNDI;
class ViewportTextureRouter;

const NDIlib_v5 *ndi = nullptr;

Error load_runtime() {
	PackedStringArray runtime_paths;
	String runtime_folder = getenv(NDILIB_REDIST_FOLDER);

#ifdef _WIN32
	runtime_paths.append(runtime_folder.path_join(NDILIB_LIBRARY_NAME));
	runtime_paths.append(NDILIB_LIBRARY_NAME);
#else
	runtime_paths.append(runtime_folder.path_join(NDILIB_LIBRARY_NAME));
	runtime_paths.append(NDILIB_LIBRARY_NAME);
	runtime_paths.append("/usr/local/lib/" NDILIB_LIBRARY_NAME);
#endif

	for (int64_t i = 0; i < runtime_paths.size(); i++) {
		print_verbose("NDI: Trying to load ", runtime_paths[i]);

		LOAD_LIBRARY(ndi_lib, runtime_paths[i].utf8());

		if (ndi_lib == nullptr) {
			print_verbose("NDI: Failed to open ", runtime_paths[i]);
			continue;
		}

		const NDIlib_v5 *(*NDIlib_v5_load)(void) = nullptr;
		GET_PROC_ADDRESS(NDIlib_v5_load, ndi_lib, "NDIlib_v5_load");

		if (NDIlib_v5_load == nullptr) {
			if (ndi_lib != nullptr) {
				FREE_LIBRARY(ndi_lib);
			}
			print_verbose("NDI: Couldn't obtain entry symbol for ", runtime_paths[i]);
			continue;
		}

		ndi = NDIlib_v5_load();

		if (ndi == nullptr) {
			if (ndi_lib != nullptr) {
				FREE_LIBRARY(ndi_lib);
			}
			print_verbose("NDI: Failed to load runtime from opened file ", runtime_paths[i]);
			continue;
		}

		print_verbose("NDI: Runtime loaded from ", runtime_paths[i]);
		break;
	}

	ERR_FAIL_NULL_V_MSG(ndi, ERR_FILE_CANT_OPEN, "NDI: Failed to load NDI Runtime. Make sure its installed on your system. Paths tried: \n" + String("\n").join(runtime_paths));
	ERR_FAIL_COND_V_MSG(!ndi->initialize(), ERR_UNAVAILABLE, "NDI: NDI isn't supported on your device");

	print_verbose("NDI: Runtime sucessfully initialized");
	return OK;
}

void initialize(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_CORE: {
			GDREGISTER_INTERNAL_CLASS(ViewportTextureRouter);

			if (!Engine::get_singleton()->has_singleton("ViewportTextureRouter")) {
				Engine::get_singleton()->register_singleton("ViewportTextureRouter", memnew(ViewportTextureRouter));
			}
		} break;

		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			print_verbose("NDI: Godot NDI Plugin ", String(GIT_COMMIT_TAG), "(", String(GIT_COMMIT_HASH).left(9), ")");

			// Not dependant on NDI SDK, needed in EditorPlugin
			GDREGISTER_CLASS(NDIRuntimeDialog);
			GDREGISTER_CLASS(NDIVersionCheck);

			if (load_runtime() != OK) {
				return;
			}

			auto property_info = Dictionary();
			property_info.set("name", "godot_ndi/enable_fps_warning");
			property_info.set("type", Variant::BOOL);
			ProjectSettings::get_singleton()->set_setting("godot_ndi/enable_fps_warning", true);
			ProjectSettings::get_singleton()->set_initial_value("godot_ndi/enable_fps_warning", true);
			ProjectSettings::get_singleton()->add_property_info(property_info);

			GDREGISTER_CLASS(NDIFinder);
			GDREGISTER_CLASS(NDIOutput);
			GDREGISTER_CLASS(VideoStreamNDI);
			GDREGISTER_CLASS(VideoStreamPlaybackNDI);

			if (!Engine::get_singleton()->has_singleton("GlobalNDIFinder")) {
				Engine::get_singleton()->register_singleton("GlobalNDIFinder", memnew(NDIFinder));
			}

		} break;

		case MODULE_INITIALIZATION_LEVEL_EDITOR: {
			GDREGISTER_CLASS(EditorPluginNDI);
			EditorPlugins::add_by_type<EditorPluginNDI>();
		} break;
	}
}

void uninitialize(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_EDITOR: {
			EditorPlugins::remove_by_type<EditorPluginNDI>();
		} break;

		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			if (Engine::get_singleton()->has_singleton("GlobalNDIFinder")) {
				NDIFinder *finder = Object::cast_to<NDIFinder>(Engine::get_singleton()->get_singleton("GlobalNDIFinder"));
				Engine::get_singleton()->unregister_singleton("GlobalNDIFinder");
				memdelete(finder);
			}

			if (ndi != nullptr) {
				ndi->destroy();
				ndi = nullptr;
			}
		} break;

		case MODULE_INITIALIZATION_LEVEL_CORE: {
			if (Engine::get_singleton()->has_singleton("ViewportTextureRouter")) {
				ViewportTextureRouter *vptr = Object::cast_to<ViewportTextureRouter>(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
				Engine::get_singleton()->unregister_singleton("ViewportTextureRouter");
				memdelete(vptr);
			}
		} break;
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT godot_ndi_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize);
	init_obj.register_terminator(uninitialize);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_CORE);
	return init_obj.init();
}
}
