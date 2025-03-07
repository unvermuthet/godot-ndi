/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifdef _WIN32
#define LOAD_LIBRARY(var, path) HMODULE var = LoadLibraryA(path)
#define GET_PROC_ADDRESS(var, lib, name) *((FARPROC *)&var) = GetProcAddress(lib, name);
#define FREE_LIBRARY(lib) FreeLibrary(lib)
#else
#define LOAD_LIBRARY(var, path) void *var = dlopen(path, RTLD_LAZY | RTLD_LOCAL)
#define GET_PROC_ADDRESS(var, lib, name) *((void **)&var) = dlsym(lib, name)
#define FREE_LIBRARY(lib) dlclose(lib)
#endif

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH ""
#endif

#ifndef GIT_COMMIT_TAG
#define GIT_COMMIT_TAG ""
#endif
