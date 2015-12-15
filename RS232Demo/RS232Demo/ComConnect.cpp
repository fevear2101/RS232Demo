#include "StdAfx.h"
#include "ComConnect.h"
#include "Mmsystem.h"

//#define MyTrace
//#define MyTrace5
//#define MyTraceWithTime

#define FrequencyFrom 40
#define FrequencyTo 90
#define UpdataFireware	235824

//#define UD_INDEPEND_OK

#define SEND_DATA_WITH_CHECK(pBuff,pData,nDataCount,nDelayTime)		\
	Send(pData,nDataCount,nDelayTime);								\
	for(int iCount = 0;iCount < nDataCount;iCount ++)				\
		pBuff.Add(pData[iCount]);									\

#define SAVE_DATA_WITH_CHECK(pBuff,pData,nDataCount,nDelayTime,bSend)	\
	if(!bSend)															\
	{																	\
		Send(pData,nDataCount,nDelayTime);								\
		for(int iCount = 0;iCount < nDataCount;iCount ++)				\
			pBuff.Add(pData[iCount]);									\
	}																	\

#define CCC_SEND_DATA_WITH_CHECK(pBuff,pData,nDataCount,nDelayTime)		\
	g_pCCC->Send(pData,nDataCount,nDelayTime);								\
	for(int iCount = 0;iCount < nDataCount;iCount ++)				\
		pBuff.Add(pData[iCount]);									\

#define CCC_SAVE_DATA_WITH_CHECK(pBuff,pData,nDataCount,nDelayTime,bSend)	\
	if(!bSend)															\
	{																	\
		g_pCCC->Send(pData,nDataCount,nDelayTime);								\
		for(int iCount = 0;iCount < nDataCount;iCount ++)				\
			pBuff.Add(pData[iCount]);									\
	}																	\

extern CComConnect* g_pCCC;

