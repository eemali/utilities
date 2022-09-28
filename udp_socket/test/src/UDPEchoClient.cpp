#include "../../include/socket.h"   // For UDPSocket and SocketException
#include <iostream>					// For cout and cerr
#include <cstdlib>					// For atoi()

const int ECHOMAX = 255;          // Longest string to echo

int main(int argc, char *argv[]) {
	if ((argc < 3) || (argc > 4)) {   // Test for correct number of arguments
		std::cerr << "Usage: " << argv[0]
			<< " <Server> <Echo String> [<Server Port>]\n";
		exit(1);
	}

	std::string servAddress = argv[1];             // First arg: server address
	char* echoString = argv[2];               // Second arg: string to echo
	int echoStringLen = strlen(echoString);   // Length of string to echo
	if (echoStringLen > ECHOMAX) {    // Check input length
		std::cerr << "Echo string too long" << std::endl;
		exit(1);
	}
	unsigned short echoServPort = Network::Socket::resolveService(
		(argc == 4) ? argv[3] : "echo", "udp");

	try {
		Network::UDPSocket sock;

		// Send the string to the server
		sock.sendTo(echoString, echoStringLen, servAddress, echoServPort);
		/*std::string srcAddr;
		unsigned short srcport;*/
		// Receive a response
		char echoBuffer[ECHOMAX + 1];       // Buffer for echoed string + \0
		int respStringLen;                  // Length of received response
		// ,srcAddr,srcport
		if ((respStringLen = sock.recvFrom(echoBuffer, ECHOMAX)) != echoStringLen) {
			std::cerr << "Unable to receive" << std::endl;
			exit(1);
		}

		echoBuffer[respStringLen] = '\0';             // Terminate the string!
		std::cout << "Received: " << echoBuffer << std::endl;   // Print the echoed arg

		// Destructor closes the socket

	}
	catch (Network::SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	return 0;
}
