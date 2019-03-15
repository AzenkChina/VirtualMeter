#ifndef _COMM_SOCKET_H_
#define _COMM_SOCKET_H_

#include <stdint.h>

#if defined ( _WIN32 ) || defined ( _WIN64 )

#include <Ws2tcpip.h>

#elif defined ( __linux )

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>

#define SOCKET			int
#define SOCKADDR		struct sockaddr
#define SOCKADDR_IN		struct sockaddr_in
#define INVALID_SOCKET	0

#else

#error platform unsupported

#endif




#ifdef __cplusplus
extern "C" {
#endif

struct __receiver
{
	SOCKET			(*open)(int);
	void			(*close)(SOCKET);
	int32_t			(*read)(SOCKET, uint8_t *, uint16_t);
};

struct __emitter
{
	SOCKET			(*open)(int, SOCKADDR_IN *);
	SOCKET			(*open_unicast)(int, SOCKADDR_IN *, char *);
	void			(*close)(SOCKET);
	int32_t			(*write)(SOCKET, const SOCKADDR_IN *, uint8_t *, uint16_t);
};


extern const struct __receiver receiver;
extern const struct __emitter emitter;

#ifdef __cplusplus
}
#endif



#endif

