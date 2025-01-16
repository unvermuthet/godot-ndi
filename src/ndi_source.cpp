#include <godot_cpp/core/class_db.hpp>

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
}

void NDISource::set_name(const String name) {
    source.p_ndi_name = name.utf8().ptr();
}

String NDISource::get_name() const {
    if (!source.p_ndi_name) {
        return String();
    }
	return String(source.p_ndi_name);
}

void NDISource::set_url(const String url) {
    source.p_url_address = url.utf8().ptr();
}

String NDISource::get_url() const {
    if (!source.p_url_address) {
        return String();
    }
	return String(source.p_url_address);
}

