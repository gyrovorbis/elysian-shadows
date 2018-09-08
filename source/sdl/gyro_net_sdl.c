#include <libGyro/gyro_net_api.h>
#include <libGyro/gyro_system_api.h>
#include <sdl/gyro_sdl.inl>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#if GYRO_PLATFORM == GYRO_PLATFORM_RASPI
    #include <SDL2/SDL_net.h>
#else

#ifdef __WIN32 // :D Mmmmmm
#   include <windows.h>
#   include <SDL/SDL_net.h>
#endif

#ifdef __linux__ // :) Sweet!
#   include <SDL2/SDL_net.h>
#endif

#ifdef __APPLE__ // D: Fuck my life...
#include <SDL_net.h>
#endif
#endif



typedef struct TcpSocketServer {
    TCPsocket           serverSocket;
    IPaddress           serverIP;
} TcpSocketServer;

typedef struct TcpSocket {
    TCPsocket socket;
} TcpSocket;


static SDLNet_SocketSet _socketSet;
static char             _ipAddrStrBuff[16];

static inline const char* _ipAddr32BitToString(uint32_t addr) {
    const char* byte = (char*)&addr;
    sprintf(_ipAddrStrBuff, "%u.%u.%u.%u", byte[3], byte[2], byte[1], byte[0]);
    return _ipAddrStrBuff;
}

const char* gyNetIpAddressToString(const GYIpAddress* addr) {
    return _ipAddr32BitToString(addr->host);
}

int gyNetPeerIpAddress(GYIpAddress* addr, const GYTcpSocket* socket) {
    IPaddress* sdlAddr = SDLNet_TCP_GetPeerAddress(((TcpSocket*)socket)->socket);
    if(sdlAddr) {
        memcpy(addr, sdlAddr, sizeof(GYIpAddress));
        return 1;
    } else {
        _gyLog(GY_DEBUG_ERROR, "Unable to get peer IP address for socket!");
        return 0;
    }
}

int gyNetInit(const GYNetInitParams* params) {
    int success = 1;

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Initializing SDL_Net.");
    _gyPush();

    if(SDLNet_Init() < 0) {
        _gyLog(GY_DEBUG_ERROR, "SDL_Net Error: %s.", SDLNet_GetError());
        success = 0;
    }

    SDL_version compile_version;
    const SDL_version *link_version = SDLNet_Linked_Version();
    SDL_NET_VERSION(&compile_version);
    _gyLog(GY_DEBUG_VERBOSE, "SDL2_Net Compile-Time Version: %d.%d.%d",
            compile_version.major,
            compile_version.minor,
            compile_version.patch);
    _gyLog(GY_DEBUG_VERBOSE, "SDL2_Net Run-Time Version: %d.%d.%d",
            link_version->major,
            link_version->minor,
            link_version->patch);

    _socketSet = SDLNet_AllocSocketSet(params->maxMonitoredConnections);
    if(!_socketSet) {
        _gyLog(GY_DEBUG_ERROR, "Could not allocate socket set of size %d: %s!",
               params->maxMonitoredConnections, SDLNet_GetError());
        success = 0;
    }

    _gyPop(1);
    return success;
}

int gyNetUninit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Uninitializing SDL_Net.");
    SDLNet_FreeSocketSet(_socketSet);
    SDLNet_Quit();
    return 1;
}

void* gyNetTcpSocketServerCreate(unsigned int port) {
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Creating Socket Server [Port: %d].", port);
    _gyPush();

    TcpSocketServer* srv = malloc(sizeof(TcpSocketServer));

    if(SDLNet_ResolveHost(&srv->serverIP, NULL, port) == 1) {
        _gyLog(GY_DEBUG_ERROR, "Could not resolve host [%s]!", SDLNet_GetError());
        goto freesrv;
    } else {

        srv->serverSocket = SDLNet_TCP_Open(&srv->serverIP);
        if(!srv->serverSocket) {
            _gyLog(GY_DEBUG_ERROR, "Could not open server socket [%s]!", SDLNet_GetError());
            goto freesrv;

        } else {

            if(SDLNet_TCP_AddSocket(_socketSet, srv->serverSocket) == -1) {
                _gyLog(GY_DEBUG_ERROR, "Could not add server's socket to socket set: [%s]!", SDLNet_GetError());
                goto closesocket;
            } else {
                _gyLog(GY_DEBUG_VERBOSE, "Listening for incoming connections [%s:%d].",
                    _ipAddr32BitToString(srv->serverIP.host), port);
                goto done;
            }
        }
    }

closesocket:
    SDLNet_TCP_Close(srv->serverSocket);

freesrv:
    free(srv);
    srv = NULL;

done:
    _gyPop(1);
    return srv;
}

