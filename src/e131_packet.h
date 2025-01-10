#include <godot_cpp/classes/ref_counted.hpp>
#include <e131.h>

using namespace godot;

class E131Packet : public RefCounted {
    GDCLASS(E131Packet, RefCounted)

    private:
        e131_packet_t packet;

    protected:
        static void _bind_methods();
    
    public:
        E131Packet(const uint16_t universe, const uint16_t num_slots);
        bool get_option(const e131_option_t option);
        int set_option(const e131_option_t, const bool state);

};

VARIANT_ENUM_CAST(e131_error_t);
VARIANT_ENUM_CAST(e131_option_t);