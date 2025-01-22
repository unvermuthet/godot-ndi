#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>
#endif

#include "Processing.NDI.Lib.h"

extern const NDIlib_v5* ndi;