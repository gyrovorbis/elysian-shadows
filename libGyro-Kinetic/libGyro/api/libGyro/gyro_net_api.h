/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_NET_API_H
#define GYRO_NET_API_H

#include <stdint.h>
#include <stddef.h>
#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

struct          GYNetInitParams;

typedef void    GYTcpSocket;
typedef void    GYTcpServer;

typedef struct  GYIpAddress {
    uint32_t host;            /* 32-bit IPv4 host address */
    uint16_t port;            /* 16-bit protocol port */
} GYIpAddress;

int             gyNetInit(const struct GYNetInitParams* params);
int             gyNetUninit(void);

int             gyNetMonitoredConnectionsPoll(uint32_t timeout);
int             gyNetMonitoredConnectionsAdd(void* sock);
int             gyNetMonitoredConnectionsRemove(void* sock);

int             gyNetTcpSocketReady(const GYTcpSocket* sock);
GYTcpSocket*    gyNetTcpSocketOpen(const char* host, unsigned port);
int             gyNetTcpSocketClose(GYTcpSocket* sock);
int             gyNetTcpSocketSend(const GYTcpSocket* socket, const void* data, size_t bytes);
int             gyNetTcpSocketRecv(const GYTcpSocket* socket, void* data, size_t maxBytes);

GYTcpServer*    gyNetTcpSocketServerCreate(unsigned int port);
int             gyNetTcpSocketServerDestroy(GYTcpServer* server);
GYTcpServer*    gyNetTcpSocketServerCheckIncomingConnections(GYTcpServer* server);

const char*     gyNetResolveIp(GYIpAddress* addr);
int             gyNetResolveHost(GYIpAddress* addr, const char *host, uint16_t port);
const char*     gyNetIpAddressToString(const GYIpAddress* addr);
int             gyNetPeerIpAddress(GYIpAddress* addr, const GYTcpSocket* socket);



//poll network status
//check if socket is ready (after poll)
//add socket to monitor
//remove socket from monitor
//server does not own child connections

#ifdef GYRO_NET_PLATFORM_INCLUDE
#	include GYRO_NET_PLATFORM_INCLUDE
#endif


#ifdef __cplusplus
}
#endif

#endif // GYRO_NET_H

