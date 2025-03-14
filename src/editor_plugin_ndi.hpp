/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "ndi_runtime_dialog.hpp"
#include "ndi_version_check.hpp"

#include <godot_cpp/classes/editor_plugin.hpp>

using namespace godot;

class NDIRuntimeDialog;
class NDIVersionCheck;

class EditorPluginNDI : public EditorPlugin {
	GDCLASS(EditorPluginNDI, EditorPlugin)

public:
	EditorPluginNDI();
	~EditorPluginNDI();

protected:
	static void _bind_methods();
	void _notification(int p_what);
};
