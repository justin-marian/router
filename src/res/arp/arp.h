#pragma once

#ifndef ARP_H_
#define ARP_H_

#include "../../include/router.h"

#define 	OP_REQUEST 			htons(1)
#define 	OP_REPLY 			htons(2)

#define 	HTYPE_ETHER 		htons(1)
#define 	IP_TYPE 			htons(0x0800)
#define 	ARP_TYPE 			htons(0x0806)

#define 	HW_LEN 				(uint8_t)6
#define 	PT_LEN 				(uint8_t)4

/** @brief  Reply to an ARP request by filling in the
 * ARP header and Ethernet header
 * with appropriate values for an ARP reply. */
extern void        Request_ARP         (routing *rout);
/** @brief Generate an ARP request packet in the rout structure. */
extern void        Reply_ARP           (routing *rout);
/** @brief Handle incoming ARP packets in the rout. */
extern void        Handler_ARP         (routing *rout);

#endif /* ARP_H_ */