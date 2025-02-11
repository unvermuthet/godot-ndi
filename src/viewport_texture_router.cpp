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
	RenderingServer::get_singleton()->set_render_loop_enabled(false);
	RenderingServer::get_singleton()->set_block_signals(true);
	
	// UtilityFunctions::print("ViewportTextureRouter destructor");
}

void ViewportTextureRouter::add_viewport(Viewport *viewport) {
	ERR_FAIL_NULL_MSG(viewport, "Viewport is null");

	if (vps.size() == 0) {
		RenderingServer::get_singleton()->connect("frame_post_draw", callable_mp(this, &ViewportTextureRouter::request_textures));
	}

	if (!vps.has(viewport)) {
		vps.append(viewport);
	}
}

void ViewportTextureRouter::remove_viewport(Viewport *viewport) {
	ERR_FAIL_NULL_MSG(viewport, "Viewport is null");

	vps.erase(viewport);

	if (vps.size() == 0) {
		RenderingServer::get_singleton()->disconnect("frame_post_draw", callable_mp(this, &ViewportTextureRouter::request_textures));
	}
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

		rd->texture_get_data_async(rd_texture_rid, 0, callable_mp(this, &ViewportTextureRouter::forward_texture).bind(texture_format, vp));
		UtilityFunctions::print("texture requested");
	}
}

void ViewportTextureRouter::forward_texture(PackedByteArray p_data, const Ref<RDTextureFormat> &p_format, Viewport *p_viewport) {
	emit_signal("texture_arrived", p_data, p_format, p_viewport);
}

void ViewportTextureRouter::_bind_methods() {
	ADD_SIGNAL(MethodInfo("texture_arrived", PropertyInfo(Variant::PACKED_BYTE_ARRAY, "p_data"), PropertyInfo(Variant::OBJECT, "p_format"), PropertyInfo(Variant::OBJECT, "p_viewport")));
}
