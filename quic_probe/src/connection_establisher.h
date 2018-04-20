/*
 * connection_establisher.h
 *
 *  Created on: Apr 18, 2018
 *      Author: sergei
 */

#ifndef SRC_CONNECTION_ESTABLISHER_H_
#define SRC_CONNECTION_ESTABLISHER_H_

#include "engine_structs.h"

void quic_connect(quic_engine* quic_engine_ref, const char* url, int is_ipv4);
void destroy(quic_engine* quic_engine_ref);

#endif /* SRC_CONNECTION_ESTABLISHER_H_ */
