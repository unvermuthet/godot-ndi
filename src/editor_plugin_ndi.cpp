/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "editor_plugin_ndi.hpp"

#include <godot_cpp/classes/os.hpp>

using namespace godot;

EditorPluginNDI::EditorPluginNDI() {
}

EditorPluginNDI::~EditorPluginNDI() {
}

void EditorPluginNDI::_set_ndi_button_callable(const Callable p_callable) {}

Callable EditorPluginNDI::_get_ndi_button_callable() {
	return callable_mp_static(&EditorPluginNDI::_open_ndi_link);
}

void EditorPluginNDI::_open_ndi_link() {
	OS::get_singleton()->shell_open("https://ndi.video/");
}

void EditorPluginNDI::_bind_methods() {}

void EditorPluginNDI::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			add_child(memnew(NDIVersionCheck));
			add_child(memnew(NDIRuntimeDialog));
		} break;
	}
}
