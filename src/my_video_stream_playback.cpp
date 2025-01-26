#include "my_video_stream_playback.hpp"

MyVideoStreamPlayback::MyVideoStreamPlayback() {
    texture.instantiate();
}

MyVideoStreamPlayback::~MyVideoStreamPlayback() {
    texture.unref();
}

Ref<Texture2D> MyVideoStreamPlayback::_get_texture() const {
	return texture;
}

void MyVideoStreamPlayback::_update(double p_delta) {
    texture->set_image(Image::load_from_file("res://big.png"));
}

void MyVideoStreamPlayback::_play() {
    playing = true;
}

void MyVideoStreamPlayback::_stop() {
    playing = false;
}

bool MyVideoStreamPlayback::_is_playing() const {
	return playing;
}

void MyVideoStreamPlayback::_set_paused(bool p_paused) {
    paused = p_paused;
}

bool MyVideoStreamPlayback::_is_paused() const {
	return paused;
}

void MyVideoStreamPlayback::_bind_methods() { }
