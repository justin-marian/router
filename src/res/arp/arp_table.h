#pragma once

#ifndef ARP_TABLE_H_
#define ARP_TABLE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAC_SIZE 6
#define ARP_SIZE 1001

// ROUTES ARE STATIC ALLOCATED!

// ARP (Address Resolution Protocol) entry
typedef struct arp_entry {
    uint32_t ip;            // IP address in network byte order
    uint8_t mac[MAC_SIZE];  // MAC address in binary form
} arp_entry;

// ARP (Address Resolution Protocol) table.
typedef struct arp_table {
    arp_entry *addrs;       // Array of ARP entries.
    int len;                // Number entries in the table.
} arp_table;

/** @brief Create a new ARP table. */
arp_table*      Create_ARP_Table        (void);
/** @brief Free an ARP table.
void            Free_ARP_Table          (arp_table **arp);

/** @brief Get the index of an ARP table entry.  */
int             Get_ARP_Entry           (arp_table *arp, uint32_t ip);
/** @brief Insert an ARP table entry. */
void            Insert_ARP_Entry        (arp_table *arp, arp_entry *new_entry);

#endif /* ARP_TABLE_H_ */