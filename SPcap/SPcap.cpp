// SPcap.cpp : 定义控制台应用程序的入口点。
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
	{ // 初始化 WS2_32.dll
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		if(::WSAStartup(sockVersion, &wsaData) != 0)
		{ exit(0); }
	}
	~CSockInit()
	{ ::WSACleanup(); }
};

CSockInit sockInit;
//产生一个新的GUID
const char* newGUID();

/* 每次捕获到数据包时，libpcap都会自动调用这个回调函数 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int  GetAdapter()
{
	pcap_if_t * allAdapters;//适配器列表
	pcap_if_t * adapter;
	char errorBuffer[ PCAP_ERRBUF_SIZE ];//错误信息缓冲区
	if( pcap_findalldevs_ex( PCAP_SRC_IF_STRING, NULL, 
		&allAdapters, errorBuffer ) == -1 )
	{//检索机器连接的所有网络适配器
		fprintf( stderr, "Error in pcap_findalldevs_ex function: %s\n", errorBuffer );
		return -1;
	}
	if( allAdapters == NULL )
	{//不存在任何适配器
		printf( "\nNo adapters found! Make sure WinPcap is installed.\n" );
		return 0;
	}
	int crtAdapter = 0;
	for( adapter = allAdapters; adapter != NULL; adapter = adapter->next)
	{//遍历输入适配器信息(名称和描述信息)
		printf( "\n%d.%s ", ++crtAdapter, adapter->name );
		printf( "-- %s\n", adapter->description );
	}
	printf( "\n" );
	pcap_freealldevs( allAdapters );//释放适配器列表
	return 0;
}
int GetAdapterWinSDK()
{
	PIP_ADAPTER_INFO pAdpterInfo=NULL;
	ULONG nSize;
	//第一次获取nsize大小
	GetAdaptersInfo(pAdpterInfo,&nSize);
	//申请合适的内存
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
	/* 获取本机设备列表 */
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	/* 打印列表 */
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
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* 跳转到选中的适配器 */
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);

	/* 打开设备 */
	if ( (adhandle= pcap_open(d->name,          // 设备名
		65536,            // 65535保证能捕获到不同数据链路层上的每个数据包的全部内容
		PCAP_OPENFLAG_PROMISCUOUS,    // 混杂模式
		1000,             // 读取超时时间
		NULL,             // 远程机器验证
		errbuf            // 错误缓冲池
		) ) == NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter. %s is not supported by WinPcap\n", d->name);
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);

	/* 释放设备列表 */
	pcap_freealldevs(alldevs);

	/* 开始捕获 */
	pcap_loop(adhandle, 0, packet_handler, NULL);

	return 0;

}
/* 每次捕获到数据包时，libpcap都会自动调用这个回调函数 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	time_t local_tv_sec;

	/* 将时间戳转换成可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	ltime=localtime(&local_tv_sec);
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

	printf("%s,%.6d len:%d\n", timestr, header->ts.tv_usec, header->len);

}


//GUID的产生
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
	//pcap环境
	//GetAdapter();
	//win环境
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

