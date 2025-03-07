/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

Loading of library referenced from NDI SDK dynamic loading example

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

#include "initialize.hpp"

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
		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			print_verbose("NDI: Godot NDI Plugin ", String(GIT_COMMIT_TAG), "(", String(GIT_COMMIT_HASH).left(9), ")");

			if (load_runtime() != OK) {
				return;
			}

			GDREGISTER_CLASS(NDIFinder);
			GDREGISTER_CLASS(NDIOutput);
			GDREGISTER_CLASS(VideoStreamNDI);
			GDREGISTER_CLASS(VideoStreamPlaybackNDI);
			GDREGISTER_CLASS(ViewportTextureRouter);

			if (!Engine::get_singleton()->has_singleton("GlobalNDIFinder")) {
				Engine::get_singleton()->register_singleton("GlobalNDIFinder", memnew(NDIFinder));
			}

			if (!Engine::get_singleton()->has_singleton("ViewportTextureRouter")) {
				Engine::get_singleton()->register_singleton("ViewportTextureRouter", memnew(ViewportTextureRouter));
			}
		} break;

		case MODULE_INITIALIZATION_LEVEL_EDITOR: {
			GDREGISTER_CLASS(NDIVersionCheck);
			GDREGISTER_CLASS(MissingNDIRuntimeDialog);
			EditorPlugins::add_by_type<NDIVersionCheck>();
			EditorPlugins::add_by_type<MissingNDIRuntimeDialog>();
		} break;
	}
}

void uninitialize(ModuleInitializationLevel p_level) {
	switch (p_level) {
		case MODULE_INITIALIZATION_LEVEL_EDITOR: {
			EditorPlugins::remove_by_type<NDIVersionCheck>();
			EditorPlugins::remove_by_type<MissingNDIRuntimeDialog>();
		} break;

		case MODULE_INITIALIZATION_LEVEL_SCENE: {
			if (Engine::get_singleton()->has_singleton("GlobalNDIFinder")) {
				memdelete(Engine::get_singleton()->get_singleton("GlobalNDIFinder"));
			}

			if (ndi != nullptr) {
				ndi->destroy();
				ndi = nullptr;
			}
		} break;

		case MODULE_INITIALIZATION_LEVEL_CORE: {
			if (Engine::get_singleton()->has_singleton("ViewportTextureRouter")) {
				memdelete(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
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
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
	return init_obj.init();
}
}
