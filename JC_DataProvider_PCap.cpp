/*
 * JC_DataProvider_PCap.cpp
 *
 *  Created on: Sep 28, 2011
 *      Author: javid
 */

#include "JC_DataProvider_PCap.h"

JC_DataProvider_PCap::JC_DataProvider_PCap()
{
	JP_N=0;
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_PCap::Start(MI_DataProviderActor* actor)
{
	MP_Actor=actor;
	return true;
}
//-----------------------------------------------------------------------------
void JC_DataProvider_PCap::End()
{
}
//-----------------------------------------------------------------------------
bool JC_DataProvider_PCap::BeginRecive()
{

	char errBuf[MCONST_BUFF_SIZE];
	const u_char *data;
	struct pcap_pkthdr h;
    struct stat st;


	pcap_t* handle=pcap_open_offline(MP_Actor->Config()->GetConfig()->Provider.PCapFileName ,errBuf);
	if(!handle)
		return false;

	while(true)
	{
		data=pcap_next(handle,&h);
		if(!data)
			break;

		PcapHandlet(&h,data);
		usleep(10000);
		if (CHECK_PROTGRAM_TERMINATE)
			break;
	}
	pcap_close(handle);
	return true;
}
//-----------------------------------------------------------------------------
void JC_DataProvider_PCap::PcapHandlet(const struct pcap_pkthdr *h,const u_char *bytes)
{
	MS_DataProviderBuffer * buffer = (MS_DataProviderBuffer *)JGetMem(sizeof(MS_DataProviderBuffer));
	if (!buffer)
		return ;

	memset(buffer, 0, sizeof(MS_DataProviderBuffer));
	int DestPort;
	iphdr_* ip;
	udphdr_* udp;

	bytes+= 14;//ether_header
	ip=(iphdr_*)bytes;
	int iphl=ip->ihl*4;

	if(ip->protocol==IPPROTO_TCP)
	{
//		int tcphL=0;
//		tcphdr* tcp;
//		tcphL=4*(bytes[iphl + 12] >> 4);
//		tcp=(tcphdr*)(bytes+(iphl));
//		memcpy(buffer.buf, tcp + sizeof(tcphdr), 0);
	}
	else if(ip->protocol==IPPROTO_UDP)
	{
		//int udphL=sizeof(udphdr_);
		udp=(udphdr_*)(bytes+(iphl));
		int i=htons(udp->len);

		memset(buffer->buf, 0, MCONST_PROVIDER_BUFF_SIZE);
		if (i>MCONST_PROVIDER_BUFF_SIZE)
		{
			printf ("%d   %d \n", i, MCONST_PROVIDER_BUFF_SIZE);
			throw 1001;
			return ;
		}
		buffer->DeviceIP = ip->saddr;
		DestPort = htons(udp->dest);
		memcpy(buffer->buf, ((u_char *)udp) + sizeof(udphdr_),  i - sizeof(udphdr_));
		buffer->DataProviderType = ME_DataProviderType_Pcap;

		if (DestPort == 514)
		{

			buffer->LogType = ME_ProviderLogType_Syslog;
			 MP_Actor->OnReciveData(buffer);
		}
		// Taghyyi dadam
		//JFreeMem(buffer->buffer, sizeof(MS_DataProviderBuffer));
		JFreeMem(buffer, sizeof(MS_DataProviderBuffer));
		JP_N++;

	}

}
/**/
