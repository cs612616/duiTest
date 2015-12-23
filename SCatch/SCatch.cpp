#include "stdafx.h"
#include <WinSock2.h>
//#include <Mstcpip.h> 
#include <Windows.h>
#include <IPHlpApi.h>
#include <ws2tcpip.h>
#pragma  comment(lib,"ws2_32.lib")
#pragma  comment(lib,"iphlpapi.lib")
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
void GetIP()
{
	char szHost[256];
	//获取本机名称
	::gethostname(szHost,256);
	//通过主机名得到地址信息
	hostent *pHost=::gethostbyname(szHost);
	//打印地址
	in_addr  addr;
	for (int i=0;;i++)
	{
		char *p=pHost->h_addr_list[i];
		if (p==NULL)
		{
			return;
		}
		memcpy((void*)&addr.S_un.S_addr,p,pHost->h_length);
		char *szIp = ::inet_ntoa(addr);
		printf("本机IP地址： %s\n", szIp);
	}
}
void GetMac()
{
	u_char ucLocalMac[6]; // 本地 MAC 地
	DWORD dwGatewayIP; // 网关 IP 地址
	DWORD dwLocalIP; // 本地 IP 地址
	DWORD dwMask; // 子网掩码
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;
	// 为适配器结构申请内存
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);
	// 取得本地适配器结构信息
	if(::GetAdaptersInfo(pAdapterInfo,&ulLen) == ERROR_SUCCESS)
	{
		while(pAdapterInfo != NULL)
		{ 
			memcpy(ucLocalMac, pAdapterInfo->Address, 6);
			dwGatewayIP = ::inet_addr(pAdapterInfo->GatewayList.IpAddress.String);
			dwLocalIP = ::inet_addr(pAdapterInfo->IpAddressList.IpAddress.String);
			dwMask = ::inet_addr(pAdapterInfo->IpAddressList.IpMask.String);
			pAdapterInfo=pAdapterInfo->Next;
			printf(" \n -------------------- 本地主机信息 -----------------------\n\n");
			in_addr in;
			in.S_un.S_addr = dwLocalIP;
			printf(" IP Address : %s \n", ::inet_ntoa(in));
			in.S_un.S_addr = dwMask;
			printf(" Subnet Mask : %s \n", ::inet_ntoa(in));
			in.S_un.S_addr = dwGatewayIP;
			printf(" Default Gateway : %s \n", ::inet_ntoa(in));
			u_char *p = ucLocalMac;
			printf(" MAC Address : %02X-%02X-%02X-%02X-%02X-%02X \n", p[0], p[1], p[2], p[3], p[4], p[5]);
			printf(" \n \n ");
		}
	}
	return ;
}

//计算封包校验和
USHORT checksum(USHORT* buff, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buff++;
		size -= sizeof(USHORT);
	}
	// 是奇数
	if(size)
	{
		cksum += *(UCHAR*)buff;
	}
	// 将32位的chsum高16位和低16位相加，然后取反
	cksum = (cksum >> 16) + (cksum & 0xffff);//高16位 + 低16位
	cksum += (cksum >> 16);					 //+进位
	return (USHORT)(~cksum);				 //取反
}
BOOL SetTimeout(SOCKET s, int nTime, BOOL bRecv)
{
	int ret = ::setsockopt(s, SOL_SOCKET, 
		bRecv ? SO_RCVTIMEO : SO_SNDTIMEO, (char*)&nTime, sizeof(nTime));
	return ret != SOCKET_ERROR;
}
//icmp报文( ICMP回送请求与应答报文格式定义ICMP的数据结构)
typedef struct icmp_hdr
{
	unsigned char   icmp_type;		// 消息类型
	unsigned char   icmp_code;		// 代码
	unsigned short  icmp_checksum;	// 校验和
	// 下面是回显头
	unsigned short  icmp_id;		// 用来惟一标识此请求的ID号，通常设置为进程ID
	unsigned short  icmp_sequence;	// 序列号
	//unsigned long   icmp_timestamp; // 时间戳
} ICMP_HDR, *PICMP_HDR;

