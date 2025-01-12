#include <godot_cpp/classes/ref_counted.hpp>
#include <e131.h>

using namespace godot;

class E131Socket : public RefCounted {
    GDCLASS(E131Socket, RefCounted)

    private:
        int sockfd;
        e131_addr_t dest;

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
        ssize_t send(const E131Packet packet);
        E131Packet recv();
};

// VARIANT_ENUM_CAST(e131_error_t);
// VARIANT_ENUM_CAST(e131_option_t);