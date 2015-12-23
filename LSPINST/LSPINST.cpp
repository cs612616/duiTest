// LSPINST.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <WinSock2.h>
#include <Ws2spi.h>
#include <Windows.h>
#include <tchar.h>
#include <Sporder.h>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")//WSCWriteProviderOrder


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

GUID ProviderGuid;
LPWSAPROTOCOL_INFO SPIGetProvider(int* pTotalProtos);
void SPIFreeProvider(LPWSAPROTOCOL_INFO pProtoInfo);

LPWSAPROTOCOL_INFO SPIGetProvider(int* pTotalProtos)
{
	DWORD dwLen=0;
	int nError=0;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;
	int nRet=::WSCEnumProtocols(NULL,pProtoInfo,&dwLen,&nError);
	if (nRet==SOCKET_ERROR)
	{
		if(nError != WSAENOBUFS)
			return NULL;
	}
	// ���뻺�������ٴε��� WSAEnumProtocols ����
	pProtoInfo = (LPWSAPROTOCOL_INFO)::GlobalAlloc(GPTR, dwLen);
	*pTotalProtos = ::WSCEnumProtocols(NULL, pProtoInfo, &dwLen, &nError);
	return pProtoInfo;
}
void SPIFreeProvider(LPWSAPROTOCOL_INFO pProtoInfo)
{
	::GlobalFree(pProtoInfo);
}

