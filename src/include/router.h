#ifndef ROUTER_H_
#define ROUTER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "../utils/lib.h"
#include "../utils/queue.h"

#include "../include/protocols.h"

#include "../res/arp/arp_table.h"
#include "../res/ipv4/ipv4_table.h"

typedef struct packet {
	char *buf;
	size_t len;
	int interface;
	uint32_t next_hop;
} packet;

typedef struct routing {
	ipv4_table *ipv4s;						/* ROUTING TABLE */
	arp_table  *macs;						/* ARP TABLE ~ MAC TABLE */

	queue waiting;							/* Waiting packets, ARP Reply type packets */

	ethhdr eth_hdr;							/* Ethernet Header */
	iphdr ip_hdr;							/* IP Header */
	arphdr arp_hdr;							/* ARP Header */
	icmphdr icmp_hdr;						/* ICMP Header */

	char buf[MAX_PACKET_LEN];				/* Packet buffer */
	size_t len;								/* Length of the buffer, read from the network */

	uint32_t next_hop;						/* Next hop best forwarding interface to send the packet */
	int interface;							/* Interface to receive/send packets */
} routing;

/** @brief Pack a network message for transmission. */
extern packet* Send_Packet(routing *route);

/** @brief  Pack a waiting message for transmission. */
extern void Waiting_Packet(routing *route, packet *pkt);

#endif /* ROUTER_H_ */