#include "StdAfx.h"
#include "LOG.h"

#include <iostream>

#define  MAX_NEWFILE	2000
#define	 MAX_FLUSHFILE	5

CLOG* CLOG::g_Instance = 0;

CLOG::CLOG(void)
{
	m_pFout=0;
	InitializeCriticalSection(&m_cs);
	createNewLog();
}

CLOG::~CLOG(void)
{
	if(m_pFout==0)
		return;

	m_pFout->flush();
	m_pFout->close();
	delete m_pFout;
	m_pFout = 0;
}

CLOG* CLOG::getInstance()
{
	if(g_Instance==0)
		g_Instance = new CLOG();
	return g_Instance;
}

void CLOG::DeleteInstance()
{
	if(g_Instance==0)
		return;
	//�ж���û�д򿪵�ostreamָ��
	if (g_Instance->m_pFout!=0)
	{
		g_Instance->m_pFout->flush();
		g_Instance->m_pFout->close();
		delete g_Instance->m_pFout;
		g_Instance->m_pFout=0;
	}
	delete g_Instance;
	g_Instance  = 0;

}

void CLOG::createNewLog()
{
	try
	{
		if (m_pFout!=0)
		{
			m_pFout->flush();
			m_pFout->close();
			delete m_pFout;
			m_pFout=0;
		}
		m_iMsgCount=0;
		time_t tt;
		struct tm	*p;
		time(&tt);
		p=localtime(&tt);
		char filename[260];
		memset(filename,0,260);
		CLOG::getCurrentEXEPath();
		sprintf_s(filename,260,"%s\\log\\%d-%d-%d_%d-%d-%d_nbox.log\0",m_strCurrentExePath.c_str(),p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
		m_pFout=new ofstream();
		locale::global(locale(""));//��ȫ��������Ϊ����ϵͳĬ������
		m_pFout->open(filename,ios::out);
		locale::global(locale("C"));//��ԭ
		{
			return ;
		}
	}
	catch (exception e)
	{

	}
}


void CLOG::outPut(string msg)
{
	//std::cout << msg;
	EnterCriticalSection(&getInstance()->m_cs);
	getInstance()->theOutPut(msg);
	LeaveCriticalSection(&getInstance()->m_cs);
}

void CLOG::theOutPut(string msg)
{
	if(m_pFout==0)
		return;

	time_t timep;

	struct tm *p;

	time(&timep);

	p=localtime(&timep);

	char filename[260];
	memset(filename,0,260);
	sprintf_s(filename,260,"%d:%d:%d -- ",p->tm_hour,p->tm_min , p->tm_sec);

	msg = string(filename) + msg;
	//add chenshuai
	getInstance()->m_strLogEdit = getInstance()->m_strLogEdit + "\r\n" + msg;

	m_iMsgCount++;

	*m_pFout << msg << "\r\n";

	if(m_iMsgCount % MAX_FLUSHFILE ==0)
		m_pFout->flush();

	if(m_iMsgCount % MAX_NEWFILE ==0)
		createNewLog();
}


const char* CLOG::getCurrentEXEPath()
{
	try{
		if (m_strCurrentExePath.length() > 0)
			return m_strCurrentExePath.c_str();

		char szEexFullpath[MAX_PATH];
		//�õ�EXE����Ŀ¼������"\"��
		//�õ������ģ����
		HMODULE hMyModule = GetModuleHandle(NULL);
		//�õ������·��
		DWORD dwLen = GetModuleFileNameA(hMyModule, szEexFullpath, MAX_PATH);
		//��������·��(�����������һ��"\")
		char * pPatch = &szEexFullpath[dwLen]; //szexepatch + dwLen;
		while (pPatch != NULL && *pPatch != '\\')
		{
			pPatch--;
		}
		//deletc by chenshuai
		//�õ����һ����б��֮���������1,����������Ϊ0,������������ݶ����ַ�����ͻ����
		//pPatch++;
		//*pPatch = 0;
		//delete

		//add by chenshuai   Ӧ�ý����һ����\��,ɾ�����򱣴�Ŀ¼�����		
		*pPatch=0;
		//add

		m_strCurrentExePath = string(szEexFullpath);
		//FILE* f=fopen("c:\\log.txt","a+");      //logpath
		//fputs(szEexFullpath,f);
		//fclose(f);
	
	}
	catch (...)
	{

	}
	return m_strCurrentExePath.c_str();
}
