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
void GetIP()
{
	char szHost[256];
	//��ȡ��������
	::gethostname(szHost,256);
	//ͨ���������õ���ַ��Ϣ
	hostent *pHost=::gethostbyname(szHost);
	//��ӡ��ַ
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
		printf("����IP��ַ�� %s\n", szIp);
	}
}
void GetMac()
{
	u_char ucLocalMac[6]; // ���� MAC ��
	DWORD dwGatewayIP; // ���� IP ��ַ
	DWORD dwLocalIP; // ���� IP ��ַ
	DWORD dwMask; // ��������
	PIP_ADAPTER_INFO pAdapterInfo = NULL;
	ULONG ulLen = 0;
	// Ϊ�������ṹ�����ڴ�
	::GetAdaptersInfo(pAdapterInfo,&ulLen);
	pAdapterInfo = (PIP_ADAPTER_INFO)::GlobalAlloc(GPTR, ulLen);
	// ȡ�ñ����������ṹ��Ϣ
	if(::GetAdaptersInfo(pAdapterInfo,&ulLen) == ERROR_SUCCESS)
	{
		while(pAdapterInfo != NULL)
		{ 
			memcpy(ucLocalMac, pAdapterInfo->Address, 6);
			dwGatewayIP = ::inet_addr(pAdapterInfo->GatewayList.IpAddress.String);
			dwLocalIP = ::inet_addr(pAdapterInfo->IpAddressList.IpAddress.String);
			dwMask = ::inet_addr(pAdapterInfo->IpAddressList.IpMask.String);
			pAdapterInfo=pAdapterInfo->Next;
			printf(" \n -------------------- ����������Ϣ -----------------------\n\n");
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

//������У���
USHORT checksum(USHORT* buff, int size)
{
	unsigned long cksum = 0;
	while(size>1)
	{
		cksum += *buff++;
		size -= sizeof(USHORT);
	}
	// ������
	if(size)
	{
		cksum += *(UCHAR*)buff;
	}
	// ��32λ��chsum��16λ�͵�16λ��ӣ�Ȼ��ȡ��
	cksum = (cksum >> 16) + (cksum & 0xffff);//��16λ + ��16λ
	cksum += (cksum >> 16);					 //+��λ
	return (USHORT)(~cksum);				 //ȡ��
}
BOOL SetTimeout(SOCKET s, int nTime, BOOL bRecv)
{
	int ret = ::setsockopt(s, SOL_SOCKET, 
		bRecv ? SO_RCVTIMEO : SO_SNDTIMEO, (char*)&nTime, sizeof(nTime));
	return ret != SOCKET_ERROR;
}
//icmp����( ICMP����������Ӧ���ĸ�ʽ����ICMP�����ݽṹ)
typedef struct icmp_hdr
{
	unsigned char   icmp_type;		// ��Ϣ����
	unsigned char   icmp_code;		// ����
	unsigned short  icmp_checksum;	// У���
	// �����ǻ���ͷ
	unsigned short  icmp_id;		// ����Ωһ��ʶ�������ID�ţ�ͨ������Ϊ����ID
	unsigned short  icmp_sequence;	// ���к�
	//unsigned long   icmp_timestamp; // ʱ���
} ICMP_HDR, *PICMP_HDR;

//ip ͷ
typedef struct _IP_hdr // 20 ���ֽ�
{ 
	unsigned char iphLen:4;         //length of header
	unsigned char iphVer:4; // �汾�ź�ͷ���ȣ���ռ 4 λ��
	UCHAR ipTOS;			// ��������
	USHORT ipLength;		// ����ܳ��ȣ������� IP ���ĳ���
	USHORT ipID;			// �����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ�
	USHORT ipFlags;			// ��־
	UCHAR ipTTL;			// ����ʱ�䣬���� TTL
	UCHAR ipProtocol;		// Э�飬������ TCP�� UDP�� ICMP ��
	USHORT ipChecksum;		// У���
	ULONG ipSource;			// Դ IP ��ַ
	ULONG ipDestination;	// Ŀ�� IP ��ַ
} IP_HDR, *PIP_HDR;

// IP option header - use with socket option IP_OPTIONS	
// ip��ѡͷ
typedef struct _ipoptionhdr
{
	unsigned char code;        // Option type
	unsigned char len;         // Length of option hdr
	unsigned char ptr;         // Offset into options
	unsigned long addr[9];     // List of IP addrs
} IpOptionHeader;
//������ip��ѡͷ�Ĵ��ڣ���ô ipͷ�ĳ��ȿɱ�

BOOL SetTTL(SOCKET s, int nValue)
{
	int ret = ::setsockopt(s, IPPROTO_IP, IP_TTL, (char*)&nValue, sizeof(nValue));
	return ret != SOCKET_ERROR;
}
int ICMP()
{
	//Ŀ���ַ
	char *szDestIp = "61.135.169.121";
	SOCKADDR_IN destAddr;
	//����ԭʼ�׽���
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
	//icmp���
	ICMP_HDR* pI_hdr;
	//��������
	char sndBuff[sizeof(ICMP_HDR)+32];
	pI_hdr=(ICMP_HDR*)sndBuff;
	//ѭ����������
	int seq=0;
	int nTTL=1;
	while(seq<=4)
	{
		SetTTL(sRaw, nTTL);
		nTTL++;
		int nRet=0;
		memset(sndBuff,0,sizeof(ICMP_HDR)+32);
		pI_hdr->icmp_type = 8;	// ����һ��ICMP����
		pI_hdr->icmp_code = 0;
		pI_hdr->icmp_id=::GetCurrentProcessId();
		pI_hdr->icmp_sequence=seq++	;
		//pI_hdr->icmp_timestamp=::GetTickCount();
		memset(sndBuff+sizeof(ICMP_HDR),'E',32);
		//���У��ͼ�����󣬶Է��Զ���������Ӧ�ð�У��ͼ�����ڷ�������һ��
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
		
		//���ͳɹ��󣬿�ʼ����
  		nRet=::recvfrom(sRaw,recvBuff,1024,0,(SOCKADDR*)&srcAddr,&nLen);//������Բ�����srcaddr ��ַ �ͻ��յ����е�ICMP ���
  
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
  
  		// ���濪ʼ�������յ���ICMP���
  		int nTick = ::GetTickCount();
  		if (nRet<sizeof(IP_HDR)+sizeof(ICMP_HDR))
  		{
  			printf("Too few bytes from %s \n",nRet, ::inet_ntoa(srcAddr.sin_addr));
  		}
  		// ���յ��������а���IPͷ��IPͷ��СΪ20���ֽڣ����Լ�20�õ�ICMPͷ
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

//  Traceroute ���õİ취�Ƿ���һ�� UDP �����Ŀ�ĵ�ַ�� �ݼ� TTL ֵ����ʼ����£� TTL
// 	��ֵ�� 1����ζ�� UDP ��������һ��·����ʱ TTL ����ֹ�������ֹ���ʹ·��������
// 	һ�� ICMP ��ʱ�����Ȼ���ʼ�� TTL ֵ�ټ� 1���ٷ������ UDP ����������� UDP ���
// 	������һ����Զ��·������ ICMP ��ʱ������ٴα����ء��ռ�ÿ�� ICMP ��Ϣ����Եõ�
// 	�����������·������һ�� TTL ���ӵ��㹻�������������նˣ� ICMP �˿ڲ��ɴ���Ϣ
// 	���ᱻ���أ�ԭ�����ڽ��ն�û�н��̵ȴ������Ϣ��
int TraceRoute()
{
		//��Ҫ���������׽��֣�һ�����ڽ���ICMP��ʱ�����ԭʼ�׽���
		//��һ�����ڷ���TTL�������ӵ�UDP������׽���
	char szHostName[]="www.baidu.com";
	char szDestIP[32]="";
	HOSTENT* pHostent=::gethostbyname(szHostName);

	//��ȡԶ�̵�ַ
	in_addr  addr;
	if(pHostent!=NULL)
	{
		memcpy(&addr,pHostent->h_addr_list[0],pHostent->h_length);
		memcpy(szDestIP,::inet_ntoa(addr),32);;
		printf("IP��ַ�� %s\n", szDestIP);
	}
	// �������ڽ��� ICMP �����ԭʼ�׽��֣��󶨵����ض˿�
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

	//�������ڷ���UDP ������׽���
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
		// ����UDP�����TTLֵ
		SetTTL(sSend, nTTL);
		nTick = ::GetTickCount();

		// �������UDP���
		nRet = ::sendto(sSend, "Hello", 5, 0, (sockaddr*)&destAddr, sizeof(destAddr));
		if(nRet == SOCKET_ERROR)
		{
			printf(" sendto() failed \n");
			break;
		}
		// �ȴ�����·�������ص�ICMP����
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
		// �������յ��� ICMP ����
	}
	return 1;
}
int TraceRoute2()
{
	char szHostName[]="www.baidu.com";
	char szDestIP[32]="";
	HOSTENT* pHostent=::gethostbyname(szHostName);

	//��ȡԶ�̵�ַ
	in_addr  addr;
	if(pHostent!=NULL)
	{
		memcpy(&addr,pHostent->h_addr_list[0],pHostent->h_length);
		memcpy(szDestIP,::inet_ntoa(addr),32);;
		printf("IP��ַ�� %s\n", szDestIP);
	}
	// �������ڽ��� ICMP �����ԭʼ�׽��֣��󶨵����ض˿�
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

		//����TTL��������
		int nRet=setsockopt(sRaw,IPPROTO_IP,IP_TTL,(char*)&nSeq,sizeof(nSeq));
		if (nRet==SOCKET_ERROR)
		{
			printf("setsockopt(TTL) error");
		}
		::sendto(sRaw,sndBuff,sizeof(sndBuff),0,(SOCKADDR*)&destAddr,sizeof(destAddr));

		//����ICMP���
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
		//ǰ20��Ipͷ
		IP_HDR* pIpHdrRecv=(IP_HDR*)recvBuff;

		ICMP_HDR* pIcmpHdrRecv=(ICMP_HDR*)&recvBuff[20];
		//�жϽ��յ�ICMP�����Ƿ�Ϊ��ʱ����   
		if(pIcmpHdrRecv->icmp_type==11&&pIcmpHdrRecv->icmp_code==0)  
		{  
			printf("%2d:        %-15s       \n",icmpcount++,inet_ntoa(fromAddr.sin_addr));      
			//return 0;  
		}  
		//�жϽ��յ�ICMP�����Ƿ�Ϊ�ظ�����   
		else if(pIcmpHdrRecv->icmp_type==0&&pIcmpHdrRecv->icmp_id==GetCurrentProcessId())  
		{  
			printf("%2d:        %-15s       \n",icmpcount++,inet_ntoa(fromAddr.sin_addr));  
			printf("Trace complete!\n");  
			return 1;  
		}  
		//��������,��ʾ���ɴ�   
		else  
		{  
			printf("%2d:        Destination host is unreachable!\n",icmpcount++);  
			//return 1;  
		}  
		//Sleep(1000);
		nSeq++;
	}
}
typedef struct _IPHeader // 20 ���ֽ�
{ 
	UCHAR iphVerLen; // �汾�ź�ͷ���ȣ���ռ 4 λ��
	UCHAR ipTOS; // ��������
	USHORT ipLength; // ����ܳ��ȣ������� IP ���ĳ���
	USHORT ipID; // �����ʶ��Ωһ��ʶ���͵�ÿһ�����ݱ�
	USHORT ipFlags; // ��־
	UCHAR ipTTL; // ����ʱ�䣬���� TTL
	UCHAR ipProtocol; // Э�飬������ TCP�� UDP�� ICMP ��
	USHORT ipChecksum; // У���
	ULONG ipSource; // Դ IP ��ַ
	ULONG ipDestination; // Ŀ�� IP ��ַ
} IPHeader, *PIPHeader;
//TCP header

typedef struct _TCPHeader
{
	USHORT	sourcePort;	//16λԴ�˿�
	USHORT	destPort;	//Ŀ�Ķ˿�
	ULONG	seqNum;		//32λ���к�
	ULONG	ackNum;		//32λȷ�Ϻ�
	UCHAR	dataoffset;	//4λ�ײ�����/6λ������
	UCHAR	flags;		//6λ��־
	USHORT	windows;	//16���ڴ�С
	USHORT	checksum;	//16λУ���
	USHORT	urgentPointer;	//16λ��������ƫ����
}TCPHeader,*PTCPheader;

void DecodeIPPacket(char *pData)
{ 
	IPHeader *pIPHdr = (IPHeader*)pData;
	in_addr source, dest;
	char szSourceIp[32], szDestIp[32];
	printf("-------------------------------\n");
	// �� IP ͷ��ȡ��Դ IP ��ַ��Ŀ�� IP ��ַ
	source.S_un.S_addr = pIPHdr->ipSource;
	dest.S_un.S_addr = pIPHdr->ipDestination;
	strcpy(szSourceIp, ::inet_ntoa(source));
	strcpy(szDestIp, ::inet_ntoa(dest));
	printf(" %s -> %s \n", szSourceIp, szDestIp);
	// IP ͷ����
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
//��������̽��
int Sniffer()
{
	//����ԭʼ�׽���
	SOCKET sRaw=socket(AF_INET,SOCK_RAW,IPPROTO_IP);
	//��ȡ����IP
	char szHostName[56];
	SOCKADDR_IN addr_in;
	struct hostent *pHost;
	gethostname(szHostName, 56);
	if((pHost = gethostbyname((char*)szHostName)) == NULL)
		return 1;
	// �ڵ��� ioctl ֮ǰ���׽��ֱ����
	addr_in.sin_family=AF_INET;
	addr_in.sin_port=htons(0);
	memcpy(&addr_in.sin_addr.S_un.S_addr,pHost->h_addr_list[0],pHost->h_length);
	printf("Binding to interFace��%s\n",::inet_ntoa(addr_in.sin_addr));
	if (::bind(sRaw,(SOCKADDR*)&addr_in,sizeof(addr_in))==SOCKET_ERROR)
	{
		printf("bind error\n");
	}
	//����SIO_RCVALL���ƴ���
	BOOL flag =TRUE ;
	setsockopt(sRaw, IPPROTO_IP, IP_HDRINCL, (char*)&flag, sizeof(flag));
	DWORD dwValue=1;
	if(ioctlsocket(sRaw,SIO_RCVALL,&dwValue)!=0)//��������Ϊ����ģʽ
	{
		printf("ioctl error\n");
		int error=WSAGetLastError();
		printf("%d\n",error );
	}
	int nRet=WSAGetLastError();
	// ��ʼ���շ��
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
	//ICMP ʵ��Ping����
	//ICMP();

	//·�ɽڵ����
	//TraceRoute();
	//TraceRoute2();
	Sniffer();
	system("pause");
	return 1;
}