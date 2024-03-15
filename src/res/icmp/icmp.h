#pragma once

#ifndef ICMP_H_
#define ICMP_H_

#include "../../include/router.h"

/** @brief  Generate an ICMP reply message in the rout's packet buffer. */
extern void        Reply_ICMP        (routing *rout, uint8_t type);

#endif /* ICMP_H_ */