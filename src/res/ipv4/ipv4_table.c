#include "./ipv4_table.h"

/* ----------------------------------------------- CREATE IPV4 TABLE ----------------------------------------------- */

/**
 * @brief Create a new IPv4 routing table entry.
 * 
 * Allocate memory for a new IPv4 routing table entry,
 * initializes its fields, and returns a pointer to the
 * newly created entry.
 * 
 * @return A pointer to the newly created IPv4 routing table entry,
 *         or NULL if memory allocation fails.
 */
static ipv4_entry* Create_IPV4_Entry(void) {
    // Allocate memory for the new IPv4 entry.
    ipv4_entry *entry = (ipv4_entry*)malloc(sizeof(ipv4_entry));
    if (!entry) return NULL;

    // Initialize fields of the new entry, with default values.
    entry->interface = -1;
    entry->type = 0;
    entry->next_hop = 0;

    // Initialize left and right pointers for tree-based data structures.
    entry->left = NULL;
    entry->right = NULL;

    // Return a pointer to the newly created IPv4 entry.
    return entry;
}

/**
 * @brief Create an empty IPv4 routing table.
 * 
 * Allocate memory for a new IPv4 routing table, initializes its root entry,
 * and returns a pointer to the newly created routing table.
 * 
 * @return A pointer to the newly created IPv4 routing table,
 *         or NULL if memory allocation fails.
 */
ipv4_table* CreateEmpty_IPV4_Table(void) {
    // Allocate memory for the new IPv4 table.
    ipv4_table *ip_table = (ipv4_table*)malloc(sizeof(ipv4_table));
    if (!ip_table) return NULL;

    // Create the root entry for the routing table.
    ip_table->root = Create_IPV4_Entry();
    if (!ip_table->root) {
        free(ip_table);
        return NULL;
    }

    // Initialize fields of the new table, with default values.
    ip_table->root->type = -1;
    ip_table->size = 0;

    // Return a pointer to the newly created IPv4 routing table.
    return ip_table;
}

/**
 * @brief Read IPv4 routing entries from a file and populates an array of route structures.
 * 
 * Read IPv4 routing entries from a file, parses each line, and populates
 * an array of route structures with the parsed information.
 * 
 * @param file   The name of the file containing IPv4 routing entries.
 * @param rtable An array of route structures to store the parsed routing entries.
 * @return The number of routing entries read from the file, or -1 on failure.
 */
static int Read_IPV4_Table(char *file, route *rtable) {
    FILE *fin = fopen(file, "r");
    if (!fin) return -1; // File opening failed

    int num_entries = 0;
    char line[MAX_LINE_SIZE];

    // Parse each line from the routing table.
    while (fgets(line, sizeof(line), fin)) {
        int byte = 0;
        char *token = strtok(line, " .");
        // Each line from the routing contains PREFIX NEXT_HOP MASK INTERFACE in this order.
        while (token && byte < 13) {
            switch (byte / 4) {
                case 0: // 0 - 3 PREFIX
                    rtable[num_entries].prefix |= atoi(token) << (byte % 4) * 8;
                    break;
                case 1: // 4 - 7 NEXT_HOP
                    rtable[num_entries].next_hop |= atoi(token) << (byte % 4) * 8;
                    break;
                case 2: // 8 - 11 MASK
                    rtable[num_entries].mask |= atoi(token) << (byte % 4) * 8;
                    break;
                case 3: // 12 INTERFACE
                    rtable[num_entries].interface = atoi(token);
                    break;
            }

            token = strtok(NULL, " .");
            byte++;
        }

        num_entries++;
    }

    fclose(fin);
    return num_entries;
}

/**
 * @brief Create an IPv4 routing table from a file containing routing entries.
 * 
 * Create an IPv4 routing table by reading routing entries from a file.
 * It populates the routing table with the parsed entries and returns a pointer to the
 * newly created routing table.
 * 
 * @param file The name of the file containing IPv4 routing entries.
 * @return A pointer to the newly created IPv4 routing table,
 *         or NULL on failure.
 */
ipv4_table* Create_IPV4_Table(char *file) {
    if (!file) return NULL;

    // Create an empty IPv4 routing table.
    ipv4_table *ip_table = CreateEmpty_IPV4_Table();
    if (!ip_table) return NULL;

    // Define an array to store parsed routing entries.
    route rtable[MAX_LINES];

    // Read routing entries from the file and get the number of entries.
    int num_entries = Read_IPV4_Table(file, rtable);
    if (num_entries < 0) {
        Free_IPV4_Table(&ip_table);
        return NULL;
    }

    // Insert parsed routing entries into the routing table.
    for (int entry = 0; entry < num_entries; entry++) {
        Insert_IPV4_Table(ip_table, &rtable[entry]);
    }

    return ip_table;
}

/* ------------------------------------------------- CREATE IPV4 TABLE --------------------------------------------------- */
/* -------------------------------------------------- FREE IPV4 TABLE ---------------------------------------------------- */

