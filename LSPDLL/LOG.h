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
	//单例模式的唯一对象
	static CLOG*	g_Instance;
	//文件输出流
	ofstream*		m_pFout;
	//msg计数
	int				m_iMsgCount;
	//应用程序目录
	string			m_strCurrentExePath;
	CRITICAL_SECTION m_cs;
public:
	string m_strLogEdit;
private:
	//新建日志文件
	void createNewLog();
	void theOutPut(string msg);
public:
	~CLOG(void);
	//唯一实例生成函数
	static CLOG*	getInstance();
	//实例删除
	static void DeleteInstance();
	static	void outPut(string msg);
	const char * getCurrentEXEPath();
};
