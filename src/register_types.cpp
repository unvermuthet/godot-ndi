#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <dlfcn.h>
#endif

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "Processing.NDI.Lib.h"

#include "ndi_find.h"
#include "ndi_source.h"
#include "register_types.h"

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	const char* ndi_runtime_folder = getenv(NDILIB_REDIST_FOLDER);

#ifdef _WIN32
	if (!ndi_runtime_folder) {
		MessageBoxA(NULL, "Please install the NewTek NDI Runtimes to use this application.", "Runtime Warning.", MB_OK);
		ShellExecuteA(NULL, "open", NDILIB_REDIST_URL, 0, 0, SW_SHOWNORMAL);
		return;
	}

	std::string ndi_runtime_path = ndi_runtime_folder;
	ndi_runtime_path += "\\" NDILIB_LIBRARY_NAME;

	HMODULE ndi_lib = LoadLibraryA(ndi_runtime_path.c_str());
	const NDIlib_v5* (*NDIlib_v5_load)(void) = NULL;

	if (ndi_lib) {
		*((FARPROC*)&NDIlib_v5_load) = GetProcAddress(ndi_lib, "NDIlib_v5_load");
	}

	if (!NDIlib_v5_load) {
		if (ndi_lib) {
			FreeLibrary(ndi_lib);
		}
		MessageBoxA(NULL, "Please re-install the NewTek NDI Runtimes to use this application.", "Runtime Warning.", MB_OK);
		ShellExecuteA(NULL, "open", NDILIB_REDIST_URL, 0, 0, SW_SHOWNORMAL);
		return;
	}
#else
	std::string ndi_runtime_path;

	if (ndi_runtime_folder) {
		ndi_runtime_path = ndi_runtime_folder;
		ndi_runtime_path += NDILIB_LIBRARY_NAME;
	} else {
		ndi_runtime_path = NDILIB_LIBRARY_NAME;
	}

	// Try to load the library
	void* ndi_lib = dlopen(ndi_runtime_path.c_str(), RTLD_LOCAL | RTLD_LAZY);

	// The main NDIFind entry point for dynamic loading if we got the library
	const NDIlib_v5* (*NDIlib_v5_load)(void) = NULL;
	if (ndi_lib) {
		*((void**)&NDIlib_v5_load) = dlsym(ndi_lib, "NDIlib_v5_load");
	}

	// If we failed to load the library then we tell people to re-install it
	if (!NDIlib_v5_load) {
		// Unload the library if we loaded it
		if (ndi_lib) {
			dlclose(ndi_lib);
		}
		printf("Please re-install the NewTek NDI Runtimes from " NDILIB_REDIST_URL " to use this application.");
		return;
	}
#endif

	ndi = NDIlib_v5_load();

	if (!ndi->initialize()) {
		printf("Cannot run NDI..");
		return;
	}

	GDREGISTER_CLASS(NDIFind);
	GDREGISTER_CLASS(NDISource);
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
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