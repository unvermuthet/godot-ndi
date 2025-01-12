#include <godot_cpp/classes/ref_counted.hpp>

#include "e131.h"

using namespace godot;

class E131Socket : public RefCounted {
    GDCLASS(E131Socket, RefCounted)

    private:
        int sockfd;
        e131_addr_t dest;
        uint8_t last_seq_send = 0x00;
        uint8_t last_seq_recv = 0x00;

    protected:
        static void _bind_methods();
    
    public:
        E131Socket();
        int bind(const uint16_t port);
        int unicast_dest(const String host, const uint16_t port);
        int multicast_dest(const uint16_t universe, const uint16_t port);
        int multicast_iface(const int ifindex);
        int multicast_join(const uint16_t universe);
        int multicast_join_iface(const uint16_t universe, const int ifindex);
        int multicast_join_ifaddr(const uint16_t universe, const String ifaddr);
        Error send(const E131Packet packet);
        Error recv(E131Packet packet);
};

// VARIANT_ENUM_CAST(e131_error_t);
// VARIANT_ENUM_CAST(e131_option_t);