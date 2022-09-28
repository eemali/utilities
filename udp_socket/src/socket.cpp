#include "../../get_time_of_day/include/gettimeofday.h"
#include "../include/socket.h"
#include <iostream>
namespace Network
{
#ifdef _WIN32
	static bool isWinSockInitialized = false;
#endif // _WIN32
	// SocketException Code
	SocketException::SocketException(const std::string &message, bool inclSysMsg)
		throw() : userMessage(message) {
		if (inclSysMsg) {
			userMessage.append(": ");
			userMessage.append(strerror(errno));
		}
	}

	SocketException::~SocketException() throw() {
	}

	const char *SocketException::what() const throw() {
		return userMessage.c_str();
	}

	// Function to fill in address structure given an address and port
	static void fillAddr(const std::string &address, unsigned short port,
		sockaddr_in &addr) {
		memset(&addr, 0, sizeof(addr));  // Zero out address structure
		addr.sin_family = AF_INET;       // Internet address

		hostent *host = gethostbyname(address.c_str());  // Resolve name
		if (NULL == host) {
			// strerror() will not work for gethostbyname() and hstrerror() 
			// is supposedly obsolete
			throw SocketException("Failed to resolve name (gethostbyname())");
		}
		addr.sin_addr.s_addr = *((unsigned long *)host->h_addr_list[0]);

		addr.sin_port = htons(port);     // Assign port in network byte order
	}

	Socket::Socket() : sockDesc(-1) {}

	// Socket Code
	Socket::Socket(int type, int protocol) {
#ifdef _WIN32
		if (!isWinSockInitialized) {
			WORD wVersionRequested;
			WSADATA wsaData;

			wVersionRequested = MAKEWORD(2, 0);              // Request WinSock v2.0
			if (WSAStartup(wVersionRequested, &wsaData) != 0) {  // Load WinSock DLL
				throw SocketException("Unable to load WinSock DLL");
			}
			isWinSockInitialized = true;
		}
#endif // _WIN32
		// Make a new socket
		// AF_INET             IPv4 Internet protocols
		sockDesc = socket(AF_INET, type, protocol);		
	}

	Socket::Socket(int sockDesc) {
		this->sockDesc = sockDesc;
	}

	Socket::~Socket() {
#ifdef _WIN32
		::closesocket(sockDesc);
#else
		::close(sockDesc);
#endif // _WIN32
		sockDesc = -1;
	}

	std::string Socket::getLocalAddress() throw(SocketException) {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);

