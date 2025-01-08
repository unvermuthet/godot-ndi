#include <godot_cpp/classes/ref_counted.hpp>
#include <e131.h>

using namespace godot;

class SACN : public RefCounted {
    GDCLASS(SACN, RefCounted)

    protected:
        static void _bind_methods();
    
    public:
        static int socket(void);
        SACN();
        ~SACN();
};