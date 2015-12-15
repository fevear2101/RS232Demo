#pragma once
#include <complex>
#include <ATLBase.h>
#include "afxmt.h"

#define MyTrace5

// 校正卡内容---查找串口时用的数据类型 [11/23/2011 Pb]
typedef struct _CheckComData
{	
	int nComPort;
	BOOL bConnect;
	int nConnCom;
	CString strComNameFmt;
}
CheckComData;

#ifdef _DEBUG
#define COMPORT_COUNT					8
#else
#define COMPORT_COUNT					8
#endif

typedef enum _Com_Command_Type
{	
	Command_NULL			=	0
} 	
Com_Command_Type;


class CComConnect
{
private:
	//串口句柄
	HANDLE m_COM_Handle;
	//两个信号变量（串口操作用）
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;


	bool m_bIsConnect;
	int m_nComPort;

	////////////////////////////////////////////串口操作/////////////////////////////////////
	long OpenCom(long lngPort,char *cfgMessage,long lngInSize,long lngOutSize);	//打开串口
	long CloseCom();															//关闭串口
	long SendData(BYTE *bytBuffer, long lngSize );								//发送数据
	long AcceptData(BYTE *bytBuffer, long lngSize );							//接收数据
	long ClearAcceptBuffer();													//清空接收缓存
	long ClearSendBuffer();														//清空发送缓存
	////////////////////////////////////////////串口操作/////////////////////////////////////
	
	void Delay_time(double msecond);				//精确延时函数
	void TRACE_SEND(const BYTE* pData,int nDataCount);

public:
	CComConnect(void);
	~CComConnect(void);

	bool CheckComPort();														//查找串口是否已打开
	void SetComPort(int newComPort);

	void ReportError(BOOL bSendOK,BOOL bSend);

	BOOL CheckConnDevice();

	BOOL OpenPort(int i);
	BOOL ClosePort();
	BOOL Send(BYTE* pData,int nBuffCount,double DelayTime);

	CEvent EVTest;
};

