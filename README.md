# Router

## Description

Router application responsible for forwarding packets within a network. The router operates on the principle of static forwarding, where routing decisions are pre-configured and do not change dynamically based on network conditions

## Packets Processing

- Network interfaces are initialized based on command-line arguments.
- The router is initialized using the provided configuration file.
- Packet Handling Loop, enters a loop where it continuously receives network messages on any network interface.
- Determine the type of the received packet and calls corresponding handler functions for IPv4 packets and for ARP packets type.
- Error handling, if there's an error when receiving a message, it frees the router and exits with an error message.

| #   | Component         | Description                                                                                                    |
|-----|:-----------------:|:---------------------------------------------------------------------------------------------------------------|
| 1.  | **Router Module**     | Encapsulates the packet forwarding logic, ARP handling, ICMP handling, and IPv4 packet processing. |
| 2.  | **Packet Structure**  | Structures for packets handled by the router, including Ethernet, IP, ARP, and ICMP headers.                     |
| 3.  | **ARP Table**         | Address Resolution Protocol (ARP) table, which maps IP addresses to MAC addresses for packet forwarding. |
| 4.  | **IPv4 Table**        | IPv4 routing table, which contains pre-configured routing entries used for packet forwarding decisions. |

### Router Forwarding

- **Initialization and Creation:**
  - During initialization, the router allocates memory for its routing table structure and initializes it.
  - The **routing table** structure typically consists of a `prefix tree` (`trie`) data structure, with each **node** representing a **routing entry**.
  - The router returns a pointer to the initialized routing table, enabling subsequent operations.

- **Insertion of Routing Entries:**

  - **Routing Entry Structure:**
    - Routing entries contain information about how to reach specific destinations in a network.
    - Each entry typically consists of a **network prefix** (**destination IP address range**), along with associated attributes (**next hop** and **interface**).

  - **Insertion Process:**
    - When a new routing entry needs to be added to a router's routing table, it is typically inserted by the admin in the `rtable.txt` file.
    - Begins with the router traversing the tree structure of its routing table to find the appropriate position for the new entry.
    - The router compares the **prefix and mask** of the new entry with existing entries in the routing table to *determine the correct position for insertion*.
    - Once the correct position is found, the new entry is added to the routing table, ensuring that it *maintains the hierarchical structure* based on **network prefixes**.
    - Below is a simplified **example** of how a new **routing entry** might be inserted into a **routing table** represented as a **prefix tree (trie)** data structure:

    ```r
    Given the routing table:
        - Prefix: 192.168.1.0/24, Next Hop: 10.0.0.1
        - Prefix: 10.0.0.0/8, Next Hop: 192.168.0.1
        - Prefix: 172.16.0.0/16, Next Hop: 192.168.0.1

    To insert a new entry with Prefix: 192.168.0.0/20, Next Hop: 10.0.0.2:
        - Traverse the routing table to find the appropriate position for the new entry.
        - Compare the prefix and mask of the new entry with existing entries.
        - Determine the correct position for insertion (e.g., between 192.168.1.0/24 and 172.16.0.0/16).
        - Add the new entry to the routing table.
    ```

#### Longest Prefix Match (LPM)

- **Algorithm Overview:**
  - `LPM` is a key algorithm used by routers to determine the **best matching route** for a *given destination IP address*.
  - When a router receives a packet, it needs to decide where to forward it based on the *destination IP address*.
  - The router traverses its routing table, which typically consists of **multiple entries** containing **IP address prefixes and corresponding next-hop** information.
  - For each entry in the routing table, the router compares the **destination IP address with the prefixes stored in the entry**.
  - The router selects the entry with the **longest matching prefix**. This means that if multiple entries match the destination IP address, the router chooses the one with the `longest prefix` (i.e., the `most specific route`).
  - The **length of the prefix** represents the **number of leading bits in the IP address** that must match for the route to be considered valid.
  - By selecting the `longest matching prefix`, the router ensures that it follows the `most specific route to the destination`, which improves routing efficiency and accuracy.

- **Packet Handling:**

  - **Handling Incoming Packets:**
    - Upon receiving a packet, the router extracts the **destination IP address** from the packet header.
    - The router performs an **LPM lookup** in its routing table to determine the **best route for forwarding** the packet.

  - **Forwarding Decisions:**
    - If a **matching route** is found, the `router forwards` the packet to the next hop according to the selected route.
    - In cases where **no matching route** is found (e.g., the `destination is unreachable or the Time-to-Live (TTL) field expires`), the router may generate `ICMP messages` such as `"Time Exceeded" or "Destination Unreachable"` to inform the sender about that the packet was `dropped` (`receiver didn't get the message`).

- **Freeing Memory:**
  - To prevent memory leaks, the router must properly deallocate memory associated with its routing table when it is no longer needed.
  - This involves recursively traversing the tree structure of the routing table and deallocating memory for each entry.

