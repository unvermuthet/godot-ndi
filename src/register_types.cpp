/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

Loading of library referenced from NDI SDK dynamic loading example

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "register_types.hpp"

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	const char* ndi_runtime_folder = getenv(NDILIB_REDIST_FOLDER);

#ifdef _WIN32
	ERR_FAIL_NULL_EDMSG(ndi_runtime_folder, NDILIB_REDIST_FOLDER "doesn't exist on PATH. Make sure you have the NDI Runtime installed on your system.");

	std::string ndi_runtime_path = ndi_runtime_folder;
	ndi_runtime_path += "\\" NDILIB_LIBRARY_NAME;

	HMODULE ndi_lib = LoadLibraryA(ndi_runtime_path.c_str());
	ERR_FAIL_NULL_EDMSG(ndi_lib, "Couldn't open NDI Library " NDILIB_LIBRARY_NAME ". Make sure you have the NDI Runtime installed on your system.");

	const NDIlib_v5* (*NDIlib_v5_load)(void) = NULL;
	*((FARPROC*)&NDIlib_v5_load) = GetProcAddress(ndi_lib, "NDIlib_v5_load");

	if (!NDIlib_v5_load) {
		if (ndi_lib) {
			FreeLibrary(ndi_lib);
		}
		ERR_FAIL_EDMSG("Couldn't obtain entry symbol for NDI. Try reinstalling the NDI Runtime.");
	}
#else
	std::string ndi_runtime_path;

	if (ndi_runtime_folder) {
		ndi_runtime_path += NDILIB_LIBRARY_NAME;
	} else {
		ndi_runtime_path = NDILIB_LIBRARY_NAME;
	}

	void* ndi_lib = dlopen(ndi_runtime_path.c_str(), RTLD_LAZY | RTLD_LOCAL);
	ERR_FAIL_NULL_EDMSG(ndi_lib, "Couldn't open NDI Library " NDILIB_LIBRARY_NAME ". Make sure you have the NDI Runtime installed on your system.");

	const NDIlib_v5* (*NDIlib_v5_load)(void) = NULL;
	*((void**)&NDIlib_v5_load) = dlsym(ndi_lib, "NDIlib_v5_load");

	if (!NDIlib_v5_load) {
		if (ndi_lib) {
			dlclose(ndi_lib);
		}
		ERR_FAIL_EDMSG("Couldn't obtain entry symbol for NDI. Try reinstalling the NDI Runtime.");
	}

#endif

	ndi = NDIlib_v5_load();
	ERR_FAIL_COND_EDMSG(!ndi->initialize(), "NDI isn't supported");

	// GDREGISTER_CLASS(NDIFind);
	GDREGISTER_CLASS(NDIFinder);
	GDREGISTER_CLASS(VideoStreamNDI);
	GDREGISTER_CLASS(VideoStreamPlaybackNDI);
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	if (ndi != NULL) {
		ndi->destroy();
	}
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT godot_ndi_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
		return init_obj.init();
	}
}