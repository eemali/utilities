#include "../include/cOfflineUDPSocket.h"
#include <cassert> // for assert
#include <sstream> // for std::ostringstream
namespace Network
{
	cOfflineUDPSocket::cOfflineUDPSocket() : fpcap(NULL) {}
	cOfflineUDPSocket::cOfflineUDPSocket(std::string pcap_fileName, uint16_t localPort)
	{
		bindsock(pcap_fileName, localPort);
	}
	cOfflineUDPSocket::~cOfflineUDPSocket()
	{
		fclose(fpcap);
	}
	void cOfflineUDPSocket::bindsock(std::string pcap_fileName, uint16_t localPort)
	{
		fpcap = fopen(pcap_fileName.c_str(), "rb");
		if (NULL == fpcap) {
			std::cerr << "\x1B[31m" << "Error: couldn't able to open " + pcap_fileName + " for reading" << "\033[0m" << std::endl;
			assert(0);
		}
		local_port = localPort;
		fread(&pcap_hdr, sizeof(pcap_hdr), 1, fpcap);
		pcktnumber = 0;
	}	
	int cOfflineUDPSocket::recvFrom(void *buffer, int bufferLen, struct timeval *ts_ptr, std::string *sourceAddress, unsigned short *sourcePort)
	{
		if (fread(&pcap_rec_hdr, sizeof(pcap_rec_hdr), 1, fpcap) <= 0) {
			/* EOF */
			return -1;
		}
		++pcktnumber;
		if (NULL != ts_ptr) {
			ts_ptr->tv_sec = (long)pcap_rec_hdr.ts_sec;
			ts_ptr->tv_usec = (long)pcap_rec_hdr.ts_usec;
		}
		if (pcap_rec_hdr.incl_len != pcap_rec_hdr.orig_len) {
			std::cout << "\x1B[33m" << "Warning : incl_len != orig_len for UDP Packet # " << pcktnumber << "\033[0m" << std::endl;
		}

		fseek(fpcap, 26, SEEK_CUR);
		uint8_t buf[4] = {0};
		uint8_t src_ip[4]; fread(&src_ip, sizeof(uint8_t), 4, fpcap);
		uint8_t dst_ip[4]; fread(&dst_ip, sizeof(uint8_t), 4, fpcap);
		uint16_t src_port; fread(&buf, sizeof(uint8_t), 2, fpcap); src_port = ((uint16_t)buf[0] << 8) + ((uint16_t)buf[1]);
		uint16_t dst_port; fread(&buf, sizeof(uint8_t), 2, fpcap); dst_port = ((uint16_t)buf[0] << 8) + ((uint16_t)buf[1]);
		uint16_t len; fread(&buf, sizeof(uint8_t), 2, fpcap); len = ((uint16_t)buf[0] << 8) + ((uint16_t)buf[1]) - 8;
		if (NULL != sourceAddress) {
			*sourceAddress = static_cast<std::ostringstream&>(std::ostringstream() << (int)src_ip[0] << "." << (int)src_ip[1] << "." << (int)src_ip[2] << "." << (int)src_ip[3]).str();
		}
		if (NULL != sourcePort) {
			*sourcePort = src_port;
		}
		if ((0 != local_port) && (dst_port != local_port)) {
			fseek(fpcap, pcap_rec_hdr.orig_len - 40, SEEK_CUR);
			return 0;
		}
		if ((int)len > bufferLen) {
			std::cout << "\x1B[33m" << "Warning: The buffer used to receive a datagram into was smaller than the datagram itself, ignoring datagram" << "\033[0m" << std::endl;
			fseek(fpcap, pcap_rec_hdr.orig_len - 40, SEEK_CUR);
			return 0;
		}
		fseek(fpcap, 2, SEEK_CUR);
		fread(buffer, len, 1, fpcap);
		fseek(fpcap, pcap_rec_hdr.orig_len - len - 42, SEEK_CUR);
		return len;
	}
	int cOfflineUDPSocket::peekFrom(void *buffer, int bufferLen, struct timeval *ts_ptr, std::string *sourceAddress, unsigned short *sourcePort)
	{
		int len = recvFrom(buffer, bufferLen, ts_ptr, sourceAddress, sourcePort);
		if (len >= 0) {
			fseek(fpcap, -(long)(pcap_rec_hdr.orig_len + sizeof(pcap_rec_hdr)), SEEK_CUR);
			--pcktnumber;
		}
		return len;
	}
}