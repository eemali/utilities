#include "../../include/socket.h"   // For UDPSocket and SocketException
#include <iostream>					// For cout and cerr
#include <cstdlib>					// For atoi()

const int MAXRCVSTRING = 4096; // Longest string to receive

int main(int argc, char *argv[]) {

  if (argc != 2) {                  // Test for correct number of parameters
    std::cerr << "Usage: " << argv[0] << " <Local Port>" << std::endl;
    exit(1);
  }

  unsigned short echoServPort = atoi(argv[1]);     // First arg:  local port

  try {
	  Network::UDPSocket sock(echoServPort);
  
    char recvString[MAXRCVSTRING + 1]; // Buffer for echo string + \0
	struct timeval pckt_ts;
    std::string sourceAddress;         // Address of datagram source
    unsigned short sourcePort;         // Port of datagram source
	
	for (;;) {
		int bytesRcvd = sock.recvFrom(recvString, MAXRCVSTRING, &pckt_ts, &sourceAddress, &sourcePort);
		recvString[bytesRcvd] = '\0';  // Terminate string

		double t = pckt_ts.tv_sec + pckt_ts.tv_usec * 1e-6;
		std::cout << "Received " << recvString << " from "
			<< sourceAddress << ": " << sourcePort
			<< " pckt_ts: " << std::fixed << t << std::endl;
	}
  } 
  catch (Network::SocketException &e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
  return 0;
}
