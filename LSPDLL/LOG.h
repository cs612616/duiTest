#pragma once
#include <Windows.h>
#include <time.h>
#include <fstream>
#include <string>

using namespace std;

#define AGENTLOG(format,...)    {\
	char theloggg[4096];	\
	memset(theloggg, 0, 4096); \
	char formatbuf[1024];	\
	memset(formatbuf, 0, 1024); \
	sprintf_s(formatbuf,"%s:","%s");	\
	strcat_s(formatbuf, sizeof(formatbuf), format);	\
	sprintf_s(theloggg, formatbuf, /*__DATE__,*/ /*__TIME__*/ __FUNCTION__, __VA_ARGS__); \
	CLOG::outPut(theloggg);  \
}

class CLOG
{
private:
	CLOG(void);
	//����ģʽ��Ψһ����
	static CLOG*	g_Instance;
	//�ļ������
	ofstream*		m_pFout;
	//msg����
	int				m_iMsgCount;
	//Ӧ�ó���Ŀ¼
	string			m_strCurrentExePath;
	CRITICAL_SECTION m_cs;
public:
	string m_strLogEdit;
private:
	//�½���־�ļ�
	void createNewLog();
	void theOutPut(string msg);
public:
	~CLOG(void);
	//Ψһʵ�����ɺ���
	static CLOG*	getInstance();
	//ʵ��ɾ��
	static void DeleteInstance();
	static	void outPut(string msg);
	const char * getCurrentEXEPath();
};
