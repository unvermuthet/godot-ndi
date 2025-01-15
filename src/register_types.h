#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Processing.NDI.Lib.h"
#include "Processing.NDI.Lib.cplusplus.h"

#ifndef EXAMPLE_REGISTER_TYPES_H
#define EXAMPLE_REGISTER_TYPES_H

void initialize_gdextension_types();
void uninitialize_gdextension_types();

#endif // EXAMPLE_REGISTER_TYPES_H

extern const NDIlib_v5 *ndi = NULL;