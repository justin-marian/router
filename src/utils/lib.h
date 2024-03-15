#ifndef LIB_H_
#define LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_PACKET_LEN          1600
#define ROUTER_NUM_INTERFACES   3

// Initialize network interfaces and the router based on command line arguments.
void Init_Network(int argc, char *argv[]);
// Send a network message to a specific network interface.
int Send_To_Link(int interface, char *frame_data, size_t length);
// Receive a network message from any available network interface.
int Recv_FromAny_Link(char *frame_data, size_t *length);

// Get the IP address as a string for a given network interface.
char *Get_IP_Interface(int interface);
// Get the IPv4 address as an integer for a given network interface.
uint32_t Get_IPV4_Interface(int interface);

// Get the MAC address for a given network interface.
void Get_MAC_Interface(int interface, uint8_t *mac);
// Convert a hardware address represented as a hexadecimal string to a byte array.
int HW_MAC_Addr(const char *txt, uint8_t *addr);

// Calculate the Internet Checksum for a sequence of 16-bit data values.
uint16_t Checksum(uint16_t *data, size_t len);

// Custom error handling macro.
#define DIE(condition, message, ...) \
    do { \
        if ((condition)) { \
            fprintf(stderr, "[(%s:%d)]: " # message "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
            perror(""); \
            exit(1); \
        } \
    } while (0)

#endif /* LIB_H_ */
