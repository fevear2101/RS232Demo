#include "StdAfx.h"
#include "USBSerialPort.h"

CUSBSerialPort::CUSBSerialPort(void) : pThreadForPortScan(NULL)
{
}

CUSBSerialPort::~CUSBSerialPort(void)
{
	EndSearch();
}

UINT PortScanFun(LPVOID pParam)
{
	while(!StopFun)
	{
		CString str;
		switch (StateOfPortScan.m_state)
		{
		case RS_SEARCH:
			//FTDIBUS\COMPORT&VID_0403&PID_6001
			GetDeviceUSBPortNum("VID_0403&PID_6001",&StateOfPortScan);
			break;
		case RS_FOUND:
			ConnectDevice(&StateOfPortScan);
			break;
		case RS_CONNECT:
			//str.Format(_T("CONNECT USB RS232 COM%d\n"),StateOfPortScan.portNum);
			//TRACE(str);
			//BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x08};
			//theCOM.Send(data,sizeof(data)/sizeof(BYTE),0);
			break;
		}

		Sleep(1000);
	}
	RecStopTest.SetEvent();
	RecStopTest.Unlock();
	//TRACE(_T("Stop Safe\n"));
	return 0;
}

int GetDeviceUSBPortNum(std::string ssin,MachineState *state)
{
	ssin = UperString(ssin);
	int iCapacity=32;
	GUID* SetupClassGuid=NULL;
	GUID* InterfaceClassGuid=NULL;

	// 根据设备安装类GUID创建空的设备信息集合
	HDEVINFO DeviceInfoSet = SetupDiCreateDeviceInfoList( SetupClassGuid, NULL );
	if (DeviceInfoSet == INVALID_HANDLE_VALUE) return -1;

	// 根据设备安装类GUID获取设备信息集合
	HDEVINFO hDevInfo;

	if(InterfaceClassGuid == NULL)

		hDevInfo = SetupDiGetClassDevsEx( NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE | DIGCF_PRESENT, DeviceInfoSet, NULL, NULL );
	else
		hDevInfo = SetupDiGetClassDevsEx( InterfaceClassGuid, (PCWSTR)&"SCSI", NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT, DeviceInfoSet, NULL, NULL );

	if (hDevInfo == INVALID_HANDLE_VALUE) return -1;

	// 存储设备实例ID	
	TCHAR DeviceInstanceId[DeviceInstanceIdSize];	

	// 存储设备信息数据
	SP_DEVINFO_DATA DeviceInfoData;					
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// 获取设备信息数据
	DWORD DeviceIndex = 0;
	bool k=true;
	while (SetupDiEnumDeviceInfo( hDevInfo, DeviceIndex++, &DeviceInfoData))
	{
		// 获取设备实例ID
		if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, DeviceInstanceId, DeviceInstanceIdSize, NULL)&&k)
		{
			// 从设备实例ID中提取VID和PID
			TCHAR* pVidIndex = _tcsstr(DeviceInstanceId, TEXT("VID_"));
			if (pVidIndex == NULL) continue;

			TCHAR* pPidIndex = _tcsstr(pVidIndex + 4, TEXT("PID_"));
			if (pPidIndex == NULL) continue;

			USHORT VendorID = (USHORT)_tcstoul(pVidIndex + 4, NULL, 16);
			USHORT ProductID = (USHORT)_tcstoul(pPidIndex + 4, NULL, 16);

			//输出串口号

			TCHAR fname[56]={0}; 

			SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData,
				SPDRP_HARDWAREID,
				0,(PBYTE) fname,
				sizeof(fname),
				NULL);


			//判断相应VID号的串口


			//字符串方法识别FTDI
			std::string str1(57,'0');
			std::string s1=ssin;
			for (int i=0;i<56;i++)
			{
				str1[i]=(char)fname[i];
			}

			if(str1.find(s1)==std::string::npos) continue;

			//清空fname

			for (int i=0;i<56;i++)
			{
				fname[i]=0;
			}

			//	输出串口号
			SetupDiGetDeviceRegistryProperty(hDevInfo, &DeviceInfoData,
				SPDRP_FRIENDLYNAME , 
				0,(PBYTE) fname,
				sizeof(fname),
				NULL);

			std::string str2(57,'0');
			std::string s2="COM";
			for (int i=0;i<56;i++)
			{
				str2[i]=(char)fname[i];
			}
			str2 = UperString(str2);
			if(str2.find(s2)!=std::string::npos)
			{
				size_t num=str2.find(s2);
				size_t len = str2.find(")");
				std::string StrCom=str2.substr(num+3,len-num-3);
				CString str;
				int portNum = atoi(StrCom.c_str());
				state->portNum = portNum;
				state->m_state = RS_FOUND;
			}
		}
	}
	return 0;
}