int gyNetTcpSocketServerDestroy(void* server) {
    TcpSocketServer* srv = server;
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Destroying Socket Server [%s]", _ipAddr32BitToString(srv->serverIP.host), srv->serverIP.port);
    SDLNet_TCP_DelSocket(_socketSet, srv->serverSocket);
    SDLNet_TCP_Close(srv->serverSocket);
    free(srv);
    return 1;
}

int gyNetMonitoredConnectionsPoll(uint32_t timeout) {
    int waitingSockets = SDLNet_CheckSockets(_socketSet, timeout);
    if(waitingSockets == -1) { //error
        _gyLog(GY_DEBUG_ERROR, "GYRO_NET_API: SDLNet_CheckSockets failed! SDL_Error [%s], strerror[%s].",
               SDLNet_GetError(), strerror(errno));
    }
    return waitingSockets;
}

int gyNetMonitoredConnectionsAdd(void* sock) {
    TcpSocket* socket = sock;
    if(SDLNet_TCP_AddSocket(_socketSet, socket->socket) == -1) {
        _gyLog(GY_DEBUG_ERROR, "GYRO_NET_API: Could not add socket to monitored socket set: [%s]!", SDLNet_GetError());
        return 0;
    }
    return 1;
}

int gyNetMonitoredConnectionsRemove(void* sock) {
    TcpSocket* socket = sock;
    SDLNet_TCP_DelSocket(_socketSet, socket->socket);
    return 1;
}

int gyNetTcpSocketReady(const void* sock) {
    const TcpSocket* socket = sock;
    return SDLNet_SocketReady(socket->socket);
}

void* gyNetTcpSocketOpen(const char* host, unsigned port) {
    IPaddress ip;
    TcpSocket* socket = NULL;

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Opening TCP socket [host: %s, port: %d].", host, port);
    _gyPush();

    if(SDLNet_ResolveHost(&ip, host, port) == -1) {
        _gyLog(GY_DEBUG_ERROR, "Could not resolve host: %s", SDLNet_GetError());

    } else {
        TCPsocket sdlSock = SDLNet_TCP_Open(&ip);
        if(!sdlSock) {
            _gyLog(GY_DEBUG_ERROR, "Could not open socket: %s", SDLNet_GetError());
        } else {
            socket = malloc(sizeof(TcpSocket));
            socket->socket = sdlSock;
        }
    }

    _gyPop(1);
    return socket;
}

int gyNetTcpSocketClose(void* sock) {
    TcpSocket* socket = sock;
    IPaddress* remoteIP = SDLNet_TCP_GetPeerAddress(socket->socket);
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_NET_API: Closing TCP socket [host: %s, port: %d].",
           remoteIP? _ipAddr32BitToString(remoteIP->host) : "?", remoteIP? remoteIP->port : -1);
    SDLNet_TCP_Close(socket->socket);
    free(socket);
    return 1;
}

int gyNetTcpSocketSend(const void* socket, const void* data, size_t bytes) {
    return SDLNet_TCP_Send(((TcpSocket*)socket)->socket, data, bytes);
}

int gyNetTcpSocketRecv(const void* socket, void* data, size_t maxBytes) {
    return SDLNet_TCP_Recv(((TcpSocket*)socket)->socket, data, maxBytes);
}

void* gyNetTcpSocketServerCheckIncomingConnections(void* server) {
    TcpSocketServer* srv = server;
    TcpSocket* newSocket = NULL;

    //Check if any sockets are trying to connect
    if(SDLNet_SocketReady(srv->serverSocket)) {
        _gyLog(GY_DEBUG_VERBOSE, "GY_NET_API: Processing incoming socket connection [port: %d].", srv->serverIP.port);
        _gyPush();

        //Accept the connection and store socket
        TCPsocket sdlSocket = SDLNet_TCP_Accept(srv->serverSocket);
        //Attempt to log remote host details
        IPaddress* remoteIp = SDLNet_TCP_GetPeerAddress(sdlSocket);
        _gyLog(GY_DEBUG_VERBOSE, "Connection Accepted [%s:%d].",
               remoteIp? _ipAddr32BitToString(remoteIp->host) : "?",
               remoteIp? remoteIp->port : -1);

        newSocket = malloc(sizeof(TcpSocket));
        newSocket->socket = sdlSocket;

        _gyPop(1);
    }

    return newSocket;
}

const char* gyNetResolveIp(GYIpAddress* addr) {
    return SDLNet_ResolveIP((IPaddress*)addr);
}

int gyNetResolveHost(GYIpAddress* addr, const char *host, uint16_t port) {
    return SDLNet_ResolveHost((IPaddress*)addr, host, port);
}


