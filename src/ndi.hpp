/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <cstddef>

#include "Processing.NDI.Lib.h"

extern const NDIlib_v5 *ndi;

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH ""
#endif

#ifndef GIT_COMMIT_TAG
#define GIT_COMMIT_TAG ""
#endif

#define SIGNAL_DISCONNECT(obj, signal, callable) \
	if (obj->is_connected(signal, callable)) {   \
		obj->disconnect(signal, callable);       \
	}
