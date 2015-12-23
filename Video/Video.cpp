// Video.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#define POINTER_64 __ptr64
#include <dshow.h>
#include <Windows.h>
#include <iostream>
using namespace std;

#pragma comment(lib,"strmbasd.lib")  //�����directshow��debug��̬��
#pragma comment(lib,"quartz.lib")



//�˲������������
IGraphBuilder *pGraph=NULL;
//�����������Ĳ��ź�ֹͣ�ȶ���
IMediaControl *pControl=NULL;
//����ͼ���˾����¼�
IMediaEvent *pEvent = NULL;

ICaptureGraphBuilder2*	pCaptureGB=NULL;
IBaseFilter*			pBaseFilter=NULL;
int nCaptureDeviceNumber;      //Device Count  
wstring capDeviceName[10]; //the Device name  
wstring renderDeviceName[10]; // Render Device name    

HRESULT InitDev()
{
	//����������������������ġ����Կ������������������ͻ���ͬ�Ľ��������У������ڲ�ͬ�Ľ��������У�����ν�Ľ���������ͽ����������
	//CLSCTX_INPROC_HANDLER�������������һ����DLLʵ�֡�
	//		CLSCTX_INPROC_SERVER
	// 		�ͻ�ϣ��������ͬһ���������е������Ϊ�ܹ�ͬ�ͻ���ͬһ���������У������������DLL��ʵ�֡� 
	// 		CLSCTX_INPROC_HANDLER 
	// 		�ͻ�ϣ�����������д�������һ�������д�����ʵ������һֻʵ����ĳ�����һ���ֵĽ����������������Ļ��帽¼���ɱ��ػ�Զ�̷������ϵ�ĳ�����������ʵ�֡�
	// 		SLSCTX_LOCAL_SERVER
	// 		�ͻ�ϣ������һ����ͬһ�����ϵ�����һ�����������е���������ط���������EXEʵ�ֵġ� 
	// 		SLSCTX_REMOTE_SERVER
	// 		�ͻ�ϣ������һ����Զ�̻��������е�������˱�־��Ҫ�ֲ�ʽCOM����������
	//�����˾�������(hrΪ���ľ��)
	HRESULT hr=::CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&pGraph);
	if(FAILED(hr))
	{
		return S_FALSE;
	}
	//�������������
	hr=::CoCreateInstance(CLSID_CaptureGraphBuilder2,NULL,CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2,(void**)pCaptureGB);
	if(FAILED(hr))
	{
		return S_FALSE;
	}

	hr=pGraph->QueryInterface(IID_IMediaControl,(void**)&pControl);
	hr=pGraph->QueryInterface(IID_IMediaEvent,(void**)&pEvent);
	if(FAILED(hr))
	{
		return S_FALSE;
	}
	pCaptureGB->SetFiltergraph(pGraph);
	return S_OK;
}

HRESULT EnumDev()
{
	ICreateDevEnum	*pDevEnum;
	IEnumMoniker	*pEnumMon;
	IMoniker		*pMoniker;
	HRESULT hr=CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**)&pDevEnum);
	if(FAILED(hr))
	{
		return hr;
	}
	//ָ��ö������Ŀ¼�����IEnumMoniker�ӿ�
	hr=pDevEnum->CreateClassEnumerator(CLSID_AudioInputDeviceCategory,&pEnumMon,0);
	if(hr==S_FALSE)
	{
		hr=VFW_E_NOT_FOUND;
		return hr;
	}
	pEnumMon->Reset();
	ULONG cFetched;
	while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr==S_OK)
	{
		IPropertyBag	*pProbag;
		hr=pMoniker->BindToStorage(0,0,IID_IPropertyBag,(void**)&pProbag);
		if(SUCCEEDED(hr))
		{
			VARIANT varTmp;
			varTmp.vt=VT_BSTR;
			hr=pProbag->Read(_T("FriendlyName"),&varTmp,NULL);
			if(SUCCEEDED(hr))
			{
				capDeviceName[nCaptureDeviceNumber]=wstring(varTmp.bstrVal);
				wcout.imbue(locale("chs",locale::all^locale::numeric));
				wcout<<capDeviceName[nCaptureDeviceNumber].c_str()<<endl;
				nCaptureDeviceNumber++;
			}
			pProbag->Release();
		}
		pMoniker->Release();
	}
	pEnumMon->Release();
	return S_OK;
}
int BindDeviceFilter(WCHAR* deviceName,GUID deviceClsid,IBaseFilter** ppBaseFilter)//CLSID_AudioInputDeviceCategory,CLSID_VideoInputDeviceCategory
{
	ICreateDevEnum	*pDevEnum;
	IEnumMoniker	*pEnumMon;
	IMoniker		*pMoniker;
	HRESULT hr=CoCreateInstance(CLSID_SystemDeviceEnum,NULL,CLSCTX_INPROC_SERVER,IID_ICreateDevEnum,(void**)&pDevEnum);
	if(SUCCEEDED(hr))
	{
		hr=pDevEnum->CreateClassEnumerator(deviceClsid,&pEnumMon,0);
		if (hr==S_FALSE)
		{
			hr=VFW_E_NOT_FOUND;
			return hr;
		}
		pEnumMon->Reset();
		ULONG cFetched;
		int index=0;
		while(hr=pEnumMon->Next(1,&pMoniker,&cFetched),hr==S_OK)
		{
			IPropertyBag *pProBag;
			hr=pMoniker->BindToStorage(0,0,IID_IPropertyBag,(void**)&pProBag);//ͨ��IPropertyBag��ȡ��FriendlyName�����Ե�ֵ��
			if(SUCCEEDED(hr))
			{
				VARIANT varTemp;  
				varTemp.vt = VT_BSTR;  
				hr = pProBag->Read(L"FriendlyName",&varTemp,NULL);  
				if(SUCCEEDED(hr))
				{
					if(wcsncmp((WCHAR*)varTemp.bstrVal,deviceName,256)==0)
					{
						hr=pMoniker->BindToObject(0,0,IID_IBaseFilter,(void**)ppBaseFilter);  //ͨ��IMoniker��ѯIBaseFilter�ӿڣ����ڻ�ȡFilter��ע���BindToStorage()���𣩡�
						if(SUCCEEDED(hr))
						{
							return hr;
						}
					}
					SysFreeString(varTemp.bstrVal);
				}
				pProBag->Release();
			}
			pMoniker->Release();
		}
		pEnumMon->Release();
	}
	return hr;
}

