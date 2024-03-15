#include "./include/router.h"
#include "./res/ipv4/ipv4.h"
#include "./res/arp/arp.h"

packet*     Send_Packet         (routing *route);
void        Waiting_Packet      (routing *route, packet *pkt);

/**
 * @brief Initialize a routing structure with required tables and queues.
 * 
 * Allocate memory for a routing structure and initializes its fields,
 * including an IPv4 routing table, an ARP table, and a waiting queue. If any of the
 * initialization steps fail, it deallocates previously allocated memory and returns NULL.
 * 
 * @param file A path to the file containing IPv4 routing table information.
 * @return     A pointer to the initialized routing structure or NULL on failure.
 */
static routing* Create_Router(char *file) {
    routing *route = (routing*)malloc(sizeof(routing));
    if (!route) return NULL;

    // Initialize the IPv4 routing table.
    route->ipv4s = Create_IPV4_Table(file);
    if (!route->ipv4s) {
        free(route);
        return NULL;
    }

    // Initialize the ARP table.
    route->macs = Create_ARP_Table();
    if (!route->macs) {
        Free_IPV4_Table(&route->ipv4s);
        free(route);
        return NULL;
    }

    // Initialize the waiting queue
    route->waiting = Queue();
    if (!route->waiting) {
        Free_ARP_Table(&route->macs);
        Free_IPV4_Table(&route->ipv4s);
        free(route);
        return NULL;
    }

    // Initialize other route fields.
    route->next_hop = 0;
    route->interface = 0;

    return route;
}

/**
 * @brief Free the memory allocated for a routing structure and its associated data structures.
 * 
 * Deallocate memory for a routing structure, including its IPv4 routing table,
 * ARP table, and waiting queue. It also takes care of freeing any associated 
 * memory within these data structures.
 * 
 * @param route   A pointer to the routing structure to be freed.
 */
static void Free_Router(routing *route) {
    if (!route) return;
    if (route->waiting) FreeQueue(route->waiting);
    if (route->macs)    Free_ARP_Table(&route->macs);
    if (route->ipv4s)   Free_IPV4_Table(&route->ipv4s);
    free(route);
}

int main(int argc, char **argv) {
    // Initialize network interfaces based on command line arguments
	// (excluding the program name and router configuration file).
    Init_Network(argc - 2, argv + 2);

    // Initialize the router based on the provided configuration file.
    routing *route = Create_Router(argv[1]);
    if (!route) return EXIT_FAILURE;

    while (true) {
        // Receive a network message on any network interface.
        route->len = 0;
        route->interface = Recv_FromAny_Link(route->buf, &route->len);

        // Check for errors when receiving a message.
        if (route->interface < 0) {
            Free_Router(route);
            fprintf(stderr, "ERROR: INTERFACE...");
            exit(EXIT_FAILURE);
        }

        // Initialize message fields in the routing structure.
        route->eth_hdr = (struct ethhdr *)route->buf;

        // Determine the type of the received packet.
        uint16_t PACKET_TYPE = route->eth_hdr->ether_type;

        // Check if the received packet type is valid (not NULL)
        if (!route || PACKET_TYPE != IP_TYPE || PACKET_TYPE != ARP_TYPE) {
            fprintf(stderr, "ERROR: TYPE PACKET...");
        }

        // Handle the received packet based on its type.
        if (PACKET_TYPE == IP_TYPE) {
            Handler_IPV4(route); // Handle IPv4 packets.
        }
        if (PACKET_TYPE == ARP_TYPE) {
            Handler_ARP(route); // Handle ARP packets.
        }
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Create and initialize a new packet based on the provided routing information.
 * 
 * Allocate memory for a new packet, copies the packet data and routing
 * information from the given routing structure, and returns the newly created packet.
 * 
 * @param route   A pointer to the routing structure containing packet and routing information.
 * @return        A pointer to the newly created packet, or NULL if memory allocation fails.
 */
packet* Send_Packet(routing *route) {
    if (!route) return NULL;

    // Allocate memory for a new packet structure.
    packet *pkt = (packet*)malloc(sizeof(packet));
    if (!pkt) return NULL;

    // Allocate memory for the packet buffer.
    pkt->buf = malloc(MAX_PACKET_LEN);

    if (!pkt->buf) {
        free(pkt);
        return NULL;
    }

    // Copy packet data, length, interface, and next hop information.
    memcpy(pkt->buf, route->buf, route->len);
    pkt->len = route->len;
    pkt->interface = route->interface;
    pkt->next_hop = route->next_hop;

    return pkt;
}

/**
 * @brief Initialize the routing structure using a waiting packet.
 * 
 * Extract relevant information from a waiting packet and sets up
 * the routing structure for further processing.
 * 
 * @param route A pointer to the routing structure to be initialized.
 * @param pkt   A pointer to the waiting packet containing Ethernet and IP headers.
 */
void Waiting_Packet(routing *route, packet *pkt) {
    // Extract Ethernet and IP headers from the waiting packet.
	route->eth_hdr = (struct ethhdr *)pkt->buf;
	route->ip_hdr = (struct iphdr *)(pkt->buf + sizeof *route->eth_hdr);

    // Set packet length, interface, and next hop information.
	route->len = pkt->len;
	route->interface = pkt->interface;
	route->next_hop = pkt->next_hop;
    // Set the Ethernet frame type to IP.
	route->eth_hdr->ether_type = IP_TYPE;

    // Copy the destination MAC address from the ARP header to the Ethernet header.
	memcpy(route->eth_hdr->ether_dhost, route->arp_hdr->sha, MAC_SIZE);
    // Get the source MAC address of the current interface.
	Get_MAC_Interface(route->interface, route->eth_hdr->ether_shost);
}
