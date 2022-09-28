#ifndef SOCKET_H
#define SOCKET_H
#include <string>            // For string
#include <exception>         // For exception class
#ifdef _WIN32
#include <winsock.h>         // For socket(), connect(), send(), and recv()
#ifdef _MSC_VER
#pragma comment (lib, "wsock32.lib")	// Need to link with wsock32.lib 
#endif // _MSC_VER
typedef int socklen_t;
typedef char raw_type;       // Type used for raw data on this platform
#else
#include <string.h>		     // For memset
#include <stdlib.h>          // For atoi()
#include <sys/types.h>       // For data types
#include <sys/socket.h>      // For socket(), connect(), send(), and recv()
#include <netdb.h>           // For gethostbyname()
#include <arpa/inet.h>       // For inet_addr()
#include <unistd.h>          // For close()
#include <netinet/in.h>      // For sockaddr_in
#include <fcntl.h>			 // For fcntl()
typedef void raw_type;       // Type used for raw data on this platform
#endif // _WIN32
#include <errno.h>             // For errno
#ifdef _WIN32
#include <winsock.h>  
#else
#include <sys/time.h>
#endif // _WIN32
namespace Network
{
	/*! \brief Signals a problem with the execution of a socket call.
	*/
	class SocketException : public std::exception {
	public:
		/*!
			Construct a SocketException with a explanatory message.
			@param message explanatory message
			@param incSysMsg true if system message (from strerror(errno))
			should be postfixed to the user provided message
		*/
		explicit SocketException(const std::string &message, bool inclSysMsg = false) throw();

		/*!
			Provided just to guarantee that no exceptions are thrown.
		*/
		~SocketException() throw();

		/*!
			Get the exception message
			@return exception message
		*/
		const char *what() const throw();

	private:
		std::string userMessage;  //!< Exception message
	};

	class Socket {
	public:
		/*!	Close and deallocate this socket
		 */
		~Socket();
		/*!  Get the local address
		     @return local address to which the socket is bound 
		     @exception SocketException thrown if fetch fails
		 */
		std::string getLocalAddress() throw(SocketException);

		/*!  Get the local port
		     @return local port to which the socket is bound
		     @exception SocketException thrown if fetch fails
		 */
		unsigned short getLocalPort() throw(SocketException);

		/*!  bind the socket to the specified local port and any address
		     @param localPort local port
		     @exception SocketException thrown if setting local port fails
		 */
		void bindsock(unsigned short localPort) throw(SocketException);

		/*!  bind the socket to the specified port and the specified address.  
		     If you omit the port, a random port
		     will be selected.
		     @param localAddress local address
		     @param localPort local port
		     @exception SocketException thrown if setting local port or address fails
		 */
		void bindsock(const std::string &localAddress,
				  unsigned short localPort = 0) throw(SocketException);

		/*! sets a socket option
		    @param level  The level at which the option is defined (for example, SOL_SOCKET).
		    @param optname The socket option for which the value is to be set (for example, SO_BROADCAST). 
		   				   The optname parameter must be a socket option defined within the specified level,
		   	      		   or behavior is undefined.
		    @param optval A pointer to the buffer in which the value for the requested option is specified.
		    @param optlen The size, in bytes, of the buffer pointed to by the optval parameter.
		    @exception SocketException thrown if setting socket option fails
		  */		
		void setsockoption(int level, int optname, 
			            raw_type* optval, socklen_t optlen) throw(SocketException);
		/*!  If WinSock, unload the WinSock DLLs; otherwise do nothing.  We ignore
		     this in our sample client code but include it in the library for
		     completeness.  If you are running on Windows and you are concerned
		     about DLL resource consumption, call this after you are done with all
		     Socket instances.  If you execute this on Windows while some instance of
		     Socket exists, you are toast.  For portability of client code, this is
		     an empty function on non-Windows platforms so you can always include it.		     
		     @exception SocketException thrown WinSock clean up fails
		 */
		static void cleanUp() throw(SocketException);

		/*!  Resolve the specified service for the specified protocol to the
		     corresponding port number in host byte order
		     @param service service to resolve (e.g., "http")
		     @param protocol protocol of service to resolve.  Default is "udp".
		 */
		static unsigned short resolveService(const std::string &service,
			const std::string &protocol = "udp");

		static std::string getHostAddress() throw(SocketException);
	private:
		/*! Prevent the user from trying to use value semantics on this object
		*/
		Socket(const Socket &sock);
		void operator=(const Socket &sock);

	protected:
		int sockDesc;              //!< Socket descriptor
		Socket();
		Socket(int type, int protocol);
		explicit Socket(int sockDesc);
	};

