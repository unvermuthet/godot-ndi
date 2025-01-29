#include "ndi_output.hpp"

void NDIOutput::_bind_methods() {
}

NDIOutput::NDIOutput() {
}

NDIOutput::~NDIOutput() {
}

void NDIOutput::_ready() {
}

void NDIOutput::_exit_tree() {
}

void NDIOutput::_process(double delta) {
	Viewport* vp = get_viewport();

	if (vp == NULL) {
		return;
	}
	
	Ref<ViewportTexture> texture = vp->get_texture();

	if (texture.is_valid()) {
		UtilityFunctions::print(texture->get_width(), texture->get_height());
	}
}