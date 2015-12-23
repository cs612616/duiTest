// LSPDLL.cpp : 定义 DLL 应用程序的导出函数。
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
WSPUPCALLTABLE g_pUpCallTable;//上层函数列表。如果LSP创建了自己的伪句柄，才使用这个函数列表  /* initialize upcallTable with function pointers */


//所有其他的 SPI 函数都经由 LSP 的分派表——lpProcTable 参数导出。描述分派表的
//WSPPROC_TABLE 结构定义了必须在 LSP 必须实现的函数，
//在 DLL 中实现函数表中的函数都很简单，在大多数情况下，只需要调用下层提供者导出
//的对应函数即可。
WSPPROC_TABLE g_NextProcTable;//下层函数列表	/* allocate memory for the ProcTable */
TCHAR g_szCurrentApp[MAX_PATH];//当前调用本DLL的程序的名称

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
	// 申请缓冲区，再次调用 WSAEnumProtocols 函数
	pProtoInfo = (LPWSAPROTOCOL_INFO)::GlobalAlloc(GPTR, dwLen);
	*pTotalProtos = ::WSCEnumProtocols(NULL, pProtoInfo, &dwLen, &nError);
	return pProtoInfo;
}
void SPIFreeProvider(LPWSAPROTOCOL_INFO pProtoInfo)
{
	::GlobalFree(pProtoInfo);
}


//WSPStartup已经在ws2spi.h 定义了导出形式(其他程序可以调用的)。（vs2005没有定义，需要自己定义dllexport)
int WSPAPI WSPStartup(
	WORD wVersionRequested,				// 调用者可以使用的 Winsock SPI 的最高版本号，高字节是小版本号
	LPWSPDATA lpWSPData,				// 指向一个 WSPDATA 结构，用于取得 Winsock 服务提供者的详细信息
	LPWSAPROTOCOL_INFO lpProtocolInfo,	// 指向一个 WSAPROTOCOL_INFO 结构， 用来指定想得到的协议的特征
	WSPUPCALLTABLE UpcallTable,			// Ws2_32.dll 提供的向上调用转发的函数表结构
	LPWSPPROC_TABLE lpProcTable			// 指向 SPI 函数表结构的指针，用来返回 30 个 SPI 服务函数
	)
{
	AGENTLOG("  WSPStartup...  %s \n", g_szCurrentApp);
	//chainlen=0 代表分层协议提供者，1代表基础协议（如TCP，UDP），>1代表协议链
	if(lpProtocolInfo->ProtocolChain.ChainLen<=1)
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	// 保存向上调用的函数表指针（这里我们不使用它）
	g_pUpCallTable = UpcallTable;
	// 枚举协议，找到下层协议的WSAPROTOCOL_INFOW结构	
	WSAPROTOCOL_INFO	NextProtocolInfo;
	int nTotalProtos;
	LPWSAPROTOCOL_INFO pProtoInfo = SPIGetProvider(&nTotalProtos);//获取所有的协议
	// 下层入口ID	
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
		AGENTLOG(" WSPStartup:	Can not find underlying protocol \n");//没有找到对应的基础协议
		return WSAEPROVIDERFAILEDINIT;
	}
	// 加载下层协议的DLL
	int nError;
	TCHAR szBaseProviderDll[MAX_PATH];
	int nLen = MAX_PATH;
	// 取得下层提供程序DLL路径
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
	// 加载下层提供程序
	HMODULE hModule = ::LoadLibrary(szBaseProviderDll);
	if(hModule == NULL)
	{
		AGENTLOG(" WSPStartup:  LoadLibrary() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}

	// 导入下层提供程序的WSPStartup函数
	LPWSPSTARTUP  pfnWSPStartup = NULL;
	pfnWSPStartup = (LPWSPSTARTUP)::GetProcAddress(hModule, "WSPStartup");
	if(pfnWSPStartup == NULL)
	{
		AGENTLOG(" WSPStartup:  GetProcAddress() failed %d \n", ::GetLastError());
		return WSAEPROVIDERFAILEDINIT;
	}
	// 调用下层提供程序的WSPStartup函数
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
	// 保存下层提供者的函数表
	// 修改传递给上层的函数表，Hook感兴趣的函数，这里做为示例，仅Hook了WSPSendTo函数
	// 您还可以Hook其它函数，如WSPSocket、WSPCloseSocket、WSPConnect等
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
		// 取得主模块的名称
		::GetModuleFileName(NULL, g_szCurrentApp, MAX_PATH);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

