#include "./icmp.h"
#include "../ipv4/ipv4.h"

/* ----------------------------------------------------- ICMP REPLY ----------------------------------------------------- */

/**
 * @brief Set ICMP header fields in the rout's packet buffer.
 * Set the ICMP header fields in the packet buffer of the rout.
 * 
 * It calculates pointers to the ICMP header and updates the total length of the packet.
 * If the ICMP message type is ICMP_TIME_EXCED / ICMP_DEST_UNREACH, it copies the IP header (20 bytes),
 * the next 8 bytes to the packet buffer to include them in the ICMP message.
 * 
 * @param rout Pointer to the rout data structure.
 * @param type ICMP message type (e.g., ICMP_TIME_EXCED or ICMP_DEST_UNREACH).
 */
static void Init_ICMP_Header(routing *rout, uint8_t type) {
    // Calculate pointers to the ICMP header and the total length.
    rout->icmp_hdr = (struct icmphdr *)(rout->buf + sizeof *rout->eth_hdr + sizeof *rout->ip_hdr);
    rout->len = sizeof *rout->eth_hdr + sizeof *rout->ip_hdr + sizeof *rout->icmp_hdr;

    // Copy the IP header (20 bytes) and the next 8 bytes if required.
    if (type == ICMP_TIME_EXCED || type == ICMP_DEST_UNREACH) {
        memcpy(rout->buf + rout->len, rout->ip_hdr, 28);
        rout->len += 28;
    }

    // Set ICMP code and type.
    rout->icmp_hdr->code = 0;
    rout->icmp_hdr->type = type;
}

/**
 * @brief Update the ICMP checksum in the rout's packet buffer.
 * 
 * Calculate and update the ICMP checksum in the rout's packet buffer for the ICMP header.
 * 
 * @param rout Pointer to the rout data structure.
 */
static void Checksum_ICMP(routing *rout) {
    rout->icmp_hdr->checksum = 0;
    rout->icmp_hdr->checksum = htons(Checksum((uint16_t *)rout->icmp_hdr, sizeof *rout->icmp_hdr));
}

/**
 * @brief Generate a new IPv4 header for ICMP messages.
 * 
 * Generate a new IPv4 header for ICMP messages in the rout's packet buffer.
 * It is used when creating ICMP replies or error messages (wrapper function)
 * 
 * @param rout Pointer to the rout data structure.
 * @param type   ICMP message type (e.g., ICMP_TIME_EXCED / ICMP_DEST_UNREACH).
 */
static void Header_NewIP(routing *rout, uint8_t type) { 
    Header_IPV4(rout, type);
}

/**
 * @brief Update the Ethernet header in the rout's packet buffer.
 * 
 * Update the Ethernet header in the rout's packet buffer with the appropriate
 * source and destination MAC addresses based on the rout's interface.
 * 
 * @param rout Pointer to the rout data structure.
 */
static void Header_NewETH(routing *rout) {
    memcpy(rout->eth_hdr->ether_dhost, rout->eth_hdr->ether_shost, MAC_SIZE);
    Get_MAC_Interface(rout->interface, rout->eth_hdr->ether_shost);
}

/**
 * @brief Generate an ICMP reply message in the rout's packet buffer.
 * 
 * @param rout Pointer to the rout data structure.
 * @param type ICMP message type (e.g., ICMP_TIME_EXCED / ICMP_DEST_UNREACH).
 */
void Reply_ICMP(routing *rout, uint8_t type) {
    Init_ICMP_Header(rout, type);
    Checksum_ICMP(rout);
    /* ---------------------- */
    Header_NewIP(rout, type);
    Header_NewETH(rout);
}

/* ----------------------------------------------------- ICMP REPLY ----------------------------------------------------- */