long CComConnect::OpenCom(long lngPort,char *cfgMessage,long lngInSize,long lngOutSize)
{
	try
	{
		char szMsg[255];
		//CString lpMsg;
		DCB dcb;

		//�򿪶˿�
		if (lngPort>9)
			sprintf_s( szMsg, "\\\\.\\COM%d", lngPort );
			//lpMsg.Format(_T("\\\\.\\COM%d"), lngPort);
		else
			sprintf_s( szMsg, "COM%d", lngPort );
			//lpMsg.Format(_T("COM%d"), lngPort);

		//���첽��ʽ��д����
		m_COM_Handle  = CreateFileA(szMsg, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED , NULL );
		//m_COM_Handle  = CreateFile(lpMsg, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED , NULL );
		if( m_COM_Handle == NULL ) 
			return( 2 );

		//����첽��д����
		memset(&(m_OverlappedRead), 0, sizeof (OVERLAPPED));
		memset(&(m_OverlappedWrite), 0, sizeof (OVERLAPPED));

		//����dcb��
		dcb.DCBlength = sizeof( DCB );         //����
		GetCommState(m_COM_Handle , &dcb );

		//������,��żУ��,����λ,ֹͣλ  �磺9600,n,8,1
		sprintf_s(szMsg,"COM%d:%s", lngPort,cfgMessage);
		BuildCommDCBA(szMsg,&dcb);
		//lpMsg.Format(_T("COM%d:%s"), lngPort,cfgMessage);
		//BuildCommDCB(lpMsg,&dcb);

		//------------------------------
		dcb.fBinary=TRUE;                      //�����Ʒ�ʽ
		dcb.fOutxCtsFlow=FALSE;                //����CTS��ⷢ��������
		dcb.fOutxDsrFlow=FALSE;                //����DSR��ⷢ��������
		dcb.fDtrControl=DTR_CONTROL_DISABLE;   //��ֹDTR��������
		dcb.fDsrSensitivity=FALSE;             //��DTR�ź��߲�����
		dcb.fTXContinueOnXoff=TRUE;            //�����ջ�����
		dcb.fOutX=FALSE;                       //���������ַ�����
		dcb.fInX =FALSE;                       //�������տ���
		dcb.fErrorChar=FALSE;                  //�Ƿ���ָ���ַ��滻У�����ַ�
		dcb.fNull=FALSE;                       //����NULL�ַ�
		dcb.fRtsControl=RTS_CONTROL_ENABLE;    //����RTS��������
		dcb.fAbortOnError=FALSE;               //���ʹ���󣬼�����������Ķ�д����
		dcb.fDummy2=0;                         //����
		//dcb.wReserved=0;                       //û��ʹ�ã�����Ϊ0
		dcb.XonLim=0;                          //ָ����XOFF�ַ�����֮ǰ���յ��������п��������С�ֽ���
		dcb.XoffLim=0;                         //ָ����XOFF�ַ�����֮ǰ�������п��������С�����ֽ���
		dcb.XonChar=0;                         //���ͺͽ��յ�XON�ַ�
		dcb.XoffChar=0;                        //���ͺͽ��յ�XOFF�ַ�
		dcb.ErrorChar=0;                       //������յ���żУ�������ַ�
		dcb.EofChar=0;                         //������ʾ���ݵĽ���
		dcb.EvtChar=0;                         //�¼��ַ������յ����ַ�ʱ�������һ���¼�
		dcb.wReserved1=0;                      //û��ʹ��
		//dcb.BaudRate =9600;                  //������
		//dcb.Parity=0;                        //��żУ��
		//dcb.ByteSize=8;                      //����λ
		//dcb.StopBits=0;                      //ֹͣλ
		//------------------------------

		if(dcb.Parity==0 )        // 0-4=None,Odd,Even,Mark,Space
		{
			dcb.fParity=FALSE;    //��żУ����Ч
		}
		else
		{
			dcb.fParity=TRUE;     //��żУ����Ч
		}
		sprintf_s(szMsg,"COM%d:%d,%d,%d,%d (InSize:%ld,OutSize:%ld)", lngPort,dcb.BaudRate,dcb.Parity,dcb.ByteSize,dcb.StopBits,lngInSize,lngOutSize);
		//lpMsg.Format(_T("COM%d:%d,%d,%d,%d (InSize:%ld,OutSize:%ld)"),lngPort,dcb.BaudRate,dcb.Parity,dcb.ByteSize,dcb.StopBits,lngInSize,lngOutSize);

		//��д��ʱ����
		COMMTIMEOUTS CommTimeOuts;
		//�����Ӳ���
		CommTimeOuts.ReadIntervalTimeout =0;                                   //�ַ�������ms   �ò������Ϊ���ֵ����ʹreadfile������������
		CommTimeOuts.ReadTotalTimeoutMultiplier =0;                             //�ܵĳ�ʱʱ��(�Ե����ֽ�)
		CommTimeOuts.ReadTotalTimeoutConstant = 2500;                           //����ĳ�ʱʱ��ms
		CommTimeOuts.WriteTotalTimeoutMultiplier =0;                            //�ܵĳ�ʱʱ��(�Ե����ֽ�)
		CommTimeOuts.WriteTotalTimeoutConstant = 2500;                          //����ĳ�ʱʱ��

		SetCommTimeouts( m_COM_Handle, &CommTimeOuts );

		//��ȡ�źž��
		m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		if( !SetCommState( m_COM_Handle, &dcb ) ||                   //�ж����ò����Ƿ�ɹ�
			!SetupComm( m_COM_Handle, lngInSize, lngOutSize ) ||     //�������������������Ƿ�ɹ�
			m_OverlappedRead.hEvent==NULL || m_OverlappedWrite.hEvent==NULL)
		{
			DWORD dwError = GetLastError();                     //��ȡ���Ĵ�����Ϣ
			if( m_OverlappedRead.hEvent != NULL )  CloseHandle( m_OverlappedRead.hEvent );
			if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
			CloseHandle( m_COM_Handle );
			m_COM_Handle=NULL;
			return dwError;
		}

		return( 0 );
	}
	catch(...)
	{
		return -1;
	}
}

long CComConnect::CloseCom()
{
	try
	{
		if(m_COM_Handle  == NULL ) return( 1 );
		SetCommMask(m_COM_Handle ,NULL);
		SetEvent(m_OverlappedRead.hEvent);
		SetEvent(m_OverlappedWrite.hEvent);

		if( m_OverlappedRead.hEvent != NULL ) 
			CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) 
			CloseHandle( m_OverlappedWrite.hEvent );
		if (CloseHandle( m_COM_Handle )==FALSE)
			return (2);

		m_COM_Handle  = NULL;
	}
	catch(...)
	{
		return (3);
	}
	return( 0 );
}

