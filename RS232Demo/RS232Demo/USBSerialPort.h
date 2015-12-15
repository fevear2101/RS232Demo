#pragma once
#include "comconnect.h"
#include "Dbt.h"
#include "Setupapi.h"
#include "iostream"

#pragma  comment(lib,"setupapi.lib")

#ifndef MACRO_HIDD_VIDPID  
#define MACRO_HIDD_VIDPID  
typedef struct _HIDD_VIDPID  
{  
	USHORT  VendorID;  
	USHORT  ProductID;  
} HIDD_VIDPID;  
#endif  

#define DeviceInstanceIdSize 256   

typedef enum SearchType{
	RS_SEARCH,
	RS_FOUND,
	RS_CONNECT
} PortState;

typedef struct Machinetype  
{
	PortState m_state;
	int portNum;
} MachineState;

typedef void (CALLBACK *DeviceDidArrivalCallback)();
typedef void (CALLBACK *DeviceDidRemovedCallback)();
typedef void (CALLBACK *SearchDidBeginCallback)();
typedef void (CALLBACK *DeviceDidFoundCallback)();
typedef void (CALLBACK *DeviceDidConnectedCallback)();

class CUSBSerialPort :
	public CObject
{
public:
	CUSBSerialPort(void);
	virtual ~CUSBSerialPort(void);

	void BeginSearch();
	void EndSearch();
	
	void ShouldCheckDeviceRemoved(UINT nEventType,DEV_BROADCAST_HDR* dhr,DeviceDidArrivalCallback darrival,DeviceDidRemovedCallback dremoved);
	void Send(BYTE* pData,int nBuffCount,double DelayTime);
	BOOL IsDeviceConnected();

	
private:
	CWinThread *pThreadForPortScan;
	
	
	
};

UINT __cdecl PortScanFun(LPVOID pParam);//搜寻设备
int GetDeviceUSBPortNum(std::string ssin,MachineState *state);//获取设备连接的串口号
int ConnectDevice(MachineState *state);//连接设备
std::string UperString(std::string ssin);
static BOOL StopFun = FALSE;
static CEvent RecStopTest;
static MachineState StateOfPortScan;
static CComConnect theCOM;