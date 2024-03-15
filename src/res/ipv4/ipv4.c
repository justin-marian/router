#include "./ipv4.h"
#include "../arp/arp.h"
#include "../icmp/icmp.h"

/* ----------------------------------------------------- HEADER IPV4 ----------------------------------------------------- */

/**
 * @brief Set the IPv4 header fields based on the specified type.
 * 
 * @param route Pointer to the routing information structure.
 * @param type  Type of ICMP packet (ICMP_TIME_EXCED or ICMP_DEST_UNREACH).
 */
static void Set_IPV4_Fields(routing *route, uint8_t type) {
    struct iphdr *ip_hdr = route->ip_hdr;

    // Set IP header fields.
    ip_hdr->ihl = IPV4_IHL;
    ip_hdr->version = IPV4_VERSION;
    ip_hdr->tos = 0;
    ip_hdr->id = htons(1);
    ip_hdr->frag_off = 0;
    ip_hdr->ttl = DEFAULT_TTL;
    ip_hdr->protocol = 1;
    ip_hdr->check = 0;

    // Compute total length
    uint16_t total_length = sizeof(struct iphdr) + sizeof(struct icmphdr);
    if (type == ICMP_TIME_EXCED || type == ICMP_DEST_UNREACH) {
        total_length += sizeof(struct iphdr) + 8;
    }
    ip_hdr->tot_len = htons(total_length);

    // Set source and destination addresses
    uint32_t src_addr = Get_IPV4_Interface(route->interface);
    ip_hdr->saddr = src_addr;
    // Reverse source and destination
    ip_hdr->daddr = route->ip_hdr->saddr;
}

/**
 * @brief Update the IPv4 header checksum.
 * 
 * @param route Pointer to the routing information structure.
 */
static void Update_IPV4_Checksum(routing *route) {
    struct iphdr *ip_hdr = route->ip_hdr;
    ip_hdr->check = 0;    // Update checksum
    ip_hdr->check = htons(Checksum((uint16_t *)ip_hdr, sizeof(struct iphdr)));
}

/**
 * @brief Create the IPv4 header for ICMP packets and update checksum.
 * 
 * @param route Pointer to the routing information structure.
 * @param type  Type of ICMP packet (ICMP_TIME_EXCED or ICMP_DEST_UNREACH).
 */
void Header_IPV4(routing *route, uint8_t type) {
    Set_IPV4_Fields(route, type);
    Update_IPV4_Checksum(route);
}

/* ----------------------------------------------------- HEADER IPV4 ----------------------------------------------------- */
/* ------------------------------------------------- HANDLER ARP PACKETS ------------------------------------------------- */

/**
 * @brief Handle incoming IPv4 packets.
 * 
 * @param route Pointer to the routing information structure.
 */
void Handler_IPV4(routing *route) {
    // Extract the IPv4 header from the received packet
    route->ip_hdr = (struct iphdr *)(route->buf + sizeof *route->eth_hdr);
    
    // Store the original checksum and reset it for recalculation
    uint16_t old_check = route->ip_hdr->check;
    route->ip_hdr->check = 0;

    // Check if the checksum is invalid, and return early if it doesn't match
    if (old_check != htons(Checksum((uint16_t *)route->ip_hdr, sizeof *route->ip_hdr))) {
        return; // Invalid checksum, drop the packet
    }

    // Check if the destination IP address doesn't match the interface's IP
    if (route->ip_hdr->daddr != Get_IPV4_Interface(route->interface)) {
        // Look up the best route based on the destination IP address
        forward *best_route = LPM_IPV4_Table(route->ipv4s, route->ip_hdr->daddr);

        if (best_route) {
            // Update the routing information with the best route
            route->next_hop = best_route->next_hop;
            route->interface = best_route->interface;

            // Free the memory allocated for the best route entry
            free(best_route);

            // Continue with the main logic since the destination IP doesn't match
            if (route->ip_hdr->ttl > 1) {
                // Recalculate the checksum for the modified TTL
                route->ip_hdr->check = 0;
                route->ip_hdr->check = ~(~old_check +
                                         ~((uint16_t)route->ip_hdr->ttl) +
                                         (uint16_t)(route->ip_hdr->ttl - 1)) - 1;
                route->ip_hdr->ttl -= 1;

                // Check if there is an ARP entry for the next hop
                int entry_idx = Get_ARP_Entry(route->macs, route->next_hop);

                if (entry_idx < 0) {
                    // Send an ARP request to resolve the next hop's MAC address
                    packet *pckg = Send_Packet(route);
                    if (pckg) Enqueue(route->waiting, (void *)pckg);
                    Request_ARP(route);
                } else {
                    // Update the Ethernet frame with the destination MAC address
                    memcpy(route->eth_hdr->ether_dhost, route->macs->addrs[entry_idx].mac, MAC_SIZE);
                    // Get the source MAC address of the current interface
                    Get_MAC_Interface(route->interface, route->eth_hdr->ether_shost);
                }
            } else {
                // TTL expired, send ICMP Time Exceeded message
                Reply_ICMP(route, ICMP_TIME_EXCED);
            }
        } else {
            // No valid route found, send ICMP Destination Unreachable message
            Reply_ICMP(route, ICMP_DEST_UNREACH);
        }
    } else {
        // Destination IP matches the interface's IP, send ICMP Response message
        Reply_ICMP(route, ICMP_RESPONE);
    }

    // Continue with the main logic by forwarding the packet to the appropriate interface
    Send_To_Link(route->interface, route->buf, route->len);
}
