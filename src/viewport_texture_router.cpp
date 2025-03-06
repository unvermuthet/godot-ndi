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

void ViewportTextureRouter::add_viewport(Viewport *viewport) {
	ERR_FAIL_NULL_MSG(viewport, "Viewport is null");

	if (!vps.has(viewport)) {
		vps.append(viewport);

		// If this is the first viewport, connect to the frame_post_draw signal
		if (vps.size() == 1) {
			RenderingServer::get_singleton()->connect("frame_post_draw", callable_mp(this, &ViewportTextureRouter::request_textures));
		}
	}
}

void ViewportTextureRouter::remove_viewport(Viewport *viewport) {
	ERR_FAIL_NULL_MSG(viewport, "Viewport is null");

	vps.erase(viewport);

	// Last viewport removed, disconnect from the frame_post_draw signal
	if (vps.size() == 0) {
		RenderingServer::get_singleton()->disconnect("frame_post_draw", callable_mp(this, &ViewportTextureRouter::request_textures));
	}
}

void ViewportTextureRouter::_bind_methods() {
	ADD_SIGNAL(MethodInfo("texture_arrived", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "p_data"), PropertyInfo(Variant::INT, "p_viewport_rid"), PropertyInfo(Variant::OBJECT, "p_format")));
}

void ViewportTextureRouter::request_textures() {
	for (int i = 0; i < vps.size(); i++) {
		Viewport *vp = Object::cast_to<Viewport>(vps[i]);
		RenderingServer *rs = RenderingServer::get_singleton();
		RenderingDevice *rd = rs->get_rendering_device();

		RID rd_texture_rid = rs->texture_get_rd_texture(vp->get_texture()->get_rid());
		ERR_FAIL_COND_MSG(!rd_texture_rid.is_valid(), "Couldn't get viewport texture's RID on the RenderingDevice");

		Ref<RDTextureFormat> texture_format = rd->texture_get_format(rd_texture_rid);
		ERR_FAIL_COND_MSG(texture_format.is_null(), "Couldn't get viewport texture format");

		rd->texture_get_data_async(rd_texture_rid, 0, callable_mp(this, &ViewportTextureRouter::forward_texture).bind(texture_format, vp->get_viewport_rid().get_id()));
	}
}

void ViewportTextureRouter::forward_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, int64_t p_viewport_rid) {
	emit_signal("texture_arrived", p_data, p_format, p_viewport_rid);
	p_data.resize(0);
}