		if (getsockname(sockDesc, (sockaddr *)&addr, (socklen_t *)&addr_len) < 0) {
			throw SocketException("Fetch of local address failed (getsockname())", true);
		}
		return inet_ntoa(addr.sin_addr);
	}

	unsigned short Socket::getLocalPort() throw(SocketException) {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);

		if (getsockname(sockDesc, (sockaddr *)&addr, (socklen_t *)&addr_len) < 0) {
			throw SocketException("Fetch of local port failed (getsockname())", true);
		}
		return ntohs(addr.sin_port);
	}

	void Socket::bindsock(unsigned short localPort) throw(SocketException) {
		// Bind the socket to its port
		sockaddr_in localAddr;
		memset(&localAddr, 0, sizeof(localAddr));
		localAddr.sin_family = AF_INET;
		localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		localAddr.sin_port = htons(localPort);
		if (bind(sockDesc, (sockaddr *)&localAddr, sizeof(sockaddr_in)) < 0) {
			throw SocketException("Set of local port failed (bind())", true);
		}
	}

	void Socket::bindsock(const std::string &localAddress,
		unsigned short localPort) throw(SocketException) {
		// Get the address of the requested host
		sockaddr_in localAddr;
		try {
			fillAddr(localAddress, localPort, localAddr);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
		if (bind(sockDesc, (sockaddr *)&localAddr, sizeof(sockaddr_in)) < 0) {
			throw SocketException("Set of local address and port failed (bind())", true);
		}
	}

	void Socket::setsockoption(int level, int optname,
		raw_type* optval, socklen_t optlen) throw(SocketException){
		if (setsockopt(sockDesc, level, optname, optval, optlen) < 0){
            throw SocketException("(setsockopt())", true);
		}
	}

	void Socket::cleanUp() throw(SocketException) {
#ifdef _WIN32
		if (WSACleanup() != 0) {
			throw SocketException("WSACleanup() failed");
		}
#endif // _WIN32
	}

	unsigned short Socket::resolveService(const std::string &service,
		const std::string &protocol) {
		struct servent *serv = getservbyname(service.c_str(), protocol.c_str()); /* Structure containing service information */
		if (NULL == serv)
			return (unsigned short) atoi(service.c_str());  /* Service is port number */
		else
			return ntohs(serv->s_port);    /* Found port (network byte order) by name */
	}

	std::string Socket::getHostAddress() throw(SocketException)
	{
        char hostbuf[256];
        if (gethostname(hostbuf, sizeof(hostbuf))) {
			throw SocketException("Failed to gethostname()");
		}
        hostent *host = gethostbyname(hostbuf);
		if (NULL == host) {
			throw SocketException("Failed to gethostbyname()");
		}
		in_addr ip_addr = *(struct in_addr*)(host->h_addr_list[0]);
		return inet_ntoa(ip_addr);
	}
	// UDPSocket Code

	UDPSocket::UDPSocket() throw(SocketException) : 
		Socket(SOCK_DGRAM, IPPROTO_UDP) {
		if (this->sockDesc < 0) {
			throw SocketException("Socket creation failed (socket())", true);
		}
	}

	UDPSocket::UDPSocket(unsigned char mode) throw(SocketException) :
		Socket(SOCK_DGRAM, IPPROTO_UDP) {
		if (this->sockDesc < 0) {
			throw SocketException("Socket creation failed (socket())", true);
		}
		try {
			setScktMode(mode);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
	}

	UDPSocket::UDPSocket(unsigned short localPort)  throw(SocketException) :
		Socket(SOCK_DGRAM, IPPROTO_UDP) {
		if (this->sockDesc < 0) {
			throw SocketException("Socket creation failed (socket())", true);
		}
		try {
			this->bindsock(localPort);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
	}

	void UDPSocket::setScktMode(unsigned char mode)  throw(SocketException)
	{
		try{
			int enable = (mode & ShareAddress) ? 1 : 0;
            this->setsockoption(SOL_SOCKET, SO_REUSEADDR, (raw_type*)&enable, sizeof(enable));
		}
		catch (SocketException e){
			throw SocketException(std::string("SO_REUSEADDR set failed for ShareAddress").append(e.what()));
		}

#ifdef _WIN32
		unsigned long iMode = (mode & NonBlocking) ? 1 : 0;
		if (ioctlsocket(this->sockDesc, FIONBIO, &iMode) < 0){
			throw SocketException("FIONBIO set failed for Nonblocking", true);
		}
#else
		int iMode = fcntl(this->sockDesc, F_GETFL, 0);
		if (iMode < 0){
			throw SocketException("F_GETFL failed for Nonblocking", true);
		}
		iMode = (mode & NonBlocking) ? (iMode | O_NONBLOCK) : (iMode & ~O_NONBLOCK);
		if (fcntl(this->sockDesc, F_SETFL, iMode) < 0){
			throw SocketException("F_SETFL failed for Nonblocking", true);
		}
#endif // _WIN32					
		try{
			// If this fails, we'll hear about it when we try to send.  This will allow 
			// system that cannot broadcast to continue if they don't plan to broadcast
			int broadcastPermission = (mode & Broadcast) ? 1 : 0;
            this->setsockoption(SOL_SOCKET, SO_BROADCAST, (raw_type*)&broadcastPermission, sizeof(broadcastPermission));
		}
		catch (SocketException e){
			throw SocketException(std::string("SO_BROADCAST set failed for Broadcast").append(e.what()));
		}		
	}

	UDPSocket::UDPSocket(unsigned short localPort, unsigned char mode)  throw(SocketException) :
		Socket(SOCK_DGRAM, IPPROTO_UDP) {
		if (this->sockDesc < 0) {
			throw SocketException("Socket creation failed (socket())", true);
		}
		try {
			setScktMode(mode);
			this->bindsock(localPort);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
	}

	UDPSocket::UDPSocket(const std::string &localAddress, unsigned short localPort)
		throw(SocketException) : Socket(SOCK_DGRAM, IPPROTO_UDP) {
		if (this->sockDesc < 0) {
			throw SocketException("Socket creation failed (socket())", true);
		}
		try {
			this->bindsock(localAddress, localPort);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
	}

	UDPSocket::UDPSocket(const std::string &localAddress, unsigned short localPort, unsigned char mode)
		throw(SocketException) : Socket(SOCK_DGRAM, IPPROTO_UDP) {
		if (this->sockDesc < 0) {
			throw SocketException("Socket creation failed (socket())", true);
		}
		try {
			setScktMode(mode);
			this->bindsock(localAddress, localPort);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
	}

	void UDPSocket::sendTo(const void *buffer, int bufferLen,
		const std::string &foreignAddress, unsigned short foreignPort)
		throw(SocketException) {
		sockaddr_in destAddr;
		try {
			fillAddr(foreignAddress, foreignPort, destAddr);
		}
		catch (SocketException e) {
			throw SocketException(e);
		}
		// Write out the whole buffer as a single message.
        if (sendto(this->sockDesc, (raw_type*)buffer, bufferLen, 0,
			(sockaddr *)&destAddr, sizeof(destAddr)) != bufferLen) {
			throw SocketException("Send failed (sendto())", true);
		}
	}

	int UDPSocket::recvFrom(void *buffer, int bufferLen, 
							timeval *pckt_ts, 
							std::string *sourceAddress, 
							unsigned short *sourcePort) throw(SocketException)
	{
		sockaddr_in clntAddr;
		socklen_t addrLen = sizeof(clntAddr);
		int rtn;
		rtn = recvfrom(this->sockDesc, (raw_type*)buffer, bufferLen, 0, (sockaddr *)&clntAddr, (socklen_t *)&addrLen);
		if (rtn < 0) {
#ifdef _WIN32
			int error = WSAGetLastError();
			if ((WSAEWOULDBLOCK == error) || (WSAETIMEDOUT == error)) {
#else
			if ((EWOULDBLOCK == errno) || (ETIMEDOUT == errno)) {
#endif // _WIN32
				rtn = 0;
			}
#ifdef _WIN32
			else if (WSAEMSGSIZE == error) {
#else
			else if ((EMSGSIZE == errno)) {
#endif // _WIN32
				std::cout << "\x1B[33m" << "Warning: The buffer used to receive a datagram into was smaller than the datagram itself, ignoring datagram" << "\033[0m" << std::endl;
				rtn = 0;
			}
			else {
				throw SocketException("Receive failed (recvfrom())", true);
			}
			}
		else {
			if (NULL != pckt_ts) {
				gettimeofday(pckt_ts, NULL);
			}
			if (NULL != sourceAddress) {
				*sourceAddress = inet_ntoa(clntAddr.sin_addr);
			}
			if (NULL != sourcePort) {
				*sourcePort = ntohs(clntAddr.sin_port);
			}
		}
		return rtn;
	}

	int UDPSocket::peekFrom(void *buffer, int bufferLen,
							timeval *pckt_ts,
						    std::string *sourceAddress, 
							unsigned short *sourcePort) throw(SocketException)
	{
		sockaddr_in clntAddr;
		socklen_t addrLen = sizeof(clntAddr);
		int rtn;
        rtn = recvfrom(this->sockDesc, (raw_type*)buffer, bufferLen, MSG_PEEK, (sockaddr *)&clntAddr, (socklen_t *)&addrLen);
        if (rtn < 0) {
#ifdef _WIN32
			int error = WSAGetLastError();
            if ((WSAEWOULDBLOCK == error) || (WSAETIMEDOUT == error)) {
#else
            if ((EWOULDBLOCK == errno) || (ETIMEDOUT == errno)) {
#endif // _WIN32
                rtn = 0;
            }
#ifdef _WIN32
            else if (WSAEMSGSIZE == WSAGetLastError()) {
#else
            else if ((EMSGSIZE == errno)) {
#endif // _WIN32
				std::cout << "\x1B[33m" << "Warning: The buffer used to receive a datagram into was smaller than the datagram itself, ignoring datagram" << "\033[0m" << std::endl;
                rtn = 0;
            }
            else {
                throw SocketException("Receive failed (recvfrom())", true);
            }
        }
        else {
			if (NULL != pckt_ts) {
				gettimeofday(pckt_ts, NULL);
			}
            if (NULL != sourceAddress) {
                *sourceAddress = inet_ntoa(clntAddr.sin_addr);
            }
            if (NULL != sourcePort) {
                *sourcePort = ntohs(clntAddr.sin_port);
            }
        }
		return rtn;
	}

	void UDPSocket::setMulticastTTL(unsigned char multicastTTL) throw(SocketException) {
		try{
			this->setsockoption(IPPROTO_IP, IP_MULTICAST_TTL,
                            (raw_type*)&multicastTTL,
							sizeof(multicastTTL));
		}
		catch (SocketException e){
			throw SocketException(std::string("Multicast TTL set failed ").append(e.what()));
		}
	}

	void UDPSocket::joinGroup(const std::string &multicastGroup) throw(SocketException) {
		struct ip_mreq multicastRequest;

		multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
		multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
        try{
			this->setsockoption(IPPROTO_IP, IP_ADD_MEMBERSHIP,
                            (raw_type*)&multicastRequest,
					        sizeof(multicastRequest));
		}
		catch (SocketException e){
			throw SocketException(std::string("Multicast group join failed ").append(e.what()));
		}
	}

	void UDPSocket::leaveGroup(const std::string &multicastGroup) throw(SocketException) {
		struct ip_mreq multicastRequest;

		multicastRequest.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
		multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
		try{
			this->setsockoption(IPPROTO_IP, IP_DROP_MEMBERSHIP,
                            (raw_type*)&multicastRequest,
					        sizeof(multicastRequest));
		}
		catch (SocketException e){
			throw SocketException(std::string("Multicast group leave failed ").append(e.what()));
		}
	}

}
