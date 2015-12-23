// LSPDLL.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#ifndef UNICODE
#define UNICODE
#endif
#define  DLL_EXPORT		extern "C" __declspec(dllexport)
#include <stdio.h>
#include <WinSock2.h>
#include <Ws2spi.h>
#include <Windows.h>
#include <tchar.h>

#pragma comment(lib, "Ws2_32.lib")

#include "LOG.h"
WSPUPCALLTABLE g_pUpCallTable;//�ϲ㺯���б����LSP�������Լ���α�������ʹ����������б�  /* initialize upcallTable with function pointers */


//���������� SPI ���������� LSP �ķ��ɱ���lpProcTable �����������������ɱ��
//WSPPROC_TABLE �ṹ�����˱����� LSP ����ʵ�ֵĺ�����
//�� DLL ��ʵ�ֺ������еĺ������ܼ򵥣��ڴ��������£�ֻ��Ҫ�����²��ṩ�ߵ���
//�Ķ�Ӧ�������ɡ�
WSPPROC_TABLE g_NextProcTable;//�²㺯���б�	/* allocate memory for the ProcTable */
TCHAR g_szCurrentApp[MAX_PATH];//��ǰ���ñ�DLL�ĳ��������

LPWSAPROTOCOL_INFO SPIGetProvider(int* pTotalProtos);
void SPIFreeProvider(LPWSAPROTOCOL_INFO pProtoInfo);
int WSPAPI WSPSendTo(
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesSent,
	DWORD			dwFlags,
	const struct sockaddr FAR * lpTo,
	int				iTolen,
	LPWSAOVERLAPPED	lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
	);



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


