#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

#include "e131.h"

using namespace godot;

class E131Packet : public RefCounted {
    GDCLASS(E131Packet, RefCounted)

    protected:
        static void _bind_methods();
    
    public:
        E131Packet();

        e131_packet_t packet;

        void set_option(const e131_option_t option, const bool state);
        bool get_option(const e131_option_t option) const;

        void set_source_name(const String name);
        String get_source_name() const;

        void set_priority(const uint8_t priority);
        uint8_t get_priority() const;

        void set_data(const PackedByteArray data);
        PackedByteArray get_data() const;

        void set_universe(const uint16_t universe);
        uint16_t get_universe() const;
};

VARIANT_ENUM_CAST(e131_option_t);