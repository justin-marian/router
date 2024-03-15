#include "./lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <linux/if.h>
#include <linux/if_packet.h>

#include <asm/byteorder.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>

int interfaces[ROUTER_NUM_INTERFACES];

/*********************************************************************************/

// Function to obtain a socket for a specified network interface.
// Takes the interface name as input.
// Returns the socket descriptor.
static int Get_Socket(const char *if_name) {    
    // Create a raw socket for packet communication
    int s = socket(AF_PACKET, SOCK_RAW, 768);
    DIE(s == -1, "socket %s", strerror(errno)); // Check if socket creation failed
    
    // Prepare a structure to request the interface index
    struct ifreq intf;
    strcpy(intf.ifr_name, if_name);
    
    // Get the interface index using ioctl
    int res = ioctl(s, SIOCGIFINDEX, &intf);
    DIE(res, "ioctl SIOCGIFINDEX %s", strerror(errno)); // Check if ioctl call failed
    
    // Prepare a sockaddr_ll structure for binding
    struct sockaddr_ll addr;
    memset(&addr, 0x00, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = intf.ifr_ifindex;
    
    // Bind the socket to the specified network interface
    res = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    DIE(res == -1, "bind %s", strerror(errno)); // Check if binding failed
    
    return s; // Return the socket descriptor
}

// Initialize network interfaces based on command line arguments.
// This function takes the number of arguments (argc) and an array of interface names (argv).
// It sets up sockets for each specified network interface.
void Init_Network(int argc, char *argv[]) {
    for (int byte = 0; byte < argc; ++byte) {
        printf("Setting up interface: %s\n", argv[byte]);
        interfaces[byte] = Get_Socket(argv[byte]); // Create a socket for the specified interface.
    }
}

// Receive a network packet from the specified socket.
// This function takes a socket descriptor (sockfd), a buffer (frame_data) to store the received data,
// and a pointer (len) to store the length of the received data.
// Note: The "frame_data" buffer should be large enough to accommodate the maximum transmission unit 
// (MTU) of the interface, e.g., 1500 bytes.
// Returns 0 on success, or an error code on failure.
int Recv_Socket_Msg(int sockfd, char *frame_data, size_t *len) {
    int ret = read(sockfd, frame_data, MAX_PACKET_LEN); // Read data from the socket.
    DIE(ret < 0, "read %s", strerror(errno)); // Check for read errors.
    *len = ret; // Store the length of the received data.
    return 0;
}

/*********************************************************************************/

// Send a network message to a specific network interface.
// This function takes the interface index (intidx), a pointer to frame data (frame_data),
// and the length of the data (len) as inputs.
// Returns the number of bytes sent on success or an error code on failure.
int Send_To_Link(int intidx, char *frame_data, size_t len) {
	int ret = write(interfaces[intidx], frame_data, len);
	DIE(ret == -1, "write %s", strerror(errno));
	return ret;
}

// Receive a network message from any available network interface using non-blocking I/O.
// This function takes a pointer to frame data (frame_data) and a pointer to store the received data length (length).
// Returns the interface index where data was received on success, or -1 on failure.
ssize_t Recv_From_Link(int intidx, char *frame_data) {
	ssize_t ret = read(interfaces[intidx], frame_data, MAX_PACKET_LEN);
	return ret;
}

// Receive a network message from any available network interface using non-blocking I/O.
// This function takes a pointer to frame data (frame_data) and a pointer to store the received data length (length).
// Returns the interface index where data was received on success, or -1 on failure.
int Recv_FromAny_Link(char *frame_data, size_t *length) {
	fd_set set;
	FD_ZERO(&set);

	while (1) {
		for (int byte = 0; byte < ROUTER_NUM_INTERFACES; byte++) {
			FD_SET(interfaces[byte], &set);
		}

		int res = select(interfaces[ROUTER_NUM_INTERFACES - 1] + 1, &set, NULL, NULL, NULL);
		DIE(res == -1, "select %s", strerror(errno));

		for (int byte = 0; byte < ROUTER_NUM_INTERFACES; byte++) {
			if (FD_ISSET(interfaces[byte], &set)) {
				ssize_t ret = Recv_From_Link(byte, frame_data);
				DIE(ret < 0, "Recv_From_Link %s", strerror(errno));
				*length = ret;
				return byte;
			}
		}
	}

	return -1;
}

/*********************************************************************************/

// Get the IP address as a string for a given network interface.
// This function takes the interface index (interface) as input.
// Returns a string representing the IP address.
char *Get_IP_Interface(int interface) {
	struct ifreq ifr;

	if (interface == 0)
		sprintf(ifr.ifr_name, "rr-0-1");
	else {
		sprintf(ifr.ifr_name, "r-%u", interface - 1);
	}

	int ret = ioctl(interfaces[interface], SIOCGIFADDR, &ifr);
	DIE(ret == -1, "ioctl SIOCGIFADDR %s", strerror(errno));
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

// Get the IPv4 address as an integer for a given network interface.
// This function takes the interface index (interface) as input.
// Returns the IPv4 address as an integer.
uint32_t Get_IPV4_Interface(int interface) {
	struct ifreq ifr;

	if (interface == 0)
		sprintf(ifr.ifr_name, "rr-0-1");
	else {
		sprintf(ifr.ifr_name, "r-%u", interface - 1);
	}

	int ret = ioctl(interfaces[interface], SIOCGIFADDR, &ifr);
	DIE(ret == -1, "ioctl SIOCGIFADDR %s", strerror(errno));
	return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}

// Get the MAC address for a given network interface.
// This function takes the interface index (interface) and a pointer
// to store the MAC address (mac) as input.
void Get_MAC_Interface(int interface, uint8_t *mac) {
	struct ifreq ifr;

	if (interface == 0)
		sprintf(ifr.ifr_name, "rr-0-1");
	else {
		sprintf(ifr.ifr_name, "r-%u", interface - 1);
	}

	int ret = ioctl(interfaces[interface], SIOCGIFHWADDR, &ifr);
	DIE(ret == -1, "ioctl SIOCGIFHWADDR %s", strerror(errno));
	memcpy(mac, ifr.ifr_addr.sa_data, 6);
}

/*********************************************************************************/

// Convert a hexadecimal character to its numeric value.
// This function takes a character (c) as input.
// Returns the numeric value of the hexadecimal character or -1 for invalid input.
int hex2num(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

// Convert a two-character hexadecimal string to a byte.
// This function takes a pointer to a two-character hexadecimal string (hex) as input.
// Returns the byte value or -1 for invalid input.
int hex2byte(const char *hex) {
	int a = hex2num(*hex++);
	if (a < 0) return -1;
	int b = hex2num(*hex++);
	if (b < 0) return -1;
	return (a << 4) | b;
}

// Convert a hardware address represented as a hexadecimal string to a byte array.
// This function takes a pointer to a null-terminated hexadecimal string (txt) and a pointer to store the resulting address (addr) as input.
// Returns 0 on success or -1 on failure.
int HW_MAC_Addr(const char *txt, uint8_t *addr) {
	for (int byte = 0; byte < 6; byte++) {
		int a, b;
		a = hex2num(*txt++);
		if (a < 0)
			return -1;
		b = hex2num(*txt++);
		if (b < 0)
			return -1;
		*addr++ = (a << 4) | b;
		if (byte < 5 && *txt++ != ':')
			return -1;
	}
	return 0;
}

/*********************************************************************************/

// Calculate the Internet Checksum for a sequence of 16-bit data values.
// This function takes a pointer to an array of 16-bit data values (data) and the 
// length of the data in bytes (len) as input.
// Returns the calculated checksum value as a 16-bit integer.
uint16_t Checksum(uint16_t *data, size_t len) {
    unsigned long checksum = 0;
    uint16_t extra_byte;

    // Process 16-bit data values in the array.
    while (len > 1) {
        checksum += ntohs(*data++);
        len -= 2;
    }

    // If there is an odd byte left, process it.
    if (len) {
        *(uint8_t *)&extra_byte = *(uint8_t *)data; // Extract the remaining byte.
        checksum += extra_byte; // Add the byte to the checksum.
    }

    // Add any carry bits and perform one's complement 
	// to obtain the final checksum.
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);

    return (uint16_t)(~checksum); // Complement checksum.
}


/*********************************************************************************/