//ip 头
typedef struct _IP_hdr // 20 个字节
{ 
	unsigned char iphLen:4;         //length of header
	unsigned char iphVer:4; // 版本号和头长度（各占 4 位）
	UCHAR ipTOS;			// 服务类型
	USHORT ipLength;		// 封包总长度，即整个 IP 报的长度
	USHORT ipID;			// 封包标识，惟一标识发送的每一个数据报
	USHORT ipFlags;			// 标志
	UCHAR ipTTL;			// 生存时间，就是 TTL
	UCHAR ipProtocol;		// 协议，可能是 TCP、 UDP、 ICMP 等
	USHORT ipChecksum;		// 校验和
	ULONG ipSource;			// 源 IP 地址
	ULONG ipDestination;	// 目标 IP 地址
} IP_HDR, *PIP_HDR;

// IP option header - use with socket option IP_OPTIONS	
// ip可选头
typedef struct _ipoptionhdr
{
	unsigned char code;        // Option type
	unsigned char len;         // Length of option hdr
	unsigned char ptr;         // Offset into options
	unsigned long addr[9];     // List of IP addrs
} IpOptionHeader;
//由于有ip可选头的存在，那么 ip头的长度可变

BOOL SetTTL(SOCKET s, int nValue)
{
	int ret = ::setsockopt(s, IPPROTO_IP, IP_TTL, (char*)&nValue, sizeof(nValue));
	return ret != SOCKET_ERROR;
}
int ICMP()
{
	//目标地址
	char *szDestIp = "61.135.169.121";
	SOCKADDR_IN destAddr;
	//创建原始套接字
	SOCKET sRaw=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	SetTimeout(sRaw,1000,true);
	SetTimeout(sRaw,1000,false);
	if (sRaw==INVALID_SOCKET)
	{
		printf("socket create error");
		return -1;
	}
	destAddr.sin_family=AF_INET;
	destAddr.sin_port=htons(0);
	destAddr.sin_addr.S_un.S_addr=inet_addr(szDestIp);
	//icmp封包
	ICMP_HDR* pI_hdr;
	//发送数据
	char sndBuff[sizeof(ICMP_HDR)+32];
	pI_hdr=(ICMP_HDR*)sndBuff;
	//循环发送数据
	int seq=0;
	int nTTL=1;
	while(seq<=4)
	{
		SetTTL(sRaw, nTTL);
		nTTL++;
		int nRet=0;
		memset(sndBuff,0,sizeof(ICMP_HDR)+32);
		pI_hdr->icmp_type = 8;	// 请求一个ICMP回显
		pI_hdr->icmp_code = 0;
		pI_hdr->icmp_id=::GetCurrentProcessId();
		pI_hdr->icmp_sequence=seq++	;
		//pI_hdr->icmp_timestamp=::GetTickCount();
		memset(sndBuff+sizeof(ICMP_HDR),'E',32);
		//如果校验和计算错误，对方自动丢弃包，应该把校验和计算放在封包的最后一步
		pI_hdr->icmp_checksum = checksum((USHORT*)sndBuff, sizeof(ICMP_HDR) + 32);

		::sendto(sRaw,sndBuff,sizeof(ICMP_HDR)+32,0,(SOCKADDR*)&destAddr,sizeof(SOCKADDR));
		if(nRet == SOCKET_ERROR)
		{
			printf(" sendto() failed: %d \n", ::WSAGetLastError());
			return -1;
		}

		printf("type=%d,code=%d,checksum=%d,id=%d,seq=%d\n",pI_hdr->icmp_type,pI_hdr->icmp_code,pI_hdr->icmp_checksum,pI_hdr->icmp_id,pI_hdr->icmp_sequence);
		SOCKADDR_IN srcAddr;
		srcAddr.sin_family=AF_INET;
		srcAddr.sin_addr.S_un.S_addr=inet_addr(szDestIp);
		char recvBuff[1024];
		ICMP_HDR*r_icmp_hdr;
		IP_HDR r_ip_hdr;
		int nLen=sizeof(srcAddr);
		
		//发送成功后，开始接收
  		nRet=::recvfrom(sRaw,recvBuff,1024,0,(SOCKADDR*)&srcAddr,&nLen);//这里可以不设置srcaddr 地址 就会收到所有的ICMP 封包
  
  		if (nRet==SOCKET_ERROR)
  		{
  			if(::WSAGetLastError() == WSAETIMEDOUT)
  			{
  				printf(" timed out\n");
  				continue;
  			}
  			printf("recvfrom() failed: %d\n", ::WSAGetLastError());
  			return -1;
  		}
  
  		// 下面开始解析接收到的ICMP封包
  		int nTick = ::GetTickCount();
  		if (nRet<sizeof(IP_HDR)+sizeof(ICMP_HDR))
  		{
  			printf("Too few bytes from %s \n",nRet, ::inet_ntoa(srcAddr.sin_addr));
  		}
  		// 接收到的数据中包含IP头，IP头大小为20个字节，所以加20得到ICMP头
  		r_icmp_hdr=(ICMP_HDR*)(recvBuff+sizeof(IP_HDR));
  		if (r_icmp_hdr->icmp_type!=0)
  		{
  			//printf("noecho type %d recvd \n",r_icmp_hdr->icmp_type);
			//continue;
  		}
  		if(r_icmp_hdr->icmp_id != ::GetCurrentProcessId())
  		{
  			printf("not CurrProcess\n");
  		}
  		printf("%d bytes from %s:", nRet, inet_ntoa(srcAddr.sin_addr));
  		printf("icmp_seq = %d. ", r_icmp_hdr->icmp_sequence);
  		//printf(" time: %d ms", nTick - r_icmp_hdr->icmp_timestamp);
  		printf("\n");
		::Sleep(1000);
	}
	return 0;
}

