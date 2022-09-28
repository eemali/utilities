#include "../../include/socket.h"   // For UDPSocket and SocketException
#include <iostream>					// For cout and cerr
#include <cstdlib>					// For atoi()

const int ECHOMAX = 255;     // Longest string to echo

int main(int argc, char *argv[]) {

	if (argc != 2) {                  // Test for correct number of parameters
		std::cerr << "Usage: " << argv[0] << " <Server Port>" << std::endl;
		exit(1);
	}

	unsigned short echoServPort = atoi(argv[1]);     // First arg:  local port

	try {
		Network::UDPSocket sock(echoServPort);

		char echoBuffer[ECHOMAX];         // Buffer for echo string
		int recvMsgSize;                  // Size of received message
		struct timeval pckt_ts;
		std::string sourceAddress;        // Address of datagram source
		unsigned short sourcePort;        // Port of datagram source
		for (;;) {  // Run forever
			// Block until receive message from a client
			recvMsgSize = sock.recvFrom(echoBuffer, ECHOMAX, &pckt_ts, &sourceAddress, &sourcePort);

			double t = pckt_ts.tv_sec + pckt_ts.tv_usec * 1e-6;
			std::cout << "Received packet from " << sourceAddress << ": " << sourcePort
				<< " pckt_ts: " << std::fixed << t << std::endl;

			sock.sendTo(echoBuffer, recvMsgSize, sourceAddress, sourcePort);
		}
	}
	catch (Network::SocketException &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	// NOT REACHED

	return 0;
}
