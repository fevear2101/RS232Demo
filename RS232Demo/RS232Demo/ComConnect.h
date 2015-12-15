#pragma once
#include <complex>
#include <ATLBase.h>
#include "afxmt.h"

#define MyTrace5

// У��������---���Ҵ���ʱ�õ��������� [11/23/2011 Pb]
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
	//���ھ��
	HANDLE m_COM_Handle;
	//�����źű��������ڲ����ã�
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;


	bool m_bIsConnect;
	int m_nComPort;

	////////////////////////////////////////////���ڲ���/////////////////////////////////////
	long OpenCom(long lngPort,char *cfgMessage,long lngInSize,long lngOutSize);	//�򿪴���
	long CloseCom();															//�رմ���
	long SendData(BYTE *bytBuffer, long lngSize );								//��������
	long AcceptData(BYTE *bytBuffer, long lngSize );							//��������
	long ClearAcceptBuffer();													//��ս��ջ���
	long ClearSendBuffer();														//��շ��ͻ���
	////////////////////////////////////////////���ڲ���/////////////////////////////////////
	
	void Delay_time(double msecond);				//��ȷ��ʱ����
	void TRACE_SEND(const BYTE* pData,int nDataCount);

public:
	CComConnect(void);
	~CComConnect(void);

	bool CheckComPort();														//���Ҵ����Ƿ��Ѵ�
	void SetComPort(int newComPort);

	void ReportError(BOOL bSendOK,BOOL bSend);

	BOOL CheckConnDevice();

	BOOL OpenPort(int i);
	BOOL ClosePort();
	BOOL Send(BYTE* pData,int nBuffCount,double DelayTime);

	CEvent EVTest;
};

