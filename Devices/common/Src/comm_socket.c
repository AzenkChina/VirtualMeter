#include "comm_socket.h"
#include <stdbool.h>

#if ( defined ( _WIN32 ) || defined ( _WIN64 ) ) && defined ( _MSC_VER )
#pragma comment(lib,"ws2_32.lib")
#endif

//服务端使用UDP广播接收
static SOCKET receiver_open(int Port)
{
	SOCKET sock;
#if defined ( _WIN32 ) || defined ( _WIN64 )
	WSADATA wsaData;
#endif

#if defined ( _WIN32 ) || defined ( _WIN64 )
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		return INVALID_SOCKET;
	}
#endif

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#if defined ( _WIN32 ) || defined ( _WIN64 )
    if(sock == INVALID_SOCKET) {
		WSACleanup();
		return INVALID_SOCKET;
    }
	
	int Mode = 1;
	ioctlsocket(sock, FIONBIO, (u_long FAR*) &Mode);//设置非阻塞
#elif defined ( __linux )
    if(sock <= 0) {
		return INVALID_SOCKET;
    }
	
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));//设置非阻塞
#endif

	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
#if defined ( _WIN32 ) || defined ( _WIN64 )
	sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
#elif defined ( __linux )
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
#endif
	sin.sin_port = htons(Port);
	if(bind(sock, (SOCKADDR*)&sin, sizeof(sin)) != 0) {
#if defined ( _WIN32 ) || defined ( _WIN64 )
		closesocket(sock);
		WSACleanup();
#elif defined ( __linux )
		close(sock);
#endif
		return INVALID_SOCKET;
	}

	return sock;
}

static void receiver_close(SOCKET sock)
{
    if(sock != INVALID_SOCKET) {
#if defined ( _WIN32 ) || defined ( _WIN64 )
		closesocket(sock);
		WSACleanup();
#elif defined ( __linux )
		close(sock);
#endif
    }
}

static int32_t receiver_read(SOCKET sock, uint8_t *Buffer, uint16_t BufferLength)
{
    if(sock == INVALID_SOCKET) {
        return -1;
    }

    SOCKADDR_IN Client;
    int ClientLength = sizeof(Client);
	int result = recvfrom(sock, (char *)Buffer, BufferLength, 0, (SOCKADDR*)&Client, &ClientLength);
#if defined ( _WIN32 ) || defined ( _WIN64 )
	if(result < 0)
	{
		if(WSAGetLastError() == 10035)
		{
			result = 0;
		}
	}
#endif
	return result;
}






//客户端使用UDP广播发送
static SOCKET emitter_open(int Port, SOCKADDR_IN *addr)
{
	SOCKET sock;
#if defined ( _WIN32 ) || defined ( _WIN64 )
	WSADATA wsaData;
#endif

	if (!addr)
	{
		return INVALID_SOCKET;
	}

#if defined ( _WIN32 ) || defined ( _WIN64 )
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		return INVALID_SOCKET;
	}
#endif

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == INVALID_SOCKET) {
#if defined ( _WIN32 ) || defined ( _WIN64 )
		WSACleanup();
#endif
		return INVALID_SOCKET;
	}
	
#if defined ( _WIN32 ) || defined ( _WIN64 )
    bool bOpt = true;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));//打开广播选项
#elif defined ( __linux )
    int bOpt = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &bOpt, sizeof(int));//打开广播选项
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &bOpt, sizeof(int));
#endif

	
#if defined ( _WIN32 ) || defined ( _WIN64 )
	int Mode = 1;
	ioctlsocket(sock, FIONBIO, (u_long FAR*) &Mode);//设置非阻塞
#elif defined ( __linux )
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));//设置非阻塞
#endif

    //设置发往的地址
    memset(addr, 0, sizeof(SOCKADDR_IN));
	addr->sin_family = AF_INET;
#if defined ( _WIN32 ) || defined ( _WIN64 )
	addr->sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
#elif defined ( __linux )
	addr->sin_addr.s_addr = htonl(INADDR_BROADCAST);
#endif
	addr->sin_port = htons(Port);

	return sock;
}

//客户端使用UDP单播发送
SOCKET emitter_open_unicast(int Port, SOCKADDR_IN *addr, char *IPaddr)
{
	SOCKET sock;
#if defined ( _WIN32 ) || defined ( _WIN64 )
	WSADATA wsaData;
#endif

	if (!addr)
	{
		return INVALID_SOCKET;
	}

#if defined ( _WIN32 ) || defined ( _WIN64 )
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR) {
		return INVALID_SOCKET;
	}
#endif

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == INVALID_SOCKET) {
#if defined ( _WIN32 ) || defined ( _WIN64 )
		WSACleanup();
#endif
		return INVALID_SOCKET;
	}

#if defined ( _WIN32 ) || defined ( _WIN64 )
	int Mode = 1;
	ioctlsocket(sock, FIONBIO, (u_long FAR*) &Mode);//设置非阻塞
#elif defined ( __linux )
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));//设置非阻塞
#endif

    //设置发往的地址
    memset(addr, 0, sizeof(SOCKADDR_IN));
	addr->sin_family = AF_INET;
#if defined ( _WIN32 ) || defined ( _WIN64 )
	addr->sin_addr.S_un.S_addr = htonl(inet_addr(IPaddr));
#elif defined ( __linux )
	addr->sin_addr.s_addr = htonl(inet_addr(IPaddr));
#endif
	addr->sin_port = htons(Port);

	return sock;
}

static void emitter_close(SOCKET sock)
{
    if(sock != INVALID_SOCKET) {
#if defined ( _WIN32 ) || defined ( _WIN64 )
		WSACleanup();
		closesocket(sock);
#elif defined ( __linux )
		close(sock);
#endif
    }
}

static int32_t emitter_write(SOCKET sock, const SOCKADDR_IN *addr, uint8_t *Buffer, uint16_t Length)
{
    if(sock == INVALID_SOCKET) {
        return -1;
    }

	return sendto(sock, (char *)Buffer, Length, 0, (SOCKADDR*)addr, sizeof(SOCKADDR_IN));
}


const struct __receiver receiver = 
{
	.open			= receiver_open,
	.close			= receiver_close,
	.read			= receiver_read,
};

const struct __emitter emitter = 
{
	.open			= emitter_open,
	.open_unicast	= emitter_open_unicast,
	.close			= emitter_close,
	.write			= emitter_write,
};
