#include "./arp_table.h"

#include <netinet/in.h>
#include <arpa/inet.h>

/* --------------------------------------------------  CREATE ARP TABLE  -------------------------------------------------- */

/**
 * @brief Create a new ARP table.
 * 
 * Allocates memory for a new ARP table structure and initializes its fields.
 * 
 * @return A pointer to the newly created ARP table or NULL if memory allocation fails.
 */
arp_table* Create_ARP_Table(void) {
    // Allocate memory for the ARP table structure.
    arp_table *arp = malloc(sizeof(*arp));
    if (!arp) return NULL;

    // Allocate memory for the ARP table's address entries.
    arp->addrs = malloc(sizeof(*arp->addrs) * ARP_SIZE);
    if (!arp->addrs) {
        free(arp);
        return NULL;
    }

    // Initialize the ARP table's length to 0.
    arp->len = 0;
    return arp;
}

/* --------------------------------------------------  CREATE ARP TABLE  -------------------------------------------------- */
/* ---------------------------------------------------  FREE ARP TABLE  --------------------------------------------------- */

/**
 * @brief Free an ARP table.
 * 
 * Frees the memory associated with an ARP table, including its entries.
 * 
 * @param arp A pointer to the ARP table pointer to be freed.
 */
void Free_ARP_Table(arp_table **arp) {
    if (!arp || !(*arp)) return;
    // Free the memory occupied by the ARP table's address entries.
    free((*arp)->addrs);
     // Free the memory occupied by the ARP table structure.
    free(*arp);
    // Prevent further access.
    *arp = NULL;
}

/* ---------------------------------------------------  FREE ARP TABLE  --------------------------------------------------- */
/* ---------------------------------------------------  GET ARP ENTRY  --------------------------------------------------- */

/**
 * @brief Get the index of an ARP table entry.
 * 
 * Searches for an ARP table entry with a given IP address and returns its index.
 * 
 * @param arp The ARP table to search in.
 * @param ip  The IP address to search for.
 * @return The index of the entry in the ARP table or -1 if not found.
 */
int Get_ARP_Entry(arp_table *arp, uint32_t ip) {
    if (!arp || !arp->addrs) return -1;
    // Iterate through the ARP table's address entries.
    for (int entry = 0; entry < arp->len; entry++) {
        if (arp->addrs[entry].ip == ip) { // match the givne ip
            return entry;
        }
    }
    // No found entry.
    return -1;
}

/* ---------------------------------------------------   GET ARP ENTRY  --------------------------------------------------- */
/* --------------------------------------------------  INSERT ARP ENTRY  -------------------------------------------------- */

/**
 * @brief Insert an ARP table entry into the ARP table.
 * 
 * Insert a new ARP table entry with the given IP address and MAC address
 * into the ARP table, using the information parsed from a file.
 * 
 * @param arp  The ARP table to insert into.
 * @param path The path to the file containing the ARP table entry.
 * @return The number of unique entries added to the ARP table.
 */
void Insert_ARP_Entry(arp_table *arp, arp_entry *new_entry) {
    if (!arp || !arp->addrs || arp->len >= ARP_SIZE) return;

    // Check if the arp address already exists in the ARPs structure.
    int idx_entry = Get_ARP_Entry(arp, new_entry->ip);

    if (idx_entry < 0) {
        // Cache the new arp address if it doesn't exist in the ARPs structure.
        arp->addrs[arp->len].ip = new_entry->ip;
        memcpy(arp->addrs[arp->len].mac, new_entry->mac, MAC_SIZE);
        arp->len++;
    }
}

/* --------------------------------------------------  INSERT ARP ENTRY  -------------------------------------------------- */
