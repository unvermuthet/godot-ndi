#include "register_types.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include <dlfcn.h>
#endif

#include "Processing.NDI.Lib.h"
#include "Processing.NDI.Lib.cplusplus.h"

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	UtilityFunctions::print("HAAALALLOOO");


#ifdef _WIN32
	const char* p_ndi_runtime_v5 = getenv(NDILIB_REDIST_FOLDER);
	if (!p_ndi_runtime_v5) {
		MessageBoxA(NULL, "Please install the NewTek NDI Runtimes to use this application.", "Runtime Warning.", MB_OK);
		ShellExecuteA(NULL, "open", NDILIB_REDIST_URL, 0, 0, SW_SHOWNORMAL);
		return;
	}

	std::string ndi_path = p_ndi_runtime_v5;
	ndi_path += "\\" NDILIB_LIBRARY_NAME;

	HMODULE hNDILib = LoadLibraryA(ndi_path.c_str());
	const NDIlib_v5* (*NDIlib_v5_load)(void) = NULL;

	if (hNDILib) {
		*((FARPROC*)&NDIlib_v5_load) = GetProcAddress(hNDILib, "NDIlib_v5_load");
	}

	if (!NDIlib_v5_load) {
		if (hNDILib) {
			FreeLibrary(hNDILib);
		}
		MessageBoxA(NULL, "Please re-install the NewTek NDI Runtimes to use this application.", "Runtime Warning.", MB_OK);
		ShellExecuteA(NULL, "open", NDILIB_REDIST_URL, 0, 0, SW_SHOWNORMAL);
		return;
	}
#else
	std::string ndi_path;

	const char* p_NDI_runtime_folder = getenv(NDILIB_REDIST_FOLDER);
	if (p_NDI_runtime_folder) {
		ndi_path = p_NDI_runtime_folder;
		ndi_path += NDILIB_LIBRARY_NAME;
	} else {
		ndi_path = NDILIB_LIBRARY_NAME;
	}

	// Try to load the library
	void* hNDILib = dlopen(ndi_path.c_str(), RTLD_LOCAL | RTLD_LAZY);

	// The main NDI entry point for dynamic loading if we got the library
	const NDIlib_v5* (*NDIlib_v5_load)(void) = NULL;
	if (hNDILib) {
		*((void**)&NDIlib_v5_load) = dlsym(hNDILib, "NDIlib_v5_load");
	}

	// If we failed to load the library then we tell people to re-install it
	if (!NDIlib_v5_load) {
		// Unload the library if we loaded it
		if (hNDILib) {
			dlclose(hNDILib);
		}
		printf("Please re-install the NewTek NDI Runtimes from " NDILIB_REDIST_URL " to use this application.");
		return;
	}
#endif

	ndiLib = NDIlib_v5_load();

	if (!ndiLib->initialize()) {
		printf("Cannot run NDI..");
		return;
	}

	UtilityFunctions::print("HAAALALLOOO");
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