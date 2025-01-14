#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/thread.hpp>

#include "e131.h"
#include "e131_packet.h"
#include "e131_socket.h"

void E131Socket::_bind_methods() {
    ClassDB::bind_method(D_METHOD("bind", "port"), &E131Socket::bind);
    ClassDB::bind_method(D_METHOD("unicast_dest", "host", "port"), &E131Socket::unicast_dest);
    ClassDB::bind_method(D_METHOD("multicast_dest", "universe", "port"), &E131Socket::multicast_dest);
    ClassDB::bind_method(D_METHOD("multicast_iface", "ifindex"), &E131Socket::multicast_iface);
    ClassDB::bind_method(D_METHOD("multicast_join", "universe"), &E131Socket::multicast_join);
    ClassDB::bind_method(D_METHOD("multicast_join_iface", "universe", "ifindex"), &E131Socket::multicast_join_iface);
    ClassDB::bind_method(D_METHOD("multicast_join_ifaddr", "universe", "ifaddr"), &E131Socket::multicast_join_ifaddr);
    ClassDB::bind_method(D_METHOD("send", "packet"), &E131Socket::send);
    ClassDB::bind_method(D_METHOD("recv", "packet"), &E131Socket::recv);
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

Error E131Socket::send(const Ref<E131Packet> packet) {
	e131_error_t err = e131_pkt_validate(&packet.ptr()->_packet);
	ERR_FAIL_COND_V_MSG(err != E131_ERR_NONE, ERR_INVALID_DATA, e131_strerror(err));

	e131_packet_t p = packet.ptr()->_packet;
	p.frame.seq_number = last_seq_send + 1;

	ERR_FAIL_COND_V_MSG(e131_send(sockfd, &p, &dest) == -1, ERR_FILE_CANT_WRITE, strerror(errno));

	last_seq_send++;
	return OK;
}

Error E131Socket::recv(Ref<E131Packet> packet) {
	e131_packet_t p;
	ERR_FAIL_COND_V_MSG(e131_recv(sockfd, &p) == -1, ERR_FILE_CANT_READ, strerror(errno));

	e131_error_t err = e131_pkt_validate(&p);
	if (err != E131_ERR_NONE) {
		WARN_PRINT(e131_strerror(err));
		return ERR_INVALID_DATA;
	}

	if (e131_pkt_discard(&p, last_seq_recv)) {
		WARN_PRINT("Packet received out of order");
		return ERR_INVALID_DATA;
	}
	last_seq_recv = p.frame.seq_number;

	packet.ptr()->_packet = p;
	return OK;
}
