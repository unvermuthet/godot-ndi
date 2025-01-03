#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

class SACN : public RefCounted {
    GDCLASS(SACN, RefCounted)

    protected:
        static void _bind_methods();
    
    public:
        int number();
        SACN();
        ~SACN();
};