### ARP

- **Creating a New ARP Table:**
  - Allocates memory for the ARP table structure and its address entries, ensuring *dynamic memory allocation* to accommodate varying entry counts.

- **Freeing Memory Associated with ARP Table:**
  - Frees the memory allocated for an ARP table, including its address entries, to prevent *memory leaks* and ensure efficient memory management.

- **Searching for ARP Table Entry:**
  - Locates an entry in the ARP table with a **specified IP address** by *iterating through the address entries*. Returns *the `index of the entry` if found; otherwise, returns `-1`*.

- **Inserting New ARP Table Entry:**
  - Inserts a new entry containing the **provided IP address and MAC address** into the ARP table. Checks for *duplicate entries* before insertion to maintain data integrity and expands the ARP table's capacity as needed.

- **Handling Incoming ARP Packets:**
  - Upon receiving an ARP packet, the router checks its validity and type.
  - If it's an `ARP request`, the router generates a reply with its **MAC address** and sends it back.
  - If it's an `ARP reply`:
    - The **sender's IP and MAC addresses** are *cached* in the ARP table.
    - Any waiting packets **destined for the sender's IP** are processed and transmitted with the resolved **MAC address**.

|                | ARP Request                                                           |
|----------------|----------------------------------------------------------------------|
| **Step**       | **Description**                                                      |
| Initializing Ethernet Header for ARP Requests | - Sets the Ethernet type field to ARP.<br>- Determines the source MAC address based on the router's interface.<br>- Sets the destination MAC address to broadcast. |
| Initializing ARP Header for ARP Requests    | - Initializes various fields in the ARP header:<br>  - Hardware type<br>  - Protocol type<br>  - Operation (request)<br>  - Source and target hardware addresses (MACs)<br>  - Source protocol address. |
| Updating Packet Length Based on Ethernet and ARP Headers | Adjusts the length of the packet buffer in the router based on the sizes of the Ethernet and ARP headers. |
| Generating ARP Request Packet              | - Prepares an ARP request packet in the router's packet buffer.<br>- Initializes the Ethernet header for ARP requests.<br>- Initializes the ARP header for ARP requests.<br>- Updates the packet length accordingly. |

|                | ARP Reply                                                            |
|----------------|----------------------------------------------------------------------|
| **Step**       | **Description**                                                      |
| Setting ARP Operation to ARP Reply         | - Sets the ARP operation field in the ARP header to indicate an ARP reply. |
| Swapping Target and Sender IP and MAC Addresses | - Exchanges the target and sender IP and MAC addresses in the ARP header. |
| Setting Sender IP and MAC Addresses in ARP Header | - Assigns the sender's IP and MAC addresses to appropriate fields in the ARP header. |
| Updating Ethernet Header with Appropriate MAC Addresses | - Modifies the Ethernet header in the router's packet buffer to set the appropriate source and destination MAC addresses. |

### ICMP

- **Initializing ICMP Header Fields:**
  - Sets the fields in the ICMP header within the router's packet buffer.
  - Calculates pointers to the ICMP header and adjusts the total packet length accordingly.
  - Copies the IP header and additional bytes if the ICMP message type is ICMP_TIME_EXCED or ICMP_DEST_UNREACH.
  - Sets the ICMP code and type in the ICMP header.
- **Updating ICMP Checksum:**
  - Calculates and updates the ICMP checksum for the ICMP header in the router's packet buffer.
- **Generating New IPv4 Header for ICMP Messages:**
  - Prepares a new IPv4 header for ICMP messages within the router's packet buffer.
  - Serves as a wrapper for the function responsible for generating IPv4 headers.
- **Updating Ethernet Header:**
  - Modifies the Ethernet header in the router's packet buffer to include appropriate source and destination MAC addresses based on the router's interface.
- **Generating ICMP Reply Message:**
  - Creates an ICMP reply message in the router's packet buffer.
  - Initializes the ICMP header, updates the ICMP checksum, generates a new IPv4 header, and updates the Ethernet header.

## Setup

To simulate a virtual network, we will use `Mininet`. Mininet is a network simulator that uses real kernel, switch, and application code in simulation. This setup should work fine on **WSL 2**.

- Update the package index:

  ```bash
  sudo apt update
  ```

- Install required packages:

    ```bash
    sudo apt install mininet openvswitch-testcontroller tshark python3-click python3-scapy xterm python3-pip
    ```

- Install Mininet using pip:

    ```bash
    sudo pip3 install mininet
    ```

- Increase font size in terminals (**optional**):

    ```bash
    echo "xterm*font: *-fixed-*-*-*-18-*" >> ~/.Xresources
    xrdb -merge ~/.Xresources
    ```