	/*!
	    UDP socket class
	*/
	class UDPSocket : public Socket {
	public:
		enum	BindFlag {
			/*! Allow other services to bind to the same address and port.
			    This is useful when multiple processes share the load of a 
			    single service by listening to the same address and port
			    (e.g., a web server with several pre - forked listeners can 
			    greatly improve response time). However, because any service 
			    is allowed to rebind, this option is subject to certain 
			    security considerations.Note that by using this option 
			    you will also allow your service to rebind an existing 
			    shared address.			   
			    Negation of this is as follow,
			    Bind the address and port exclusively, so that no other 
			    services are allowed to rebind. you are guaranteed that 
			    on successs, your service is the only one that listens to 
			    the address and port. No services are allowed to rebind  
			  */
				ShareAddress = 0x1,		
			/*! By default, sockets are in "blocking" mode. For example, 
			    when you call recvfrom() to read from a datagram, 
			    control isn't returned to your program until at least one 
			    byte of data is read from the remote site. This process of 
			    waiting for data to appear is referred to as "blocking".
			    Its possible to set a descriptor so that it is placed in 
			    "non-blocking" mode. When placed in non-blocking mode, you 
			    never wait for an operation to complete. This is an invaluable 
			    tool if you need to switch between many different connected sockets, 
			    and want to ensure that none of them cause the program to "lock up."
			    If you call "recv()" in non-blocking mode, it will return any data 
			    that the system has in it's read buffer for that socket. But, 
			    it won't wait for that data. If the read buffer is empty, the system 
			    will return from recv() immediately saying ``"Operation Would Block!"''.
			  */
				NonBlocking = 0x2,
			/*! Configures a socket for sending broadcast data.
			 */
				Broadcast = 0x4
		};
		/*!  Construct a UDP socket
		     @exception SocketException thrown if unable to create UDP socket
		 */
		UDPSocket() throw(SocketException);
		/*! Construct a UDP socket with given mode
		    @param mode binding mode
		    @exception SocketException thrown if unable to create UDP socket
		*/
		explicit UDPSocket(unsigned char mode) throw(SocketException);

		/*!  Construct a UDP socket with the given local port
		     @param localPort local port
		     @exception SocketException thrown if unable to create UDP socket
		 */
		explicit UDPSocket(unsigned short localPort) throw(SocketException);
		/*!  Construct a UDP socket with the given local port and
		     given binding mode
		     @param localPort local port
		     @param mode binding mode
		     @exception SocketException thrown if unable to create UDP socket
		 */
		UDPSocket(unsigned short localPort, unsigned char mode) throw(SocketException);
		/*!  Construct a UDP socket with the given local port and address
		     @param localAddress local address
		     @param localPort local port
		     @exception SocketException thrown if unable to create UDP socket
		 */
		UDPSocket(const std::string &localAddress, unsigned short localPort)
			throw(SocketException);

		/*!  Construct a UDP socket with the given local port, address and binding mode 
		     @param localAddress local address
		     @param localPort local port
		     @param mode binding mode
		     @exception SocketException thrown if unable to create UDP socket
		 */
		UDPSocket(const std::string &localAddress, unsigned short localPort, unsigned char mode)
			throw(SocketException);

		/*!  Send the datagram at buffer of size bufferLen to 
		     the host address foreignAddress at port foreignPort.   
		     @param buffer buffer to be written
		     @param bufferLen number of bytes to write
		     @param foreignAddress address (IP address or name) to send to
		     @param foreignPort port number to send to
		     @return true if send is successful
		     @exception SocketException thrown if unable to send datagram
		 */
		void sendTo(const void *buffer, int bufferLen, const std::string &foreignAddress,
					unsigned short foreignPort) throw(SocketException);
		 
		/*!  Receive a datagram no larger than bufferLen bytes and stores it in buffer.
		     The sender's host address and port is stored in sourceAddress and sourcePort if provided
		     @param buffer buffer to receive datagram
		     @param bufferLen maximum number of bytes to receive
		     @param sourceAddress address of datagram source
		     @param sourcePort port of data source
		     @return number of bytes received and -1 for error
		     @exception SocketException thrown if unable to receive datagram
		 */
		int recvFrom(void *buffer, int bufferLen, 
					 timeval *pckt_ts = NULL, 
					 std::string *sourceAddress = NULL,
				     unsigned short *sourcePort = NULL) throw(SocketException);
		/*!  Peek a datagram no larger than bufferLen bytes and stores it in buffer.
		     The datagram is treated as unread ans it is not removed from socket buffer.
		     The sender's host address and port is stored in sourceAddress and sourcePort if provided
		     @param buffer buffer to receive data
		     @param bufferLen maximum number of bytes to receive
		     @param sourceAddress address of datagram source
		     @param sourcePort port of data source
		     @return number of bytes received and -1 for error
		     @exception SocketException thrown if unable to receive datagram
		 */
		int peekFrom(void *buffer, int bufferLen, 
					 timeval *pckt_ts = NULL,
					 std::string *sourceAddress = NULL,
				     unsigned short *sourcePort = NULL) throw(SocketException);
		/*!  Set the multicast TTL
		     @param multicastTTL multicast TTL
		     @exception SocketException thrown if unable to set TTL
		 */
		void setMulticastTTL(unsigned char multicastTTL) throw(SocketException);

		/*!  Join the specified multicast group
		     @param multicastGroup multicast group address to join
		     @exception SocketException thrown if unable to join group
		 */
		void joinGroup(const std::string &multicastGroup) throw(SocketException);

		/*!  Leave the specified multicast group
		     @param multicastGroup multicast group address to leave
		     @exception SocketException thrown if unable to leave group
		 */
		void leaveGroup(const std::string &multicastGroup) throw(SocketException);
	//protected:
		void setScktMode(unsigned char mode) throw(SocketException);
	};
}
#endif