HRESULT OpenDevice(WCHAR* wszDeviceName)
{
	HRESULT hr;
	IBaseFilter *pWaveDest=NULL,*pWriter=NULL,*pOutputDev=NULL;
	IFileSinkFilter	*pSink = NULL;
	WCHAR wszTmpDeviceName[256]=_T("Infinite Pin Tee Filter");
	WCHAR wszOutpin[256]=_T("Capture"),wszInpin[256]=_T("Input");
	IBaseFilter	*pPinTeeFilter=NULL;
	hr=BindDeviceFilter(wszDeviceName,CLSID_AudioInputDeviceCategory);
	if(SUCCEEDED(hr))
	{
		 // Add the audio capture filter to the filter graph.  ����Ƶ��������ӵ����˹�������
		hr=pGraph->AddFilter(pBaseFilter,_T("Capture"));
		if(FAILED(hr))
		{
			return hr;
		}
		/************************************************************************/  
		/* Audio Prieview                                           */  
		/************************************************************************/  
		hr=BindDeviceFilter(wszDeviceName,CLSID_LegacyAmFilterCategory,&pPinTeeFilter);//ö�ٽ���ĵ�filter
		if(FAILED(hr))
		{
			return hr;
		}
		hr=pGraph->AddFilter(pPinTeeFilter,wszDeviceName);
		hr=DeviceConnect(pBaseFilter,pPinTeeFilter,wszInpin,wszOutpin);
		if(FAILED(hr))
		{
			return hr;
		}
		//////////////////////////////////////////////////////////////////////////  
		// output the Rendered Device
		hr=BindDeviceFilter((WCHAR*)renderDeviceName[0].c_str(),CLSID_AudioInputDeviceCategory,&pOutputDev);
		if(FAILED(hr))
		{
			return hr;
		}
		hr=pGraph->AddFilter(pOutputDev,(WCHAR*)renderDeviceName[0].c_str());
		if(FAILED(hr))
		{
			return hr;
		}
		////////////////////////////////////////////////////////////////////////// 

		//Add AudioRecorder WAV Dest 
		wcsncmp(wszTmpDeviceName,_T("AudioRecorder WAV Dest"),256);
		hr=BindDeviceFilter(wszTmpDeviceName,CLSID_LegacyAmFilterCategory,&pWaveDest);
		if(FAILED(hr))
		{
			return hr;
		}
		hr=pGraph->AddFilter(pWaveDest,wszTmpDeviceName);;
		if(FAILED(hr))
		{
			return hr;
		}

	}
}

HRESULT DeviceConnect(IBaseFilter*	pInputDevice,IBaseFilter* pOutputDevice,WCHAR* wszInput,WCHAR *wszOutput)
{
	HRESULT hr;
	IEnumPins	*pInputPins=NULL,*pOutputPins=NULL;
	IPin	*pIn=NULL,*pOut=NULL;
	hr=pInputDevice->EnumPins(&pInputPins);
	if(SUCCEEDED(hr))
	{
		hr=pInputDevice->FindPin(wszInput,&pIn);
		if(FAILED(hr))	return hr;
	}
	else 
	{
		return hr;
	}

	hr=pOutputDevice->EnumPins(&pOutputPins);
	if(SUCCEEDED(hr))
	{
		hr=pOutputDevice->FindPin(wszOutput,&pOut);
		if(FAILED(hr))	return hr;
	}
	else 
	{
		return hr;
	}
	return hr;  
}

int _tmain(int argc, _TCHAR* argv[])
{

	//��ʼ��com��
	HRESULT hr=CoInitialize(NULL);
	if(FAILED(hr))
	{
		return S_FALSE;
	}
	hr=InitDev();
	if(FAILED(hr))
	{
		return S_FALSE;
	}
	
	//��ʼö���豸
	EnumDev();
	//��һ���豸������
	system("pause");
	return 0;
}