//  Traceroute 利用的办法是发送一个 UDP 封包到目的地址， 递加 TTL 值。初始情况下， TTL
// 	的值是 1，意味着 UDP 封包到达第一个路由器时 TTL 将终止。这个终止会促使路由器产生
// 	一个 ICMP 超时封包。然后初始的 TTL 值再加 1，再发送这个 UDP 封包，这样， UDP 封包
// 	将到达一个更远的路由器， ICMP 超时封包会再次被发回。收集每个 ICMP 消息便可以得到
// 	封包所经过的路由器。一旦 TTL 增加得足够大，以至到达了终端， ICMP 端口不可达消息
// 	多半会被返回，原因是在接收端没有进程等待这个消息。
int TraceRoute()
{
		//需要创建两个套接字，一个用于接收ICMP超时封包的原始套接字
		//另一个用于发送TTL不断增加的UDP封包的套接字
	char szHostName[]="www.baidu.com";
	char szDestIP[32]="";
	HOSTENT* pHostent=::gethostbyname(szHostName);

	//获取远程地址
	in_addr  addr;
	if(pHostent!=NULL)
	{
		memcpy(&addr,pHostent->h_addr_list[0],pHostent->h_length);
		memcpy(szDestIP,::inet_ntoa(addr),32);;
		printf("IP地址： %s\n", szDestIP);
	}
	// 创建用于接收 ICMP 封包的原始套接字，绑定到本地端口
	SOCKET sRaw = ::socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	SOCKADDR_IN sockAddr;
	sockAddr.sin_family=AF_INET;
	sockAddr.sin_port=htons(0);
	sockAddr.sin_addr.S_un.S_addr=INADDR_ANY;
	if (::bind(sRaw,(SOCKADDR*)&sockAddr,sizeof(SOCKADDR_IN))==SOCKET_ERROR)
	{
		printf("bind error\n");
		return -1;
	}
	SetTimeout(sRaw,5*1000,true);
	SetTimeout(sRaw,5*1000,false);

	//创建用于发送UDP 封包的套接字
	SOCKET sSend=::socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN destAddr;
	destAddr.sin_family=AF_INET;
	destAddr.sin_port=htons(22);
	destAddr.sin_addr=addr;
	int nTTL=1;
	int nRet;
	ICMP_HDR *pIcmpHdr;
	int nTick;
	SOCKADDR_IN recvAddr;
	char recvBuf[1024];
	while(1)
	{
		// 设置UDP封包的TTL值
		SetTTL(sSend, nTTL);
		nTick = ::GetTickCount();

		// 发送这个UDP封包
		nRet = ::sendto(sSend, "Hello", 5, 0, (sockaddr*)&destAddr, sizeof(destAddr));
		if(nRet == SOCKET_ERROR)
		{
			printf(" sendto() failed \n");
			break;
		}
		// 等待接收路由器返回的ICMP报文
		int nLen = sizeof(recvAddr);
		nRet = ::recvfrom(sRaw, recvBuf, 1024, 0, (sockaddr*)&recvAddr, &nLen);
		if(nRet == SOCKET_ERROR)
		{
			if(::WSAGetLastError() == WSAETIMEDOUT)
			{
				printf(" time out \n");
				break;
			}
			else
			{
				printf(" recvfrom() failed \n");
				break;
			}
		}
		++nTTL;
		// 解析接收到的 ICMP 数据
	}
	return 1;
}
int TraceRoute2()
{
	char szHostName[]="www.baidu.com";
	char szDestIP[32]="";
	HOSTENT* pHostent=::gethostbyname(szHostName);

	//获取远程地址
	in_addr  addr;
	if(pHostent!=NULL)
	{
		memcpy(&addr,pHostent->h_addr_list[0],pHostent->h_length);
		memcpy(szDestIP,::inet_ntoa(addr),32);;
		printf("IP地址： %s\n", szDestIP);
	}
	// 创建用于接收 ICMP 封包的原始套接字，绑定到本地端口
	SOCKET sRaw = ::socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	SOCKADDR_IN destAddr;
	destAddr.sin_family=AF_INET;
	destAddr.sin_port=htons(0);
	destAddr.sin_addr=addr;
	char sndBuff[sizeof(ICMP_HDR)+32];
	ICMP_HDR* pIcmpHdr=(ICMP_HDR*)sndBuff;

	char recvBuff[1024]={0};
	int nSeq=0;
	int icmpcount=0;
	while(nSeq<255)
	{
		memset(sndBuff,0,sizeof(sndBuff));
		pIcmpHdr->icmp_type=8;
		pIcmpHdr->icmp_code=0;
		pIcmpHdr->icmp_id=::GetCurrentProcessId();
		pIcmpHdr->icmp_sequence=nSeq;
		char* pData=&sndBuff[sizeof(ICMP_HDR)];
		int nLen1=sizeof(pData);
		memcpy(pData,"I am Hacker",nLen1);
		pIcmpHdr->icmp_checksum=checksum((USHORT*)sndBuff,sizeof(ICMP_HDR)+32);

		//设置TTL生存周期
		int nRet=setsockopt(sRaw,IPPROTO_IP,IP_TTL,(char*)&nSeq,sizeof(nSeq));
		if (nRet==SOCKET_ERROR)
		{
			printf("setsockopt(TTL) error");
		}
		::sendto(sRaw,sndBuff,sizeof(sndBuff),0,(SOCKADDR*)&destAddr,sizeof(destAddr));

		//接收ICMP封包
		SOCKADDR_IN fromAddr;
		int nLen=sizeof(SOCKADDR);
		fromAddr.sin_family=AF_INET;
		int nRecv=recvfrom(sRaw,recvBuff,1024,0,(SOCKADDR*)&fromAddr,&nLen);
		if(nRecv==SOCKET_ERROR)  
		{  
			if(WSAGetLastError()==WSAETIMEDOUT)  
			{  
				printf("recvfrom() TimeOut:%d\n",WSAGetLastError());  
				continue;  
			}  
			printf("recvfrom() failed:%d\n",WSAGetLastError());  
			return -1;  
		} 
		//前20是Ip头
		IP_HDR* pIpHdrRecv=(IP_HDR*)recvBuff;

		ICMP_HDR* pIcmpHdrRecv=(ICMP_HDR*)&recvBuff[20];
		//判断接收的ICMP报文是否为超时报文   
		if(pIcmpHdrRecv->icmp_type==11&&pIcmpHdrRecv->icmp_code==0)  
		{  
			printf("%2d:        %-15s       \n",icmpcount++,inet_ntoa(fromAddr.sin_addr));      
			//return 0;  
		}  
		//判断接收的ICMP报文是否为回复报文   
		else if(pIcmpHdrRecv->icmp_type==0&&pIcmpHdrRecv->icmp_id==GetCurrentProcessId())  
		{  
			printf("%2d:        %-15s       \n",icmpcount++,inet_ntoa(fromAddr.sin_addr));  
			printf("Trace complete!\n");  
			return 1;  
		}  
		//其他类型,表示不可达   
		else  
		{  
			printf("%2d:        Destination host is unreachable!\n",icmpcount++);  
			//return 1;  
		}  
		//Sleep(1000);
		nSeq++;
	}
}
typedef struct _IPHeader // 20 个字节
{ 
	UCHAR iphVerLen; // 版本号和头长度（各占 4 位）
	UCHAR ipTOS; // 服务类型
	USHORT ipLength; // 封包总长度，即整个 IP 报的长度
	USHORT ipID; // 封包标识，惟一标识发送的每一个数据报
	USHORT ipFlags; // 标志
	UCHAR ipTTL; // 生存时间，就是 TTL
	UCHAR ipProtocol; // 协议，可能是 TCP、 UDP、 ICMP 等
	USHORT ipChecksum; // 校验和
	ULONG ipSource; // 源 IP 地址
	ULONG ipDestination; // 目标 IP 地址
} IPHeader, *PIPHeader;
//TCP header

