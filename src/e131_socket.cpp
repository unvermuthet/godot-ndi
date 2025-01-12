#include <godot_cpp/core/class_db.hpp>

#include "e131.h"
#include "e131_packet.h"
#include "e131_socket.h"

void E131Socket::_bind_methods() {
}

E131Socket::E131Socket() {
    int s = e131_socket();
    ERR_FAIL_COND_MSG(s == -1, "Failed to create socket");
    sockfd = s;
}

int E131Socket::bind(const uint16_t port) {
	return e131_bind(sockfd, port);
}

int E131Socket::unicast_dest(const String host, const uint16_t port) {
	return e131_unicast_dest(&dest, host.utf8().ptr(), port);
}

int E131Socket::multicast_dest(const uint16_t universe, const uint16_t port) {
	return e131_multicast_dest(&dest, universe, port);
}

int E131Socket::multicast_iface(const int ifindex) {
	return e131_multicast_iface(sockfd, ifindex);
}

int E131Socket::multicast_join(const uint16_t universe) {
	return e131_multicast_join(sockfd, universe);
}

int E131Socket::multicast_join_iface(const uint16_t universe, const int ifindex) {
	return e131_multicast_join_iface(sockfd, universe, ifindex);
}

int E131Socket::multicast_join_ifaddr(const uint16_t universe, const String ifaddr) {
	return e131_multicast_join_ifaddr(sockfd, universe, ifaddr.utf8().ptr());
}

ssize_t E131Socket::send(const E131Packet packet) {
	return e131_send(sockfd, &packet.packet, &dest);
}

E131Packet E131Socket::recv() {
	return E131Packet();
}
