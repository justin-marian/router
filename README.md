# Router

## Description

Router application responsible for forwarding packets within a network. The router operates on the principle of static forwarding, where routing decisions are pre-configured and do not change dynamically based on network conditions

## Packets Processing

- Network interfaces are initialized based on command-line arguments.
- The router is initialized using the provided configuration file.
- Packet handling, enters a loop where it continuously receives network messages on any network interface.
- Call corresponding handler function for IPv4 packets and for ARP packets type.
- Error handling, if there's an error when receiving a message, it frees the router and exits with an error message.

## Router Forwarding

The router navigates the routing table's `prefix tree` (`trie`) structure to find the insertion point.
It compares the **prefix and mask** of the new entry with existing ones to determine the insertion position.
After finding the correct position, the new entry is added while **preserving the hierarchical structure**.

**Packet Handling:**

- **Handling Incoming Packets:**
  - Extracts the **destination IP address** from the packet header upon receiving a packet.
  - Performs an **LPM lookup** in the routing table to determine the **best route for forwarding**.

- **Forwarding Decisions:**
  - If a **matching route** is found, the router forwards the packet to the next hop.
  - When **no matching route** is found ICMP messages are send, the packet was dropped because of:
    - `"Time Exceeded"`
    - `"Destination Unreachable"`

- **Initialization and Creation:**
  - During initialization, the router allocates memory for its routing table structure and initializes it.
  - The **routing table** structure is a `prefix tree` (`trie`), with each **node** representing a (**routing entry**).

- **Routing Entry Structure:**
  - Routing entries contain information about how to reach specific destinations in a network.
  - Consists of a **network prefix** (**destination IP address range**), and with its attributes (**next hop** and **interface**).

- **Insertion Process:**
  - When adding a new routing entry, the admin typically updates the `rtable.txt` file.
  - The router navigates the routing table's tree structure to find the insertion point.
  - After finding the correct position, the new entry is added while preserving the hierarchical structure.
  - Once the correct position is found, the new entry is added to the routing table, ensuring that it *maintains the hierarchical structure* based on **network prefixes**.
  - Simple example illustrates how a new entry is inserted into the routing table represented as a `trie`.

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

## Longest Prefix Match (LPM)

**Algorithm Overview:**

`LPM` (Longest Prefix Match) is used by the router to determine the **best matching route** for a given **destination IP address**.

- When a router receives a packet, it needs to decide where to forward it based on the destination IP address.
- The router looks through its routing table, with multiple entries with IP address prefixes and next-hop information.
- For each entry in the routing table, the router compares the destination IP address with the stored prefixes.
- The entry with the **longest matching prefix** is select, meaning the one that matches the most bits of the destination IP address.
- Router follows the **most specific route** to the destination, the router chooses the one with the `longest prefix` (`most specific route`), improving routing efficiency and accuracy.

## ARP

- **Searching for ARP Table Entry:**
  - Iterates through address entries to find an entry with a specified IP.
  - Returns *the entry's index if found; otherwise, returns -1*.
- **Inserting New ARP Table Entry:**
  - Inserts a new entry with the provided IP and MAC address. Checks for duplicates and expands table capacity as needed.
- **Handling Incoming ARP Packets:**
  - Upon receiving an ARP packet, the router checks its validity and type.
    - ARP requests, replies with router's MAC address.
    - ARP replies, caches sender's IP and MAC addresses or processes waiting packets for the sender's IP with resolved MAC.

### ARP Request

- **Initialize Ethernet Header:**
  - Sets the Ethernet type to ARP, determines source MAC address, and sets destination broadcast MAC.
- **Update Packet Length:**
  - Adjusts the packet buffer length based on Ethernet and ARP header sizes.
- **Generate ARP Request Packet:**
  - Prepares an ARP request packet, initializes Ethernet header, and updates packet length.

### ARP Reply

- **Set ARP Operation to Reply:**
  - Indicates an ARP reply by setting the ARP operation field.
- **Swap Target and Sender IP/MAC:**
  - Exchanges target and sender IP/MAC addresses in the ARP header.
- **Set Sender IP and MAC:**
  - Assigns sender's IP and MAC addresses in the ARP header.
- **Update Ethernet Header:**
  - Modifies Ethernet header to set appropriate MAC addresses in the packet buffer.

## ICMP

- **Initialize ICMP Header:**
  - Sets ICMP header fields in the packet buffer, calculates pointers, and adjusts packet length.
  - Copies IP header and additional bytes for specific ICMP message types.
- **Update ICMP Checksum:**
  - Calculates and updates the ICMP checksum in the packet buffer.
- **Generate New IPv4 Header:**
  - Prepares a new IPv4 header for ICMP messages.
- **Update Ethernet Header:**
  - Modifies Ethernet header to include appropriate MAC addresses based on the router's interface.
- **Generate ICMP Reply:**
  - ICMP reply message: *initializes ICMP header, updates checksum, Ethernet header, and generates IPv4 header*.

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
