#pragma once

#ifndef IPV4_H_
#define IPV4_H_

#include "../../include/router.h"

#define     IPV4_VERSION    	4
#define     IPV4_IHL        	5
#define     DEFAULT_TTL     	64

#define 	ICMP_RESPONE 		(uint8_t)0
#define 	ICMP_TIME_EXCED 	(uint8_t)11
#define 	ICMP_DEST_UNREACH 	(uint8_t)3

/** @brief Create the IPv4 header for ICMP packets and update checksum. */
extern void        Header_IPV4       (routing *route, uint8_t type);
/** @brief  Handle incoming IPv4 packets. */
extern void        Handler_IPV4      (routing *route);

#endif /* IPV4_H_ */