//WSPStartup�Ѿ���ws2spi.h �����˵�����ʽ(����������Ե��õ�)����vs2005û�ж��壬��Ҫ�Լ�����dllexport)
int WSPAPI WSPStartup(
	WORD wVersionRequested,				// �����߿���ʹ�õ� Winsock SPI ����߰汾�ţ����ֽ���С�汾��
	LPWSPDATA lpWSPData,				// ָ��һ�� WSPDATA �ṹ������ȡ�� Winsock �����ṩ�ߵ���ϸ��Ϣ
	LPWSAPROTOCOL_INFO lpProtocolInfo,	// ָ��һ�� WSAPROTOCOL_INFO �ṹ�� ����ָ����õ���Э�������
	WSPUPCALLTABLE UpcallTable,			// Ws2_32.dll �ṩ�����ϵ���ת���ĺ�����ṹ
	LPWSPPROC_TABLE lpProcTable			// ָ�� SPI ������ṹ��ָ�룬�������� 30 �� SPI ������
	)
{
	AGENTLOG("  WSPStartup...  %s \n", g_szCurrentApp);
	//chainlen=0 ����ֲ�Э���ṩ�ߣ�1�������Э�飨��TCP��UDP����>1����Э����
	if(lpProtocolInfo->ProtocolChain.ChainLen<=1)
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	// �������ϵ��õĺ�����ָ�루�������ǲ�ʹ������
	g_pUpCallTable = UpcallTable;
	// ö��Э�飬�ҵ��²�Э���WSAPROTOCOL_INFOW�ṹ	
	WSAPROTOCOL_INFO	NextProtocolInfo;
	int nTotalProtos;
	LPWSAPROTOCOL_INFO pProtoInfo = SPIGetProvider(&nTotalProtos);//��ȡ���е�Э��
	// �²����ID	
	DWORD dwBaseEntryId = lpProtocolInfo->ProtocolChain.ChainEntries[1];
	int i=0;
	for(i=0; i<nTotalProtos; i++)
	{
		if(pProtoInfo[i].dwCatalogEntryId == dwBaseEntryId)
		{
			memcpy(&NextProtocolInfo, &pProtoInfo[i], sizeof(NextProtocolInfo));
			break;
		}
	}
	if(i >= nTotalProtos)
	{
		AGENTLOG(" WSPStartup:	Can not find underlying protocol \n");//û���ҵ���Ӧ�Ļ���Э��
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²�Э���DLL
	int nError;
	TCHAR szBaseProviderDll[MAX_PATH];
	int nLen = MAX_PATH;
	// ȡ���²��ṩ����DLL·��
	if(::WSCGetProviderPath(&NextProtocolInfo.ProviderId, szBaseProviderDll, &nLen, &nError) == SOCKET_ERROR)
	{
		AGENTLOG(" WSPStartup: WSCGetProviderPath() failed %d \n", nError);
		return WSAEPROVIDERFAILEDINIT;
	}
	if(!::ExpandEnvironmentStrings(szBaseProviderDll, szBaseProviderDll, MAX_PATH))
	{
		AGENTLOG(" WSPStartup:  ExpandEnvironmentStrings() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²��ṩ����
	HMODULE hModule = ::LoadLibrary(szBaseProviderDll);
	if(hModule == NULL)
	{
		AGENTLOG(" WSPStartup:  LoadLibrary() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}

	// �����²��ṩ�����WSPStartup����
	LPWSPSTARTUP  pfnWSPStartup = NULL;
	pfnWSPStartup = (LPWSPSTARTUP)::GetProcAddress(hModule, "WSPStartup");
	if(pfnWSPStartup == NULL)
	{
		AGENTLOG(" WSPStartup:  GetProcAddress() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	// �����²��ṩ�����WSPStartup����
	LPWSAPROTOCOL_INFOW pInfo = lpProtocolInfo;
	if(NextProtocolInfo.ProtocolChain.ChainLen == BASE_PROTOCOL)
		pInfo = &NextProtocolInfo;

	int nRet = pfnWSPStartup(wVersionRequested, lpWSPData, pInfo, UpcallTable, lpProcTable);
	if(nRet != ERROR_SUCCESS)
	{
		AGENTLOG(" WSPStartup:  underlying provider's WSPStartup() failed %d \n", nRet);
		return nRet;
	}
	g_NextProcTable=*lpProcTable;
	// �����²��ṩ�ߵĺ�����
	// �޸Ĵ��ݸ��ϲ�ĺ�����Hook����Ȥ�ĺ�����������Ϊʾ������Hook��WSPSendTo����
	// ��������Hook������������WSPSocket��WSPCloseSocket��WSPConnect��
	lpProcTable->lpWSPSendTo = WSPSendTo;

	SPIFreeProvider(pProtoInfo);
	return nRet;
	return 1;
}
int WSPAPI WSPSendTo(
	SOCKET			s,
	LPWSABUF		lpBuffers,
	DWORD			dwBufferCount,
	LPDWORD			lpNumberOfBytesSent,
	DWORD			dwFlags,
	const struct sockaddr FAR * lpTo,
	int				iTolen,
	LPWSAOVERLAPPED	lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID	lpThreadId,
	LPINT			lpErrno
	)
{
	AGENTLOG("In User Level I hook you UDP protocol,sendTo Function!!!\n");
	return g_NextProcTable.lpWSPSendTo(s,lpBuffers,dwBufferCount,lpNumberOfBytesSent,dwFlags,lpTo,iTolen,lpOverlapped,lpCompletionRoutine,lpThreadId,lpErrno);
}


class CConsole
{
	CConsole()
	{
		AllocConsole();
		freopen("CONOUT$","w+t",stdout);
		freopen("CONIN$","r+t",stdin);
	}
	~CConsole()
	{
		fclose(stdout);
		fclose(stdin);
		FreeConsole();
	}
};


BOOL APIENTRY DllMain( HMODULE hModule,
DWORD  ul_reason_for_call,
LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// ȡ����ģ�������
		::GetModuleFileName(NULL, g_szCurrentApp, MAX_PATH);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

