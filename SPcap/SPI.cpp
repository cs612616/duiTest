#include "StdAfx.h"
#include "SPI.h"


SPI::SPI(void)
{
	m_pProtocolInfo=NULL;
	m_nTotalProtocol=0;
}


SPI::~SPI(void)
{
}


int SPI::EnumProtocol()
{
	DWORD dwLen=0;
	//取得缓冲区长度
	//使用WSAEnumProtocols仅能枚举基础协议和协议链，不能枚举分层协议    Winsock API接口
	int nRet=::WSAEnumProtocols(NULL,m_pProtocolInfo,&dwLen);
	if (nRet==SOCKET_ERROR)
	{
		if((nRet=::WSAGetLastError()) != WSAENOBUFS)
			return NULL;
	}
	// 申请缓冲区，再次调用 WSAEnumProtocols 函数
	m_pProtocolInfo = (LPWSAPROTOCOL_INFO)::GlobalAlloc(GPTR, dwLen);
	m_nTotalProtocol = ::WSAEnumProtocols(NULL, m_pProtocolInfo, &dwLen);
	return 1;
}

void SPI::PrintProtocol()
{
	if (m_pProtocolInfo!=NULL)
	{
		for (int i=0;i<m_nTotalProtocol;i++)
		{
			// 打印出各个提供者的协议信息
			for(int i=0; i<m_nTotalProtocol; i++)
			{ 
				TCHAR szBuff[1024];
				wsprintf(szBuff,_T(" Protocol: %s \n"), m_pProtocolInfo[i].szProtocol);
				wprintf(_T("%s"),szBuff);
				wprintf(_T(" CatalogEntryId: %d ChainLen: %d \n\n"),
				m_pProtocolInfo[i].dwCatalogEntryId, m_pProtocolInfo[i].ProtocolChain.ChainLen);
			}
		}
	}
}
void  SPI::FreeProtocol()
{
	::GlobalFree(m_pProtocolInfo);
}



int SPI::SPIEnumProtocol()
{
	DWORD dwLen=0;
	int nError=0;

	int nRet=::WSCEnumProtocols(NULL,m_pProtocolInfo,&dwLen,&nError);
	if (nRet==SOCKET_ERROR)
	{
		if(nError != WSAENOBUFS)
			return NULL;
	}
	// 申请缓冲区，再次调用 WSAEnumProtocols 函数
	m_pProtocolInfo = (LPWSAPROTOCOL_INFO)::GlobalAlloc(GPTR, dwLen);
	m_nTotalProtocol = ::WSCEnumProtocols(NULL, m_pProtocolInfo, &dwLen,&nError);
	return 1;
}
void SPI::SPIPrintProtocol()
{
	if (m_pProtocolInfo!=NULL)
	{
		// 打印出各个提供者的协议信息
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
void SPI::SPiFreeProtocol()
{
	::GlobalFree(m_pProtocolInfo);
}


//安装LSP函数
BOOL SPI::SPIInstallProtocol(TCHAR *szDLLPathName)
{
	TCHAR szLSPName[] = _T("PhoenixLSP");
	LPWSAPROTOCOL_INFO originalProtocolInfo[3];
	DWORD	dwOrigCataLogId[3];
	return 0;
}