//*************************************************************************
//�� �� ����SendData
//��    �룺BYTE *bytBuffer,   ����
//      long lngSize       ����
//��    ����long
//������������������
//ȫ�ֱ�����
//����ģ�飺
//��    �ߣ�Ҷ��
//��    �ڣ�2006��4��4��
//�� �� �ˣ�
//��    �ڣ�
//��    ����
//*************************************************************************
long CComConnect::SendData(BYTE *bytBuffer, long lngSize )
{
	try
	{
		if( m_COM_Handle  == NULL )
			return( -1 );

		DWORD dwBytesWritten = lngSize;
		BOOL bWriteStat;
		COMSTAT ComStat;
		DWORD   dwErrorFlags;

		ClearCommError(m_COM_Handle,&dwErrorFlags,&ComStat);
		bWriteStat = WriteFile(m_COM_Handle, bytBuffer, lngSize, &dwBytesWritten, &(m_OverlappedWrite));

		if(!bWriteStat)
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{
				GetOverlappedResult(m_COM_Handle,&(m_OverlappedWrite),&dwBytesWritten,TRUE); //�ȴ�ֱ���������
			}
			else
			{
				dwBytesWritten=0;
			}
		}
		return (long)dwBytesWritten;
	}
	catch(...)
	{
		return -1;
	}
}

//*************************************************************************
//�� �� ����AcceptData
//��    �룺BYTE *bytBuffer,   ����
//      long lngSize       ����
//��    ����long
//������������ȡ����
//ȫ�ֱ�����
//����ģ�飺
//��    �ߣ�Ҷ��
//��    �ڣ�2006��4��4��
//�� �� �ˣ�
//��    �ڣ�
//��    ����
//*************************************************************************
long CComConnect::AcceptData(BYTE *bytBuffer, long lngSize )
{
	try
	{
		if( m_COM_Handle == NULL )
			return( -1 );

		DWORD   lngBytesRead=lngSize;
		BOOL    fReadStat;
		DWORD   dwRes=0;

		//������
		fReadStat=ReadFile(m_COM_Handle,bytBuffer,lngSize,&lngBytesRead,&(m_OverlappedRead));
		if( !fReadStat )
		{
			if( GetLastError() == ERROR_IO_PENDING )//�ص� I/O �����ڽ�����
			{
				dwRes = WaitForSingleObject(m_OverlappedRead.hEvent,1000);   //�ȴ���ֱ����ʱ
				switch(dwRes)
				{
				case WAIT_OBJECT_0:   //�����
					if(GetOverlappedResult(m_COM_Handle,&(m_OverlappedRead),&lngBytesRead,FALSE)==0)
					{
						//����
						return -2;
					}
					break;
				case WAIT_TIMEOUT:    //��ʱ
					return -1;
				default:              //WaitForSingleObject ����
					break;
				}
			}
		}

		return lngBytesRead;

	}
	catch(...)
	{
		return -1;
	}
}

//*************************************************************************
//�� �� ����ClearAcceptBuffer
//��    �룺
//��    ����long
//����������������ջ�����
//ȫ�ֱ�����
//����ģ�飺
//��    �ߣ�Ҷ��
//��    �ڣ�2006��4��4��
//�� �� �ˣ�
//��    �ڣ�
//��    ����
//*************************************************************************
long CComConnect::ClearAcceptBuffer()
{
	try
	{
		if(m_COM_Handle  == NULL ) return( -1 );
		PurgeComm(m_COM_Handle,PURGE_RXABORT | PURGE_RXCLEAR);   //
	}
	catch(...)
	{
		return(1);
	}
	return(0);
}

//*************************************************************************
//�� �� ����ClearSendBuffer
//��    �룺
//��    ����long
//����������������ͻ�����
//ȫ�ֱ�����
//����ģ�飺
//��    �ߣ�Ҷ��
//��    �ڣ�2006��4��4��
//�� �� �ˣ�
//��    �ڣ�
//��    ����
//*************************************************************************
long CComConnect::ClearSendBuffer()
{
    try
    {
            if(m_COM_Handle  == NULL ) return( -1 );
            PurgeComm(m_COM_Handle,PURGE_TXABORT |  PURGE_TXCLEAR);  //
    }
    catch(...)
    {
            return (1);
    }
    return(0);
}

void CComConnect::Delay_time(double msecond)   
{
	if(msecond >= 1000)
		Sleep((DWORD)msecond);
	else
	{
		int Initialized; 
		LARGE_INTEGER Frequency; 
		LARGE_INTEGER BeginTime; 
		LARGE_INTEGER CurTime; 
		double delay = (double)msecond / 1000;
		double Result = 0.0; 

		Initialized = QueryPerformanceFrequency(&Frequency);
		if(!Initialized)
			return ; 
		QueryPerformanceCounter(&BeginTime); 
		do 
		{ 
			QueryPerformanceCounter(&CurTime); 
			Result = (double)(CurTime.LowPart - BeginTime.LowPart) / (double)Frequency.LowPart;
		} 
		while(Result < delay);
	}
}




