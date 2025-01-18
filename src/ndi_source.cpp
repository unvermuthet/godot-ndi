#include "ndi_source.h"

void NDISource::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_name", "name"), &NDISource::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &NDISource::get_name);
    ClassDB::bind_method(D_METHOD("set_url", "url"), &NDISource::set_url);
    ClassDB::bind_method(D_METHOD("get_url"), &NDISource::get_url);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "url"), "set_url", "get_url");
}

NDISource::NDISource() {
    recv_desc.allow_video_fields = false;
    recv_desc.bandwidth = NDIlib_recv_bandwidth_highest;
    recv_desc.color_format = NDIlib_recv_color_format_RGBX_RGBA;
    recv_desc.p_ndi_recv_name = NULL;
}

NDISource::NDISource(NDIlib_source_t source) {
    NDISource();
    recv_desc.source_to_connect_to = source;
}

void NDISource::set_name(const String name) {
    recv_desc.source_to_connect_to.p_ndi_name = name.utf8().ptr();
}

String NDISource::get_name() const {
    if (!recv_desc.source_to_connect_to.p_ndi_name) {
        return String();
    }
	return String(recv_desc.source_to_connect_to.p_ndi_name);
}

void NDISource::set_url(const String url) {
    recv_desc.source_to_connect_to.p_url_address = url.utf8().ptr();
}

String NDISource::get_url() const {
    if (!recv_desc.source_to_connect_to.p_url_address) {
        return String();
    }
	return String(recv_desc.source_to_connect_to.p_url_address);
}

void NDISource::set_bandwidth(const NDIlib_recv_bandwidth_e bandwidth) {
    recv_desc.bandwidth = bandwidth;
}

NDIlib_recv_bandwidth_e NDISource::get_bandwidth() const {
	return recv_desc.bandwidth;
}

void NDISource::connect() {
    recv = lib->NDIlib_recv_create_v3(&recv_desc);
    sync = lib->NDIlib_framesync_create(recv);
}

void NDISource::disconnect() {
    lib->NDIlib_recv_destroy(recv);
    lib->NDIlib_framesync_destroy(sync);
}
