// SPcap.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <pcap.h>
#include <remote-ext.h>

#include <Windows.h>
#include <IPHlpApi.h>

#include "SPI.h"

#pragma  comment(lib,"iphlpapi")

SPI spi;
class CSockInit 
{
public:
	CSockInit(BYTE minorVer = 2, BYTE majorVer = 2)
	{ // ��ʼ�� WS2_32.dll
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		if(::WSAStartup(sockVersion, &wsaData) != 0)
		{ exit(0); }
	}
	~CSockInit()
	{ ::WSACleanup(); }
};

CSockInit sockInit;
//����һ���µ�GUID
const char* newGUID();

/* ÿ�β������ݰ�ʱ��libpcap�����Զ���������ص����� */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int  GetAdapter()
{
	pcap_if_t * allAdapters;//�������б�
	pcap_if_t * adapter;
	char errorBuffer[ PCAP_ERRBUF_SIZE ];//������Ϣ������
	if( pcap_findalldevs_ex( PCAP_SRC_IF_STRING, NULL, 
		&allAdapters, errorBuffer ) == -1 )
	{//�����������ӵ���������������
		fprintf( stderr, "Error in pcap_findalldevs_ex function: %s\n", errorBuffer );
		return -1;
	}
	if( allAdapters == NULL )
	{//�������κ�������
		printf( "\nNo adapters found! Make sure WinPcap is installed.\n" );
		return 0;
	}
	int crtAdapter = 0;
	for( adapter = allAdapters; adapter != NULL; adapter = adapter->next)
	{//����������������Ϣ(���ƺ�������Ϣ)
		printf( "\n%d.%s ", ++crtAdapter, adapter->name );
		printf( "-- %s\n", adapter->description );
	}
	printf( "\n" );
	pcap_freealldevs( allAdapters );//�ͷ��������б�
	return 0;
}
int GetAdapterWinSDK()
{
	PIP_ADAPTER_INFO pAdpterInfo=NULL;
	ULONG nSize;
	//��һ�λ�ȡnsize��С
	GetAdaptersInfo(pAdpterInfo,&nSize);
	//������ʵ��ڴ�
	pAdpterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, nSize);

	GetAdaptersInfo(pAdpterInfo,&nSize);
	while(pAdpterInfo!=NULL)
	{
		printf("%s,%s\n",pAdpterInfo->AdapterName,pAdpterInfo->Description);
		pAdpterInfo=pAdpterInfo->Next;
	}
	return 0;
}

int CapturePacket()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	/* ��ȡ�����豸�б� */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* ��ӡ�б� */
	for(d=alldevs; d; d=d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if(i==0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	printf("Enter the interface number (1-%d):",i);
	scanf("%d", &inum);

	if(inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* ��ת��ѡ�е������� */
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

	/* ���豸 */
	if ( (adhandle= pcap_open(d->name,          // �豸��
		65536,            // 65535��֤�ܲ��񵽲�ͬ������·���ϵ�ÿ�����ݰ���ȫ������
		PCAP_OPENFLAG_PROMISCUOUS,    // ����ģʽ
		1000,             // ��ȡ��ʱʱ��
		NULL,             // Զ�̻�����֤
		errbuf            // ���󻺳��
		) ) == NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* �ͷ��豸�б� */
	pcap_freealldevs(alldevs);

	/* ��ʼ���� */
	pcap_loop(adhandle, 0, packet_handler, NULL);

	return 0;

}
/* ÿ�β������ݰ�ʱ��libpcap�����Զ���������ص����� */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;

	/* ��ʱ���ת���ɿ�ʶ��ĸ�ʽ */
	local_tv_sec = header->ts.tv_sec;
	ltime=localtime(&local_tv_sec);
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

	printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len);

}


//GUID�Ĳ���
const char* newGUID()
{
	static char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		sprintf_s(buf, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			,sizeof(buf)
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}
	return (const char*)buf;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//pcap����
	//GetAdapter();
	//win����
	//GetAdapterWinSDK();
	//CapturePacket();
	//spi.EnumProtocol();
	//spi.PrintProtocol();
	//spi.FreeProtocol();
	spi.SPIEnumProtocol();
	spi.SPIPrintProtocol();
	spi.SPiFreeProtocol();
	system( "PAUSE" );
	return 0;
}