CComConnect::CComConnect(void) : EVTest( TRUE )
{
	m_nComPort = -1;
	m_bIsConnect = false;
	m_COM_Handle = NULL;
	//����첽��д����
	memset(&(m_OverlappedRead), 0, sizeof (OVERLAPPED));
	memset(&(m_OverlappedWrite), 0, sizeof (OVERLAPPED));
}

CComConnect::~CComConnect(void)
{
#ifdef ENABLE_INTERFACE_CORR_DATA_ADV			//��������ɫУ���ӿ�
	EndThreadSave();
	if(m_pInterfaceData)
	{
		delete m_pInterfaceData;
		m_pInterfaceData = NULL;
	}
#endif
	CloseCom();

}

bool CComConnect::CheckComPort()
{
	//if(m_bIsConnect)									//delete pb20081115
	//	return m_bIsConnect;
	if(m_nComPort <= 0)
	{
		for(int i = 1;i <= COMPORT_COUNT;i++)
		{
			CloseCom();
			if(0 == OpenCom(i,"115200,n,8,1",2048,2048))
			{
				m_bIsConnect = true;
				if( CheckConnDevice() )
				{
					TRACE("���ԣ�����%d������\n",i);
					m_nComPort = i;
					return m_bIsConnect;
				}
				
				TRACE("���ԣ�����%dδ����\n",i);
				m_bIsConnect = false;
			}
			else
			{
				TRACE("���ԣ��Ҳ�������%d\n",i);
			}
		}

		CloseCom();
		return false;
	}
	else
	{
		CloseCom();
		if(0 == OpenCom(m_nComPort,"115200,n,8,1",2048,2048))
		{
			m_bIsConnect = true;
			if( CheckConnDevice() )
			{
				TRACE("���ԣ�����%d������\n",m_nComPort);
				return m_bIsConnect;
			}

			TRACE("���ԣ�����%dδ����\n",m_nComPort);
			m_bIsConnect = false;
		}
		else
		{
			TRACE("���ԣ��Ҳ�������%d\n",m_nComPort);
		}
		return m_bIsConnect = false;
	}
}

void CComConnect::SetComPort(int newComPort)
{
	if(m_nComPort != newComPort)
	{
		m_nComPort = newComPort;
	}
}

BOOL CComConnect::CheckConnDevice()
{
	return FALSE;
}

void CComConnect::ReportError(BOOL bSendOK,BOOL bSend)
{
	if(bSendOK)
	{
		if(bSend)
			AfxMessageBox(_T("���ͳɹ�!"));
		else
			AfxMessageBox(_T("����ɹ�!"));
	}
	else
		AfxMessageBox(_T("���ڹ����޷�����,�����Ƿ����ӻ��������!"));
}

BOOL CComConnect::Send(BYTE* pData,int nBuffCount,double DelayTime)
{

	SendData(pData,nBuffCount);
	TRACE_SEND(pData,nBuffCount);
	Delay_time(DelayTime);
	return TRUE;
}

BOOL CComConnect::OpenPort(int i)
{
	if(OpenCom(i,"115200,n,8,1",2048,2048) == 0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CComConnect::ClosePort()
{
	if(CloseCom() == 0)
	{
		return TRUE;
	}
	return FALSE;
}

void CComConnect::TRACE_SEND(const BYTE* pData,int nDataCount)
{
#ifdef MyTrace
	int tCount = 0;
	for(int j = 0;j < nDataCount;j++)
	{
		TRACE("%02x",pData[j]);
		if(++tCount == (Num_Count_SendData + 1) )
		{
			TRACE("\n");
			tCount = 0;
		}
	}
#endif

#ifdef MyTrace5
	int tCount = 0;
	for(int j = 0;j < nDataCount;j++)
	{
		if((j % 6) == 0)
			continue;
		TRACE("%02x",pData[j]);
		if(++tCount == (/*Num_Count_SendData*/4 + 1) )
		{
			TRACE("\n");
			tCount = 0;
		}
	}
	int c = 0;
#endif

#ifdef MyTraceWithTime
	int tCount = 0;
	for(int j = 0;j < nDataCount;j++)
	{
		TRACE("%02x",pData[j]);
		if(++tCount == (Num_Count_SendData + 1) )
		{
			TRACE(",DelayTime = %f ms",DelayTime);
			TRACE("\n");
			tCount = 0;
		}
	}
#endif
}