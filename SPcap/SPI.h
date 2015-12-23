#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <WS2SPI.H>

#pragma comment(lib,"ws2_32.lib")
class SPI
{
public:
	SPI(void);
	~SPI(void);
public:
	LPWSAPROTOCOL_INFO m_pProtocolInfo;
	int m_nTotalProtocol;
public:
	int EnumProtocol();
	void PrintProtocol();
	void FreeProtocol();

	int SPIEnumProtocol();
	void SPIPrintProtocol();
	void SPiFreeProtocol();
	//°²×°LSPº¯Êý
	BOOL SPIInstallProtocol(TCHAR *szPathName);

};