//��װ���ǵ�LSPdll
BOOL InstallProvider(TCHAR *szDllPathName)
{
	TCHAR szLSPName[]=_T("CSLSP");//���ǵ�LSPЭ������
	int nError=NO_ERROR;
	LPWSAPROTOCOL_INFO pProtoInfo;
	int nProtocols;
	WSAPROTOCOL_INFO UDPLayerInfo,UDPChainInfo;//����Ҫ��װ��UDP�ֲ�Э���Э����
	DWORD dwUdpOriginCatalogId,dwLayeredCatalogId;

	//��winsockĿ¼���ҵ�ԭ����UDPЭ������ṩ�ߣ����ǵ�LSPҪ��װ����֮��
	//ö�����з�������ṩ��
	pProtoInfo=SPIGetProvider(&nProtocols);
	int i=0;
	for (i=0;i<nProtocols;i++)
	{
		if(pProtoInfo[i].iAddressFamily==AF_INET&&pProtoInfo[i].iProtocol==IPPROTO_UDP)
		{
			memcpy(&UDPChainInfo,&pProtoInfo[i],sizeof(UDPChainInfo));
			//ȥ��XP1_IFS_HANDLES��־
			UDPChainInfo.dwServiceFlags1=UDPChainInfo.dwServiceFlags1&~XP1_IFS_HANDLES;
			//����ԭ�������ID
			dwUdpOriginCatalogId=pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	//���Ȱ�װ��Э�飬��ȡһ��winsock�ⰲ�ŵ�Ŀ¼��ţ���dwLayeredCatalogId
	//ֱ��ʹ���²�Э���WSAprotocol_info����
	memcpy(&UDPLayerInfo,&UDPChainInfo,sizeof(UDPLayerInfo));
	//�޸�Э�����ƣ����ͣ�����PFL_HIDDEN��־
	wcscpy(UDPLayerInfo.szProtocol,szLSPName);
	UDPLayerInfo.ProtocolChain.ChainLen=LAYERED_PROTOCOL;
	UDPLayerInfo.dwProviderFlags |=PFL_HIDDEN;
	//��װ

	if(S_FALSE==::CoCreateGuid(&ProviderGuid))
	{
		return -1;
	}
	if (::WSCInstallProvider(&ProviderGuid,szDllPathName,&UDPLayerInfo,1,&nError)==SOCKET_ERROR)//��װһ���Լ��ķֲ�Э��
	{
		return nError;//5Ȩ�޲�������ʹ�ù���Ա����
	}
	//����ö��Э�飬��ȡ�ֲ�Э���Ŀ¼ID��
	SPIFreeProvider(pProtoInfo);
	pProtoInfo=SPIGetProvider(&nProtocols);
	for(i=0;i<nProtocols;i++)
	{
		if(memcmp(&pProtoInfo[i].ProviderId,&ProviderGuid,sizeof(ProviderGuid))==0)
		{
			dwLayeredCatalogId=pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	//��װЭ����
	//�޸�Э�����ƣ�����
	WCHAR wszChainName[WSAPROTOCOL_LEN + 1];
	swprintf(wszChainName, L"%ws over %ws", szLSPName, UDPChainInfo.szProtocol);
	wcscpy(UDPChainInfo.szProtocol, wszChainName);
	if(UDPChainInfo.ProtocolChain.ChainLen == 1)//����Э��
	{
		UDPChainInfo.ProtocolChain.ChainEntries[1] = dwUdpOriginCatalogId;
	}
	else
	{
		for(i=UDPChainInfo.ProtocolChain.ChainLen; i>0 ; i--)
		{
			UDPChainInfo.ProtocolChain.ChainEntries[i] = UDPChainInfo.ProtocolChain.ChainEntries[i-1];
		}
	}
	UDPChainInfo.ProtocolChain.ChainLen ++;
	// �����ǵķֲ�Э�����ڴ�Э�����Ķ���
	UDPChainInfo.ProtocolChain.ChainEntries[0] = dwLayeredCatalogId;
	// ��ȡһ��Guid����װ֮
	GUID ProviderChainGuid;
	if(::CoCreateGuid(&ProviderChainGuid) == S_OK)
	{
		if(::WSCInstallProvider(&ProviderChainGuid, 
			szDllPathName, &UDPChainInfo, 1, &nError) == SOCKET_ERROR)//��װ�Լ���һ��Э����
			return nError;
	}
	else
		return GetLastError();

	// ��������WinsockĿ¼�������ǵ�Э������ǰ
	// ����ö�ٰ�װ��Э��

	SPIFreeProvider(pProtoInfo);
	pProtoInfo=SPIGetProvider(&nProtocols);
	LPDWORD dwIDs;
	dwIDs=(LPDWORD)GlobalAlloc(GPTR,nProtocols*sizeof(DWORD));
	int nIndex=0;
	//������ǵ�Э����
	for(int i=0;i<nProtocols;i++)
	{
		if ((pProtoInfo[i].ProtocolChain.ChainLen>1)&&(pProtoInfo[i].dwCatalogEntryId==dwLayeredCatalogId))
		{
			dwIDs[nIndex++]=pProtoInfo[i].dwCatalogEntryId;
		}
	}
	//�������Э��
	for(i=0; i<nProtocols;i++)
	{
		if((pProtoInfo[i].ProtocolChain.ChainLen <= 1) ||
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] != dwLayeredCatalogId))
			dwIDs[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
	}
	// ��������WinsockĿ¼
	nError = WSCWriteProviderOrder(dwIDs, nProtocols);
	GlobalFree(dwIDs);
	SPIFreeProvider(pProtoInfo);
	return nError;
}
void RemoveProvider()
{ 
	LPWSAPROTOCOL_INFOW pProtoInfo;
	int nProtocols;
	DWORD dwLayeredCatalogId;
	// ����Guidȡ�÷ֲ�Э���Ŀ¼ID��
	pProtoInfo = SPIGetProvider(&nProtocols);
	int nError;
	int i=0;
	for(i=0; i<nProtocols;i++)
	{
		if(memcmp(&ProviderGuid, &pProtoInfo[i].ProviderId, sizeof(ProviderGuid)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	if(i < nProtocols)
	{
		// �Ƴ�Э����
		for(i=0; i<nProtocols;i++)
		{
			if((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
				(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
			{
				::WSCDeinstallProvider(&pProtoInfo[i].ProviderId, &nError);
			}
		}
		// �Ƴ��ֲ�Э��
		::WSCDeinstallProvider(&ProviderGuid, &nError);
	}
}

void send_spi()
{
	SOCKET s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(s == INVALID_SOCKET)
	{
		wprintf(_T("Failed socket() %d \n"), ::WSAGetLastError());
		return ;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4567);
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	char szText[] = " SPI TEST Demo! \r\n";
	::sendto(s, szText, strlen(szText), 0, (sockaddr*)&addr, sizeof(addr));
	::closesocket(s);
}

void SPIPrintProtocol(LPWSAPROTOCOL_INFO m_pProtocolInfo,int m_nTotalProtocol)
{
	if (m_pProtocolInfo!=NULL)
	{
		// ��ӡ�������ṩ�ߵ�Э����Ϣ
		for(int i=0; i<m_nTotalProtocol; i++)
		{ 
			TCHAR szBuff[1024];
			int nRet=0;
			WCHAR GuidString[40] = { 0 };
			wprintf(L"Winsock Catalog Provider Entry #%d\n", i);
			wprintf
				(L"----------------------------------------------------------\n");
			wprintf(L"Entry type:\t\t\t ");
			if (m_pProtocolInfo[i].ProtocolChain.ChainLen == 1)
				wprintf(L"Base Service Provider\n");
			else
				wprintf(L"Layered Chain Entry\n");

			wprintf(L"Protocol:\t\t\t %ws\n", m_pProtocolInfo[i].szProtocol);

			nRet =
				StringFromGUID2(m_pProtocolInfo[i].ProviderId,
				(LPOLESTR) & GuidString, 39);
			if (nRet == 0)
				wprintf(L"StringFromGUID2 failed\n");
			else
				wprintf(L"Provider ID:\t\t\t %ws\n", GuidString);

			wprintf(L"Catalog Entry ID:\t\t %u\n",
				m_pProtocolInfo[i].dwCatalogEntryId);

			wprintf(L"Version:\t\t\t %d\n", m_pProtocolInfo[i].iVersion);

			wprintf(L"Address Family:\t\t\t %d\n",
				m_pProtocolInfo[i].iAddressFamily);
			wprintf(L"Max Socket Address Length:\t %d\n",
				m_pProtocolInfo[i].iMaxSockAddr);
			wprintf(L"Min Socket Address Length:\t %d\n",
				m_pProtocolInfo[i].iMinSockAddr);

			wprintf(L"Socket Type:\t\t\t %d\n", m_pProtocolInfo[i].iSocketType);
			wprintf(L"Socket Protocol:\t\t %d\n", m_pProtocolInfo[i].iProtocol);
			wprintf(L"Socket Protocol Max Offset:\t %d\n",
				m_pProtocolInfo[i].iProtocolMaxOffset);

			wprintf(L"Network Byte Order:\t\t %d\n",
				m_pProtocolInfo[i].iNetworkByteOrder);
			wprintf(L"Security Scheme:\t\t %d\n",
				m_pProtocolInfo[i].iSecurityScheme);
			wprintf(L"Max Message Size:\t\t %u\n", m_pProtocolInfo[i].dwMessageSize);

			wprintf(L"ServiceFlags1:\t\t\t 0x%x\n",
				m_pProtocolInfo[i].dwServiceFlags1);
			wprintf(L"ServiceFlags2:\t\t\t 0x%x\n",
				m_pProtocolInfo[i].dwServiceFlags2);
			wprintf(L"ServiceFlags3:\t\t\t 0x%x\n",
				m_pProtocolInfo[i].dwServiceFlags3);
			wprintf(L"ServiceFlags4:\t\t\t 0x%x\n",
				m_pProtocolInfo[i].dwServiceFlags4);
			wprintf(L"ProviderFlags:\t\t\t 0x%x\n",
				m_pProtocolInfo[i].dwProviderFlags);

			wprintf(L"Protocol Chain length:\t\t %d\n",
				m_pProtocolInfo[i].ProtocolChain.ChainLen);
			wprintf(L"\n");
		}
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	if(InstallProvider(L"LSPDLL.dll") == ERROR_SUCCESS)
		wprintf(_T(" Install successully \n"));
	else
		wprintf(_T(" Install failed \n"));
	send_spi();
	LPWSAPROTOCOL_INFO pProtocolInfo=NULL;
	int nTotal;
	pProtocolInfo=SPIGetProvider(&nTotal);
	SPIPrintProtocol(pProtocolInfo,nTotal);
	RemoveProvider();
// 	HMODULE hDLL=LoadLibrary(_T("LSPDLL.dll"));
// 	if(!hDLL)
// 	{
// 		return -1;
// 	}
// 	LPWSPSTARTUP lpWSPStartup;
// 	lpWSPStartup=(LPWSPSTARTUP)GetProcAddress(hDLL,"WSPStartup");
	system("pause");
	return 0;
}

