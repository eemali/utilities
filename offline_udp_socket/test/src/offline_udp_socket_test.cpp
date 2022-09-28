#include "../../include/cOfflineUDPSocket.h" // include cOfflineUDPSocket.h file
#include <iostream>
#include <sstream> // for std::ostringstream
#include <iomanip> // for std::steprecision

#define PCKT_BUF_LEN 4096U // packet buffer size

// Helper function to print received UDP packet in Hex
std::string strPcktInHex(uint8_t *ptr, uint16_t len);

int main(int argc, char** argv) 
{
	/*======================= Need Parameter Initilization =======================*/
	
	// Current version 'cOfflineUDPSocket' can only support .pcap file
	// containing only UDP packet, so make sure that your .pcap file contains 
	// only UDP packets otherwise its output may be unexpected 
	std::string pcap_filename = "../../my_udp.pcap";	
	
	// port number to be listen
	// If port number = 0, it acts as raw UDP socket
	unsigned short port_number = 4032U;
	
	// Buffer to hold received UDP packet
	unsigned char pcktBuffer[PCKT_BUF_LEN] = { 0 }; // let's initilize it to zeros
	
	// to hold size of received UDP packet in term of bytes
	int len;

	// [Optional] to hold time stamp (stamped by wireshark by using local system time)
	// of received UDP packet
	struct timeval pckt_ts;

	// [Optional] to hold source ip address and port number
	std::string sourceAddress;
	unsigned short sourcePort;


	/*=========== Two ways to initilize and bind the offline UDP socket ===========*/
	
	// 1. Seperate initilization and binding 
	// a. initilization using default constructor
	Network::cOfflineUDPSocket offlineUDPSocket_1;
	// b. port binding using 'bindsock' function.
	// Note that port_number is optional parameter. If not provided or port_number = 0,
	// then this socket will act as raw UDP socket.
	offlineUDPSocket_1.bindsock(pcap_filename, port_number);


	// 2. Single shot initilization and binding
	// using parametric constrcutor. 
	// Note again that port_number is optional parameter. If not provided or port_number = 0,
	// then this socket will act as raw UDP socket.
	Network::cOfflineUDPSocket offlineUDPSocket_2(pcap_filename, port_number);

	/*=============================================================================*/

	while (true) {
		
		// Use 'recvFrom' function to read received UDP.
		// It reads receieved UDP packet, move pointer to next received UDP packet and
		// returns length of currently read UDP packet.
		// The returned length = -1 indicates end of file. 
		// Note that 'pckt_ts', 'sourceAddress', and 'sourcePort' are optional parameters
		len = offlineUDPSocket_1.recvFrom(pcktBuffer, PCKT_BUF_LEN, &pckt_ts, &sourceAddress, &sourcePort);  		
		
		// 'peekFrom' function is same as 'recvFrom' except it does not move pointer to
		// to next received UDP packet

		// Is end of file
		if (len < 0) { 
			break; // then break the infinite while loop
		}

		// Print received packet details
		std::cout << "------------------------------------------------------" << std::endl;
		std::cout << "Source IP: " << sourceAddress << std::endl;
		std::cout << "Source Port: " << sourcePort << std::endl;
		std::cout << "Length: " << len << std::endl;
		std::cout << "Data: " << std::endl << strPcktInHex(pcktBuffer, len) << std::endl;		
	}

	return 0;
}

std::string strPcktInHex(uint8_t *ptr, uint16_t len)
{
	std::ostringstream out;
	for (int ctr = 1; ctr <= len; ++ctr, ++ptr) {
		out << std::setfill('0') << std::setw(2) << std::hex << (int)*ptr;
		if (0 == ctr % 16) { out << std::endl; }
		else { out << " "; }
	}
	return out.str();
}