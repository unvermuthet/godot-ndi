/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

ViewportTextureRouter::ViewportTextureRouter() {
}

ViewportTextureRouter::~ViewportTextureRouter() {
}

void ViewportTextureRouter::request_texture(Viewport *p_viewport) {
	ERR_FAIL_NULL_MSG(p_viewport, "Viewport is null");

	RenderingServer *rs = RenderingServer::get_singleton();
	RenderingDevice *rd = rs->get_rendering_device();

	RID rd_texture_rid = rs->texture_get_rd_texture(p_viewport->get_texture()->get_rid());
	ERR_FAIL_COND_MSG(!rd_texture_rid.is_valid(), "Couldn't get viewport texture's RID on the RenderingDevice");

	Ref<RDTextureFormat> texture_format = rd->texture_get_format(rd_texture_rid);
	ERR_FAIL_COND_MSG(texture_format.is_null(), "Couldn't get viewport texture format");

	rd->texture_get_data_async(rd_texture_rid, 0, callable_mp(this, &ViewportTextureRouter::forward_texture).bind(texture_format, p_viewport));
}

void ViewportTextureRouter::forward_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, Viewport *p_viewport) {
	emit_signal("texture_arrived", p_data, p_format, p_viewport);
}

void ViewportTextureRouter::_bind_methods() {
	ADD_SIGNAL(MethodInfo("texture_arrived", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "p_data"), PropertyInfo(Variant::OBJECT, "p_format"), PropertyInfo(Variant::OBJECT, "p_viewport")));
}