/**
 * @brief Recursively frees the memory associated with an IPv4 routing table entry.
 * 
 * Recursively frees the memory associated with an IPv4 routing table entry
 * and its child entries in a tree-like structure, inorder traverse.
 * 
 * @param entry A pointer to the IPv4 routing table entry to be freed.
 */
static void Rec_Free_IPV4_Table(ipv4_entry *entry) {
    if (!entry) return;

    // Store references to the left and right child entries.
    ipv4_entry *left_entry = entry->left;
    ipv4_entry *right_entry = entry->right;

    // Free the memory associated with the current entry.
    free(entry);

    // Recursively free the memory for left and right child entries.
    Rec_Free_IPV4_Table(left_entry);
    Rec_Free_IPV4_Table(right_entry);
}

/**
 * @brief Free the memory associated with an IPv4 routing table.
 * 
 * Free the memory associated with an entire IPv4 routing table,
 * including its root entry and all child entries.
 * 
 * @param ip_table A pointer to a pointer to the IPv4 routing table to be freed.
 *                 After the function call, the pointer is set to NULL.
 */
void Free_IPV4_Table(ipv4_table **ip_table) {
    if (!ip_table || !(*ip_table)) return;

    // Store references to the routing table and its root entry.
    ipv4_table *ip4s = *ip_table;
    ipv4_entry *root = ip4s->root;

    // Free the memory associated with the routing table.
    free(ip4s);
    // Avoid dangling pointer access.
    *ip_table = NULL;

    // Recursively free the memory,
    // for the root entry and its children.
    Rec_Free_IPV4_Table(root);
}

/* -------------------------------------------------- FREE IPV4 TABLE ---------------------------------------------------- */
/* ------------------------------------------------- INSERT IPV4 TABLE --------------------------------------------------- */

/**
 * @brief Insert a new IPv4 routing table entry into an IPv4 routing table.
 * 
 * Insert a new IPv4 routing table entry into an existing IPv4 routing table.
 * The entry is inserted based on its prefix and mask, creating any necessary intermediate nodes
 * in the routing table tree structure.
 * 
 * @param ip_table  A pointer to the IPv4 routing table where the new entry should be inserted.
 * @param new_entry A pointer to the new routing entry to be inserted.
 */
void Insert_IPV4_Table(ipv4_table *ip_table, route *new_entry) {
    if (!ip_table || !new_entry->mask) return;

    ipv4_entry *ipv4s = ip_table->root;
    uint32_t network = new_entry->prefix & new_entry->mask;
    uint32_t network_length = __builtin_popcount(new_entry->mask);

    while (network_length) {
        // Determine the next child entry (left or right) based on the network bit.
        ipv4_entry **next_entry = (network & 1) ? &(ipv4s->right) : &(ipv4s->left);
        // Create a new entry if the next entry is NULL.
        if (!*next_entry) *next_entry = Create_IPV4_Entry();

        ipv4s = *next_entry;
        network >>= 1;
        network_length--;
    }

    // Update the attributes of the final entry if it exists.
    if (ipv4s) {
        ipv4s->type = 1;    // VALID ENTRY.
        ipv4s->next_hop = new_entry->next_hop;
        ipv4s->interface = new_entry->interface;
    }

    // Increase the size of the routing table.
    ip_table->size++;
}

/* ------------------------------------------------- INSERT IPV4 TABLE --------------------------------------------------- */
/* -------------------------------------------------  LPM IPV4 TABLE  ---------------------------------------------------- */

/**
 * @brief Perform Longest Prefix Match (LPM) in an IPv4 routing table.
 * 
 * Search an IPv4 routing table for the longest prefix match (LPM)
 * for the given destination IP address and returns the corresponding forward structure.
 * 
 * @param ip_table A pointer to the IPv4 routing table to search.
 * @param ip       The destination IP address to perform LPM on.
 * @return A pointer to the forward structure representing the LPM result,
 *         or NULL if no match is found.
 */
forward* LPM_IPV4_Table(ipv4_table *ip_table, uint32_t ip) {
    if (!ip_table || !ip_table->root) return NULL;

    forward *lpm = NULL;
    ipv4_entry *entry = ip_table->root;

    while (entry) {
        if (entry->type == 1) {
            if (!lpm) lpm = (forward*)malloc(sizeof(forward));
            if (!lpm) return NULL;
            // Update the forward structure with
            // the information from the matching entry.
            lpm->status = true;
            lpm->next_hop = entry->next_hop;
            lpm->interface = entry->interface;
        }
        // Determine the next child entry (left or right) based on the network bit.
        entry = (ip & 1) ? entry->right : entry->left;
        ip >>= 1;
    }

    // Return the Longest Prefix Match result.
    return lpm;
}

/* -------------------------------------------------  LPM IPV4 TABLE  ---------------------------------------------------- */