typedef struct _TCPHeader
{
	USHORT	sourcePort;	//16位源端口
	USHORT	destPort;	//目的端口
	ULONG	seqNum;		//32位序列号
	ULONG	ackNum;		//32位确认号
	UCHAR	dataoffset;	//4位首部长度/6位保留字
	UCHAR	flags;		//6位标志
	USHORT	windows;	//16窗口大小
	USHORT	checksum;	//16位校验和
	USHORT	urgentPointer;	//16位紧急数据偏移量
}TCPHeader,*PTCPheader;

void DecodeIPPacket(char *pData)
{ 
	IPHeader *pIPHdr = (IPHeader*)pData;
	in_addr source, dest;
	char szSourceIp[32], szDestIp[32];
	printf("-------------------------------\n");
	// 从 IP 头中取出源 IP 地址和目的 IP 地址
	source.S_un.S_addr = pIPHdr->ipSource;
	dest.S_un.S_addr = pIPHdr->ipDestination;
	strcpy(szSourceIp, ::inet_ntoa(source));
	strcpy(szDestIp, ::inet_ntoa(dest));
	printf(" %s -> %s \n", szSourceIp, szDestIp);
	// IP 头长度
	int nHeaderLen = (pIPHdr->iphVerLen & 0xf) * sizeof(ULONG);
	switch(pIPHdr->ipProtocol)
	{
	case IPPROTO_TCP: // TCP
		//DecodeTCPPacket(pData + nHeaderLen);
		printf("this is TCP\n");
		break;
	case IPPROTO_UDP:
		printf("this is UDP\n");
		break;
	case IPPROTO_ICMP:
		printf("this is ICMP\n");
		break;
	}
}

