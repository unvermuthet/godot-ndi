/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

Loading of library referenced from NDI SDK dynamic loading example

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH ""
#endif

#ifndef GIT_COMMIT_TAG
#define GIT_COMMIT_TAG ""
#endif

const NDIlib_v5 *ndi = nullptr;

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	print_verbose("NDI: Godot NDI Plugin ", String(GIT_COMMIT_TAG), "(", String(GIT_COMMIT_HASH).left(9), ")");

	String runtime_folder = getenv(NDILIB_REDIST_FOLDER);

#ifdef _WIN32

	String runtime_path = runtime_folder;
	if (runtime_folder.is_empty()) {
		runtime_path = NDILIB_LIBRARY_NAME;
	} else {
		runtime_path = runtime_folder + "\\" NDILIB_LIBRARY_NAME;
	}

	print_verbose("NDI: Trying to load ", runtime_path);
	HMODULE ndi_lib = LoadLibraryA(runtime_path.utf8());
	ERR_FAIL_NULL_EDMSG(ndi_lib, "NDI: Failed to load NDI Runtime. Make sure its installed on your system. Path tried: " + runtime_path);

	const NDIlib_v5 *(*NDIlib_v5_load)(void) = NULL;
	*((FARPROC *)&NDIlib_v5_load) = GetProcAddress(ndi_lib, "NDIlib_v5_load");

	if (!NDIlib_v5_load) {
		if (ndi_lib) {
			FreeLibrary(ndi_lib);
		}
		ERR_FAIL_EDMSG("NDI: Couldn't obtain entry symbol for " + runtime_path);
	}

	ndi = NDIlib_v5_load();

#else

	PackedStringArray runtime_paths;
	runtime_paths.append(runtime_folder.path_join(NDILIB_LIBRARY_NAME));
	runtime_paths.append(NDILIB_LIBRARY_NAME);
	runtime_paths.append("/usr/local/lib/" NDILIB_LIBRARY_NAME);

	for (int64_t i = 0; i < runtime_paths.size(); i++) {
		print_verbose("NDI: Trying to load ", runtime_paths[i]);

		void *ndi_lib = dlopen(runtime_paths[i].utf8(), RTLD_LAZY | RTLD_LOCAL);
		if (!ndi_lib) {
			print_verbose("NDI: Failed to open ", runtime_paths[i]);
			continue;
		}

		const NDIlib_v5 *(*NDIlib_v5_load)(void) = nullptr;
		*((void **)&NDIlib_v5_load) = dlsym(ndi_lib, "NDIlib_v5_load");

		if (!NDIlib_v5_load) {
			if (ndi_lib) {
				dlclose(ndi_lib);
			}
			print_verbose("NDI: Couldn't obtain entry symbol for ", runtime_paths[i]);
			continue;
		}

		ndi = NDIlib_v5_load();
		break;
	}

	ERR_FAIL_NULL_EDMSG(ndi, "NDI: Failed to load NDI Runtime. Make sure its installed on your system. Paths tried: \n" + String("\n").join(runtime_paths));

#endif

	ERR_FAIL_COND_EDMSG(!ndi->initialize(), "NDI: NDI isn't supported on your device");

	GDREGISTER_CLASS(NDIFinder);
	GDREGISTER_CLASS(VideoStreamNDI);
	GDREGISTER_CLASS(VideoStreamPlaybackNDI);
	GDREGISTER_CLASS(NDIOutput);
	GDREGISTER_CLASS(ViewportTextureRouter);

	if (!Engine::get_singleton()->has_singleton("GlobalNDIFinder")) {
		Engine::get_singleton()->register_singleton("GlobalNDIFinder", memnew(NDIFinder));
	}

	if (!Engine::get_singleton()->has_singleton("ViewportTextureRouter")) {
		Engine::get_singleton()->register_singleton("ViewportTextureRouter", memnew(ViewportTextureRouter));
	}
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_CORE) {
		return;
	}

	if (Engine::get_singleton()->has_singleton("GlobalNDIFinder")) {
		memdelete(Engine::get_singleton()->get_singleton("GlobalNDIFinder"));
	}

	if (Engine::get_singleton()->has_singleton("ViewportTextureRouter")) {
		memdelete(Engine::get_singleton()->get_singleton("ViewportTextureRouter"));
	}

	if (ndi != nullptr) {
		ndi->destroy();
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT godot_ndi_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_gdextension_types);
	init_obj.register_terminator(uninitialize_gdextension_types);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
	return init_obj.init();
}
}
