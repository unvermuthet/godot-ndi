/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "video_stream_ndi.hpp"

// The only persistent parts of this resource are the name and bandwidth fields.
// I've decided to not expose/bind the url field of the NDI_source_t struct.
// Its only set when obtaining a VideoStreamNDI from NDIFind.
// When creating this resource by hand or loading it from disk the url field stays NULL.
// The SDK will spin up a find instance itself and obtain the url that way.
// Read more in ndi/Processing.NDI.structs.h at line 182

void VideoStreamNDI::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_name", "name"), &VideoStreamNDI::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &VideoStreamNDI::get_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

	// ClassDB::bind_method(D_METHOD("set_url", "url"), &VideoStreamNDI::set_url);
	ClassDB::bind_method(D_METHOD("get_url"), &VideoStreamNDI::get_url);
	// ADD_PROPERTY(PropertyInfo(Variant::STRING, "url"), "set_url", "get_url");

	ClassDB::bind_method(D_METHOD("set_bandwidth", "bandwidth"), &VideoStreamNDI::set_bandwidth);
	ClassDB::bind_method(D_METHOD("get_bandwidth"), &VideoStreamNDI::get_bandwidth);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bandwidth", PROPERTY_HINT_ENUM, "Metadata only:-10,Audio only:10,Lowest:0,Highest:100"), "set_bandwidth", "get_bandwidth");

	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_metadata_only);
	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_audio_only);
	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_lowest);
	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_highest);
}

VideoStreamNDI::VideoStreamNDI() {
	name = NULL;
	url = NULL;
	bandwidth = NDIlib_recv_bandwidth_highest;
}

VideoStreamNDI::VideoStreamNDI(const NDIlib_source_t p_source) : VideoStreamNDI::VideoStreamNDI() {
	name = p_source.p_ndi_name;
	url = p_source.p_url_address;
}

VideoStreamNDI::~VideoStreamNDI() {}

void VideoStreamNDI::set_name(const String p_name) {
	if (p_name.is_empty()) {
		name = NULL;
	} else {
		name = p_name.utf8();
	}
	emit_changed();
}

String VideoStreamNDI::get_name() const {
	return String::utf8(name);
}

// void VideoStreamNDI::set_url(const String p_url) {
// 	if (p_url.is_empty()) {
// 		url = NULL;
// 	} else {
// 		url = p_url.utf8();
// 	}
// 	emit_changed();
// }

String VideoStreamNDI::get_url() const {
	return String::utf8(url);
}

void VideoStreamNDI::set_bandwidth(const NDIlib_recv_bandwidth_e p_bandwidth) {
	bandwidth = p_bandwidth;    
	emit_changed();
}

NDIlib_recv_bandwidth_e VideoStreamNDI::get_bandwidth() const {
	return bandwidth;
}

Ref<VideoStreamPlayback> VideoStreamNDI::_instantiate_playback() {
	NDIlib_source_t source;
	source.p_ndi_name = name;
	source.p_url_address = url;

	NDIlib_recv_create_v3_t recv_desc;
	recv_desc.source_to_connect_to = source;
	recv_desc.color_format = NDIlib_recv_color_format_RGBX_RGBA;
	recv_desc.bandwidth = bandwidth;
	recv_desc.allow_video_fields = false;
	recv_desc.p_ndi_recv_name = NULL;

	Ref<VideoStreamPlaybackNDI> p = memnew(VideoStreamPlaybackNDI);
	p->recv_desc = recv_desc;
	return p;
}
