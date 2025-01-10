#include <godot_cpp/classes/ref_counted.hpp>
#include <e131.h>

using namespace godot;

class E131Socket : public RefCounted {
    GDCLASS(E131Socket, RefCounted)

    private:
        int sockfd;
        e131_addr_t dest;
        e131_packet_t packet;

    protected:
        static void _bind_methods();
    
    public:
        E131Socket(int port);
        int unicast_dest(String host, int port);
        int multicast_dest(int universe, int port);
        int multicast_iface(int ifindex);
        int multicast_join(int universe);
        int multicast_join_iface(int universe, int ifindex);
        int multicast_join_ifaddr(int universe, String ifaddr);
        int pkt_init(int universe, int num_slots);
        bool get_option(e131_option_t option);
        int set_option(e131_option_t option, bool state);
        ssize_t send();
        ssize_t recv();
        e131_error_t pkt_validate();
        bool pkt_discard(uint8_t last_seq_number);
        int pkt_dump();

};

VARIANT_ENUM_CAST(e131_error_t);
VARIANT_ENUM_CAST(e131_option_t);