int ConnectDevice(MachineState *state)
{
	if(theCOM.OpenPort(state->portNum))
	{
		state->m_state = RS_CONNECT;
		return 0;
	}
	else
	{
		state->m_state = RS_SEARCH;
		return -1;
	}
	return -1;

}

std::string UperString(std::string ssin)
{
	std::string ssout(ssin.length(),'0');
	for(size_t i = 0;i<ssin.length();++i)
	{
		ssout[i] = toupper(ssin[i]);
	}
	return ssout;
}

void CUSBSerialPort::BeginSearch()
{
	EndSearch();
	StateOfPortScan.m_state = RS_SEARCH;
	StateOfPortScan.portNum = 0;

	StopFun = FALSE;
	pThreadForPortScan = AfxBeginThread(PortScanFun,NULL,THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	if (pThreadForPortScan)
	{
		pThreadForPortScan->m_bAutoDelete = TRUE;
		pThreadForPortScan->ResumeThread();
	}
	
}

void CUSBSerialPort::EndSearch()
{
	if(pThreadForPortScan)
	{
		StopFun = TRUE;
		StateOfPortScan.m_state = RS_SEARCH;
		StateOfPortScan.portNum = 0;
		WaitForSingleObject(RecStopTest,5000);
		if(pThreadForPortScan != NULL)
		{
			pThreadForPortScan = NULL;
		}
		//TRACE(_T("after Stop Safe\n"));
	}
}

void CUSBSerialPort::ShouldCheckDeviceRemoved(UINT nEventType,DEV_BROADCAST_HDR* dhr,DeviceDidArrivalCallback darrival,DeviceDidRemovedCallback dremoved)
{
	TRACE("--nEventType--:%d\n", nEventType);
	switch (nEventType)
	{
	case DBT_DEVICEREMOVECOMPLETE://移除设备
		TRACE("--DEVICE REMOVE--\n");
#if 1
		if(dhr->dbch_devicetype == DBT_DEVTYP_PORT)
		{
			PDEV_BROADCAST_PORT lpdbv = (PDEV_BROADCAST_PORT)dhr;
			size_t len = wcslen(lpdbv->dbcp_name);
			CString name(lpdbv->dbcp_name);//COM8
			int port = 0;
			swscanf_s(name.GetBuffer(0), _T("COM%d"), &port);

			if(port == StateOfPortScan.portNum)
			{
				theCOM.ClosePort();
				StateOfPortScan.m_state = RS_SEARCH;
				StateOfPortScan.portNum = 0;
			}
			if(dremoved)
			{
				dremoved();
			}
		}
#endif
		TRACE("--DEVICE REMOVE--end\n");
		break;
	case DBT_DEVICEARRIVAL://添加设备
		TRACE("--DEVICE ARRIVAL--\n");
#if 1
		if(dhr->dbch_devicetype == DBT_DEVTYP_PORT)
		{
			PDEV_BROADCAST_PORT lpdbv = (PDEV_BROADCAST_PORT)dhr;
			size_t len = wcslen(lpdbv->dbcp_name);
			CString name(lpdbv->dbcp_name);//COM8
			int port = 0;
			swscanf_s(name.GetBuffer(0), _T("COM%d"), &port);
		}
#endif
		if(darrival)
		{
			darrival();
		}
		break;
	default:
		break;
	}
}

void CUSBSerialPort::Send(BYTE* pData,int nBuffCount,double DelayTime)
{
	theCOM.Send(pData,nBuffCount,DelayTime);
}

BOOL CUSBSerialPort::IsDeviceConnected()
{
	return StateOfPortScan.m_state == RS_CONNECT;
}