// Video.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#define POINTER_64 __ptr64
#include <dshow.h>
#include <Windows.h>
#include <iostream>
using namespace std;

#pragma comment(lib,"strmbasd.lib")  //这个是directshow的debug静态库
#pragma comment(lib,"quartz.lib")



//滤波器链表管理器
IGraphBuilder *pGraph=NULL;
//控制数据流的播放和停止等动作
IMediaControl *pControl=NULL;
//捕获图像滤镜的事件
IMediaEvent *pEvent = NULL;

ICaptureGraphBuilder2*	pCaptureGB=NULL;
IBaseFilter*			pBaseFilter=NULL;
int nCaptureDeviceNumber;      //Device Count  
wstring capDeviceName[10]; //the Device name  
wstring renderDeviceName[10]; // Render Device name    

HRESULT InitDev()
{
	//这第三个参数叫做类上下文。可以控制所创建组件是在与客户相同的进程中运行，还是在不同的进程中运行，即所谓的进程内组件和进程外组件。
	//CLSCTX_INPROC_HANDLER，进程内组件。一般是DLL实现。
	//		CLSCTX_INPROC_SERVER
	// 		客户希望创建在同一进程中运行的组件。为能够同客户在同一进程中运行，组件必须是在DLL中实现。 
	// 		CLSCTX_INPROC_HANDLER 
	// 		客户希望创建进程中处理器。一个进程中处理器实际上是一只实现了某个组件一部分的进程中组件。该组件的基体附录将由本地或远程服务器上的某个进程外组件实现。
	// 		SLSCTX_LOCAL_SERVER
	// 		客户希望创建一个在同一机器上的另外一个进程中运行的组件。本地服务器是由EXE实现的。 
	// 		SLSCTX_REMOTE_SERVER
	// 		客户希望创建一个在远程机器上运行的组件。此标志需要分布式COM正常工作。
	//创建滤镜管理器(hr为他的句柄)
	HRESULT hr=::CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&pGraph);
	if(FAILED(hr))
	{
		return S_FALSE;
	}
	//创建捕获管理器
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
	//指定枚举类型目录，获得IEnumMoniker接口
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
			hr=pMoniker->BindToStorage(0,0,IID_IPropertyBag,(void**)&pProBag);//通过IPropertyBag获取“FriendlyName”属性的值。
			if(SUCCEEDED(hr))
			{
				VARIANT varTemp;  
				varTemp.vt = VT_BSTR;  
				hr = pProBag->Read(L"FriendlyName",&varTemp,NULL);  
				if(SUCCEEDED(hr))
				{
					if(wcsncmp((WCHAR*)varTemp.bstrVal,deviceName,256)==0)
					{
						hr=pMoniker->BindToObject(0,0,IID_IBaseFilter,(void**)ppBaseFilter);  //通过IMoniker查询IBaseFilter接口（用于获取Filter，注意和BindToStorage()区别）。
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
		 // Add the audio capture filter to the filter graph.  把音频过滤器添加到过滤管理器中
		hr=pGraph->AddFilter(pBaseFilter,_T("Capture"));
		if(FAILED(hr))
		{
			return hr;
		}
		/************************************************************************/  
		/* Audio Prieview                                           */  
		/************************************************************************/  
		hr=BindDeviceFilter(wszDeviceName,CLSID_LegacyAmFilterCategory,&pPinTeeFilter);//枚举解码的的filter
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

	//初始化com库
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
	
	//开始枚举设备
	EnumDev();
	//绑定一个设备过滤器
	system("pause");
	return 0;
}