#define SIO_RCVALL  (IOC_IN|IOC_VENDOR|1)
//局域网嗅探器
int Sniffer()
{
	//创建原始套接字
	SOCKET sRaw=socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	//获取本地IP
	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct hostent *pHost;
	gethostname(szHostName, 56);
	if((pHost = gethostbyname((char*)szHostName)) == NULL)
		return 1;
	// 在调用 ioctl 之前，套接字必须绑定
	addr_in.sin_family=AF_INET;
	addr_in.sin_port=htons(0);
	memcpy(&addr_in.sin_addr.S_un.S_addr,pHost->h_addr_list[0],pHost->h_length);
	printf("Binding to interFace：%s\n",::inet_ntoa(addr_in.sin_addr));
	if (::bind(sRaw,(SOCKADDR*)&addr_in,sizeof(addr_in))==SOCKET_ERROR)
	{
		printf("bind error\n");
	}
	//设置SIO_RCVALL控制代码
	BOOL flag =TRUE ;
	setsockopt(sRaw, IPPROTO_IP, IP_HDRINCL, (char*)&flag, sizeof(flag));
	DWORD dwValue=1;
	if(ioctlsocket(sRaw,SIO_RCVALL,&dwValue)!=0)//设置网卡为混杂模式
	{
		printf("ioctl error\n");
		int error=WSAGetLastError();
		printf("%d\n",error );
	}
	int nRet=WSAGetLastError();
	// 开始接收封包
	char buff[1024];
	
	while(TRUE)
	{ 
		nRet = recv(sRaw, buff, 1024, 0);
		if(nRet > 0)
		{ 
			DecodeIPPacket(buff); 
		}
	}
	closesocket(sRaw);
}

int main ()
{
	GetIP();
	GetMac();
	//ICMP 实现Ping命令
	//ICMP();

	//路由节点跟踪
	//TraceRoute();
	//TraceRoute2();
	Sniffer();
	system("pause");
	return 1;
}