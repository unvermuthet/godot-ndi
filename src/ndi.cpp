#include "ndi.h"

void NDI::_bind_methods() {
}

NDI::NDI() {
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
	void* ndi_lib = dlopen(ndi_runtime_path.c_str(), RTLD_GLOBAL | RTLD_NOW);

	if (ndi_lib == NULL) {
		printf("ndi lib is null\n");
	}

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

	lib = NDIlib_v5_load();

	if (!lib->initialize()) {
		printf("Cannot run NDI..");
		return;
	}
}

NDI::~NDI() {
	lib->destroy();
}
