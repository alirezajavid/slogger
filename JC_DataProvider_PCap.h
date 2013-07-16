/*
 * JC_DataProvider_PCap.h
 *
 *  Created on: Sep 28, 2011
 *      Author: javid
*/

#ifndef JC_DATAPROVIDER_PCAP_H_
#define JC_DATAPROVIDER_PCAP_H_


#include <pcap/pcap.h>
#include "Common.h"

struct iphdr_
{
	unsigned char  ihl:4,
				   version:4;
	unsigned char  tos;
	unsigned short tot_len;
	unsigned short id;
	unsigned short frag_off;
	unsigned char  ttl;
	unsigned char  protocol;
	unsigned short check;
	unsigned int   saddr;
	unsigned int   daddr;
};

struct tcphdr_
{
	unsigned short source;
	unsigned short dest;
	unsigned int   seq;
	unsigned int   ack_seq;
	unsigned short res1:4,
		       doff:4,
		       fin:1,
		       syn:1,
		       rst:1,
		       psh:1,
		       ack:1,
		       urg:1,
		       ece:1,
		       cwr:1;
	unsigned short  window;
	unsigned short  check;
	unsigned short  urg_ptr;
};

struct udphdr_
{
	unsigned short  source;
	unsigned short  dest;
	unsigned short  len;
	unsigned short  check;
};

class JC_DataProvider_PCap: public MI_DataProvider {
	public:
		JC_DataProvider_PCap();
		JE_Provider_Type GetType() { return JE_Provider_Type_PCap;};
		bool Start(MI_DataProviderActor* actor);
		void End();
		bool BeginRecive();
	private:
		int JP_N;
		MI_DataProviderActor* MP_Actor;
		void PcapHandlet(const struct pcap_pkthdr *h,const u_char *bytes);
};

#endif /* JC_DATAPROVIDER_PCAP_H_ */
