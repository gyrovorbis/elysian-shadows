/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_net_api.h>
#include <libGyro/gyro_system_api.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

static inline const char* _ipAddr32BitToString(uint32_t addr) {
    return "localhost";
}

const char* gyNetIpAddressToString(const GYIpAddress* addr) {
    return _ipAddr32BitToString(addr->host);
}

int gyNetPeerIpAddress(GYIpAddress* addr, const GYTcpSocket* socket) {
    _gyLog(GY_DEBUG_ERROR, "Unable to get peer IP address for socket!");
    return 0;
}

int gyNetInit(const GYNetInitParams* params) {
    return 0;
}

int gyNetUninit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Uninitializing SDL_Net.");
    return 1;
}

void* gyNetTcpSocketServerCreate(unsigned int port) {
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Creating Socket Server [Port: %d].", port);
    _gyPush();
    return nullptr;
}

int gyNetTcpSocketServerDestroy(void* server) {
    // TcpSocketServer* srv = server;
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Destroying Socket Server");
    // free(srv);
    return 1;
}

int gyNetMonitoredConnectionsPoll(uint32_t timeout) {
    return 0;
}

int gyNetMonitoredConnectionsAdd(void* sock) {
    return 0;
}

int gyNetMonitoredConnectionsRemove(void* sock) {
    return 1;
}

int gyNetTcpSocketReady(const void* sock) {
    return 0;
}

void* gyNetTcpSocketOpen(const char* host, unsigned port) {
    return NULL;
}

int gyNetTcpSocketClose(void* sock) {
    return 1;
}

int gyNetTcpSocketSend(const void* socket, const void* data, size_t bytes) {
    return 0;
}

int gyNetTcpSocketRecv(const void* socket, void* data, size_t maxBytes) {
    return 0;
}

void* gyNetTcpSocketServerCheckIncomingConnections(void* server) {
    return 0;
}

const char* gyNetResolveIp(GYIpAddress* addr) {
  return "localhost";
}

int gyNetResolveHost(GYIpAddress* addr, const char *host, uint16_t port) {
    return 0;
}
