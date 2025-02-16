/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "includes.hpp"

VideoStreamNDI::VideoStreamNDI() {
	name = nullptr;
	url = nullptr;
	bandwidth = NDIlib_recv_bandwidth_highest;

	finder = nullptr;

	ERR_FAIL_COND_MSG(!Engine::get_singleton()->has_singleton("GlobalNDIFinder"), "NDIFinder Singleton not found");
	finder = Object::cast_to<NDIFinder>(Engine::get_singleton()->get_singleton("GlobalNDIFinder"));
	finder->connect("sources_changed", callable_mp(this, &VideoStreamNDI::update_available_sources_hint));

	update_available_sources_hint();
}

VideoStreamNDI::VideoStreamNDI(const NDIlib_source_t p_source) :
		VideoStreamNDI::VideoStreamNDI() {
	name = p_source.p_ndi_name;
	url = p_source.p_url_address;
}

VideoStreamNDI::~VideoStreamNDI() {
	if (finder != nullptr) {
		finder->disconnect("sources_changed", callable_mp(this, &VideoStreamNDI::update_available_sources_hint));
	}
}

void VideoStreamNDI::set_name(const String p_name) {
	set_url("");

	if (p_name.is_empty()) {
		name = NULL;
		return;
	}

	name = p_name.utf8();

	if (finder != nullptr) {
		TypedArray<VideoStreamNDI> sources = finder->get_sources();

		for (int64_t i = 0; i < sources.size(); i++) {
			VideoStreamNDI *source = Object::cast_to<VideoStreamNDI>(sources[i]);
			if (get_name() == source->get_name()) {
				set_url(source->get_url());
				return;
			}
		}
	}
}

String VideoStreamNDI::get_name() const {
	if (finder != nullptr) {
		finder->update();
	}

	return String::utf8(name);
}

void VideoStreamNDI::set_url(const String p_url) {
	if (p_url.is_empty()) {
		url = nullptr;
		return;
	}

	url = p_url.utf8();
}

String VideoStreamNDI::get_url() const {
	return String::utf8(url);
}

void VideoStreamNDI::set_bandwidth(const NDIlib_recv_bandwidth_e p_bandwidth) {
	bandwidth = p_bandwidth;
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
	recv_desc.p_ndi_recv_name = nullptr;

	Ref<VideoStreamPlaybackNDI> pb = memnew(VideoStreamPlaybackNDI);
	pb->recv_desc = recv_desc;
	return pb;
}

void VideoStreamNDI::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_name", "p_name"), &VideoStreamNDI::set_name);
	ClassDB::bind_method(D_METHOD("get_name"), &VideoStreamNDI::get_name);
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name", PROPERTY_HINT_ENUM_SUGGESTION), "set_name", "get_name");

	// ClassDB::bind_method(D_METHOD("set_url", "p_url"), &VideoStreamNDI::set_url);
	// ClassDB::bind_method(D_METHOD("get_url"), &VideoStreamNDI::get_url);
	// ADD_PROPERTY(PropertyInfo(Variant::STRING, "url", PROPERTY_HINT_NONE), "set_url", "get_url");

	ClassDB::bind_method(D_METHOD("set_bandwidth", "p_bandwidth"), &VideoStreamNDI::set_bandwidth);
	ClassDB::bind_method(D_METHOD("get_bandwidth"), &VideoStreamNDI::get_bandwidth);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bandwidth", PROPERTY_HINT_ENUM, "Metadata only:-10,Audio only:10,Lowest:0,Highest:100"), "set_bandwidth", "get_bandwidth");

	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_metadata_only);
	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_audio_only);
	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_lowest);
	BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_highest);
}

void VideoStreamNDI::_validate_property(PropertyInfo &p_property) {
	if (p_property.name == StringName("name")) {
		p_property.hint_string = available_sources_hint;
	}
}

void VideoStreamNDI::update_available_sources_hint() {
	if (finder == nullptr) {
		return;
	}

	TypedArray<VideoStreamNDI> sources = finder->get_sources();
	PackedStringArray source_names;

	for (int64_t i = 0; i < sources.size(); i++) {
		source_names.push_back(Object::cast_to<VideoStreamNDI>(sources[i])->get_name());
	}

	available_sources_hint = String(",").join(source_names);
	notify_property_list_changed();
}
