#include "SACN.h"
#include <godot_cpp/core/class_db.hpp>

SACN::~SACN() {
}
void SACN::_bind_methods() {
    ClassDB::bind_method(D_METHOD("number"), &SACN::number);
    
}
int SACN::number() {
	return 42;
}
SACN::SACN() {
}