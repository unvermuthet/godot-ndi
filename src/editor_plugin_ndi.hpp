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

	static void _set_ndi_button_callable(const Callable p_callable);
	static Callable _get_ndi_button_callable();
	static void _open_ndi_link();

protected:
	static void _bind_methods();
	void _notification(int p_what);
};

#define ADD_NDI_BUTTON                                                                                                                 \
	ClassDB::bind_static_method(get_class_static(), D_METHOD("_set_ndi_button_callable"), &EditorPluginNDI::_set_ndi_button_callable); \
	ClassDB::bind_static_method(get_class_static(), D_METHOD("_get_ndi_button_callable"), &EditorPluginNDI::_get_ndi_button_callable); \
	ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "_ndi_button_callable", PROPERTY_HINT_TOOL_BUTTON, "https://ndi.video/,ExternalLink", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_INTERNAL), "_set_ndi_button_callable", "_get_ndi_button_callable");
