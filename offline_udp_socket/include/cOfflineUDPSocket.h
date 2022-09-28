#ifndef OFFLINE_SOCKET_H
#define OFFLINE_SOCKET_H
#include <cstdint>
#include <iostream>
#ifdef _WIN32
#include <winsock.h>  
#else
#include <sys/time.h>
#endif // _WIN32
namespace Network 
{
#pragma pack(push, 1)
	struct pcap_hdr_t
	{
		uint32_t magic_number;   /* magic number */
		uint16_t version_major;  /* major version number */
		uint16_t version_minor;  /* minor version number */
		uint32_t thiszone;       /* GMT to local correction */
		uint32_t sigfigs;        /* accuracy of timestamps */
		uint32_t snaplen;        /* max length of captured packets, in octets */
		uint32_t network;        /* data link type */
	};
	struct pcap_rec_hdr_t
	{
		uint32_t ts_sec;         /* timestamp seconds */
		uint32_t ts_usec;        /* timestamp microseconds */
		uint32_t incl_len;       /* number of octets of packet saved in file */
		uint32_t orig_len;       /* actual length of packet */
	};
#pragma pack(pop)
	class cOfflineUDPSocket
	{
	public:
		cOfflineUDPSocket();
		explicit cOfflineUDPSocket(std::string pcap_fileName, uint16_t localPort = 0);
		~cOfflineUDPSocket();
		void bindsock(std::string pcap_fileName, uint16_t localPort = 0);
		int recvFrom(void *buffer, int bufferLen, struct timeval *ts_ptr = NULL, std::string *sourceAddress = NULL,
			unsigned short *sourcePort = NULL);
		int peekFrom(void *buffer, int bufferLen, struct timeval *ts_ptr = NULL, std::string *sourceAddress = NULL,
			unsigned short *sourcePort = NULL);
	private:
		FILE *fpcap;
		uint16_t local_port;
		pcap_hdr_t pcap_hdr;
		pcap_rec_hdr_t pcap_rec_hdr;
		uint64_t pcktnumber;
	};
}
#endif // !OFFLINE_SOCKET_H
