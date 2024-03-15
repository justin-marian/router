#pragma once

#ifndef IPV4_TABLE_H_
#define IPV4_TABLE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 64
#define MAX_LINES 100001

// ROUTES ARE STATIC ALLOCATED!

// Routing entry in an IPv4 routing table.
typedef struct route {
    uint32_t prefix;            // Destination IP address prefix.
    uint32_t next_hop;          // Next Hop IP address.
    uint32_t mask;              // Subnet mask.
    int interface;              // Interface index.
} route;

// Forwarding entry in an IPv4 routing table.
typedef struct forward {
    uint32_t next_hop;          // Next Hop IP address.
    int interface;              // Interface index.
    bool status;                // Status flag (INVALID / VALID).
} forward;

// Entry in an IPv4 routing table.
typedef struct ipv4_entry {
    uint32_t next_hop;          // Next Hop IP address.
    int type;                   // Type falg (INVALID -1 / EMPTY 0 / VALID 1)
    int interface;              // Interface index.
    struct ipv4_entry *left;    // Left child entry.
    struct ipv4_entry *right;   // Right child entry.
} ipv4_entry;

// An IPv4 routing table.
typedef struct ipv4_table {
    ipv4_entry *root;           // Root entry of the routing table.
    size_t size;                // Number of entries in the routing table.
} ipv4_table;

/** @brief Create an empty IPv4 routing table. */
ipv4_table*     CreateEmpty_IPV4_Table          (void);
/** @brief Create an IPv4 routing table from a file containing routing entries. */
ipv4_table*     Create_IPV4_Table               (char *file);

/** @brief Free the memory associated with an IPv4 routing table. */
void            Free_IPV4_Table                 (ipv4_table **ip_table);
/** @brief Insert a new IPv4 routing table entry into an IPv4 routing table. */
void            Insert_IPV4_Table               (ipv4_table *ip_table, route *new_entry);

/** @brief Perform Longest Prefix Match (LPM) in an IPv4 routing table. */
forward*        LPM_IPV4_Table                  (ipv4_table *ip_table, uint32_t ip);

#endif /* IPV4_TABLE_H_ */
