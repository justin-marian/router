#include "./arp.h"

/* -----------------------------------------------------  ARP REPLY  ----------------------------------------------------- */

/**
 * @brief Reply to an ARP request by filling in
 * the ARP header and Ethernet header
 * with appropriate values for an ARP reply.
 * 
 * @param rout The rout structure containing ARP and Ethernet headers.
 */
void Reply_ARP(routing *rout) {
    // Set ARP operation to ARP reply.
	rout->arp_hdr->op = OP_REPLY;

    // Swap target and sender IP and MAC addresses.
	rout->arp_hdr->tpa = rout->arp_hdr->spa;
	memcpy(rout->arp_hdr->tha, rout->arp_hdr->sha, MAC_SIZE);

    // Set the sender IP and MAC addresses in the ARP header.
	Get_MAC_Interface(rout->interface, rout->arp_hdr->sha);
	rout->arp_hdr->spa = Get_IPV4_Interface(rout->interface);

    // Update Ethernet header with destination MAC and source MAC.
	memcpy(rout->eth_hdr->ether_dhost, rout->eth_hdr->ether_shost, MAC_SIZE);

    // Set the source MAC address in the Ethernet header.
	Get_MAC_Interface(rout->interface, rout->eth_hdr->ether_shost);
}

/* -----------------------------------------------------  ARP REPLY  ----------------------------------------------------- */
/* ----------------------------------------------------- ARP REQUEST ----------------------------------------------------- */

/**
 * @brief Initialize the Ethernet header in the rout structure for ARP requests.
 * 
 * @param rout The rout structure containing the Ethernet header.
 */
static void Init_ETH_Header(routing *rout) {
    // Set the Ethernet type to ARP.
    rout->eth_hdr->ether_type = ARP_TYPE;
    // Get the MAC address of the rout's interface and set it as the source address.
    Get_MAC_Interface(rout->interface, rout->eth_hdr->ether_shost);
    // Set the destination MAC address to broadcast.
    memset(rout->eth_hdr->ether_dhost, 0xff, MAC_SIZE);
}

/**
 * @brief Initialize the ARP header in the rout structure for ARP requests.
 * 
 * @param rout The rout structure containing the ARP header.
 */
static void Init_ARP_Header(routing *rout) {
    // Set the ARP header pointer to the appropriate location in the packet buffer.
    rout->arp_hdr = (struct arphdr *)(rout->buf + sizeof *rout->eth_hdr);

    // Set ARP header fields:
    rout->arp_hdr->htype = HTYPE_ETHER; // Hardware type (Ethernet)
    rout->arp_hdr->ptype = IP_TYPE;     // Protocol type (IPv4)
    rout->arp_hdr->hlen = HW_LEN;       // Hardware address length (MAC address size)
    rout->arp_hdr->plen = PT_LEN;       // Protocol address length (IPv4 address size)
    rout->arp_hdr->op = OP_REQUEST;     // ARP operation (Request)

    // Get the MAC address of the rout's interface and set it as the source hardware address.
    Get_MAC_Interface(rout->interface, rout->arp_hdr->sha);

    // Set the source protocol address to the IPv4 address of the rout's interface.
    rout->arp_hdr->spa = Get_IPV4_Interface(rout->interface);

    // Set the target hardware address to all zeros (to be filled in by the recipient).
    memset(rout->arp_hdr->tha, 0, MAC_SIZE);

    // Set the target protocol address to the next next_hop address.
    rout->arp_hdr->tpa = rout->next_hop;
}

/**
 * @brief Update the packet length in the rout structure based on Ethernet and ARP headers.
 * 
 * @param rout The rout structure containing the packet buffer and headers.
 */
static void Update_ARP_Length(routing *rout) {
    rout->len = sizeof(*rout->eth_hdr) + sizeof(*rout->arp_hdr);
}

/**
 * @brief Generate an ARP request packet in the rout structure.
 * 
 * @param rout The rout structure to store the ARP request packet.
 */
void Request_ARP(routing *rout) {
    // Initialize the Ethernet header for ARP requests.
    Init_ETH_Header(rout);
    // Initialize the ARP header for ARP requests.
    Init_ARP_Header(rout);
    // Update the packet length.
    Update_ARP_Length(rout);
}

/* ----------------------------------------------------- ARP REQUEST ----------------------------------------------------- */
/* ------------------------------------------------- HANDLER ARP PACKETS ------------------------------------------------- */

/**
 * @brief Handle incoming ARP packets in the rout.
 * 
 * @param rout The rout structure containing the received packet and ARP information.
 */
void Handler_ARP(routing *rout) {
    // Check if the rout and packet type are valid for ARP processing.
    if (!rout || rout->eth_hdr->ether_type != ARP_TYPE) return;

    // Point to the ARP header within the received packet.
    rout->arp_hdr = (struct arphdr *)(rout->buf + sizeof *rout->eth_hdr);

    // Check if the ARP operation is a request.
    if (rout->arp_hdr->op == OP_REQUEST) {
        // Reply to the ARP request.
        Reply_ARP(rout);
        // Send the ARP reply back to the sender.
        Send_To_Link(rout->interface, rout->buf, rout->len);
        return;
    }

    // Check if the ARP operation is a reply.
    if (rout->arp_hdr->op != OP_REPLY) return;

    arp_entry *entry = malloc(sizeof(arp_entry));
    if (entry) {
        entry->ip = rout->arp_hdr->spa;
        memcpy(entry->mac, rout->arp_hdr->sha, MAC_SIZE);
    } else {
        free(entry);
        return;
    }

    // Cache the new MAC address associated with the sender's IP address.
    Insert_ARP_Entry(rout->macs, entry);

    // Process and send waiting packets to the newly resolved MAC address.
    while (!EmptyQueue(rout->waiting)) {
        packet *pkt = Dequeue(rout->waiting);

        // Check if the waiting packet's destination matches the sender's IP address.
        if (pkt->next_hop == rout->arp_hdr->spa) {
            // Process the waiting packet.
            Waiting_Packet(rout, pkt);

            // Send the packet to the resolved MAC address.
            Send_To_Link(rout->interface, pkt->buf, rout->len);

            // Free the packet's resources.
            free(pkt->buf);
            free(pkt);
        } else {
            // Re-enqueue packets that are not intended for the resolved address.
            Enqueue(rout->waiting, (void *)pkt);
        }
    }

    free(entry);
}

/* ------------------------------------------------- HANDLER ARP PACKETS ------------------------------------------------- */
