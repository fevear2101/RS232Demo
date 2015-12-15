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

		//打开端口
		if (lngPort>9)
			sprintf_s( szMsg, "\\\\.\\COM%d", lngPort );
			//lpMsg.Format(_T("\\\\.\\COM%d"), lngPort);
		else
			sprintf_s( szMsg, "COM%d", lngPort );
			//lpMsg.Format(_T("COM%d"), lngPort);

		//用异步方式读写串口
		m_COM_Handle  = CreateFileA(szMsg, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED , NULL );
		//m_COM_Handle  = CreateFile(lpMsg, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED , NULL );
		if( m_COM_Handle == NULL ) 
			return( 2 );

		//清空异步读写参数
		memset(&(m_OverlappedRead), 0, sizeof (OVERLAPPED));
		memset(&(m_OverlappedWrite), 0, sizeof (OVERLAPPED));

		//设置dcb块
		dcb.DCBlength = sizeof( DCB );         //长度
		GetCommState(m_COM_Handle , &dcb );

		//波特率,奇偶校验,数据位,停止位  如：9600,n,8,1
		sprintf_s(szMsg,"COM%d:%s", lngPort,cfgMessage);
		BuildCommDCBA(szMsg,&dcb);
		//lpMsg.Format(_T("COM%d:%s"), lngPort,cfgMessage);
		//BuildCommDCB(lpMsg,&dcb);

		//------------------------------
		dcb.fBinary=TRUE;                      //二进制方式
		dcb.fOutxCtsFlow=FALSE;                //不用CTS检测发送流控制
		dcb.fOutxDsrFlow=FALSE;                //不用DSR检测发送流控制
		dcb.fDtrControl=DTR_CONTROL_DISABLE;   //禁止DTR流量控制
		dcb.fDsrSensitivity=FALSE;             //对DTR信号线不敏感
		dcb.fTXContinueOnXoff=TRUE;            //检测接收缓冲区
		dcb.fOutX=FALSE;                       //不做发送字符控制
		dcb.fInX =FALSE;                       //不做接收控制
		dcb.fErrorChar=FALSE;                  //是否用指定字符替换校验错的字符
		dcb.fNull=FALSE;                       //保留NULL字符
		dcb.fRtsControl=RTS_CONTROL_ENABLE;    //允许RTS流量控制
		dcb.fAbortOnError=FALSE;               //发送错误后，继续进行下面的读写操作
		dcb.fDummy2=0;                         //保留
		//dcb.wReserved=0;                       //没有使用，必须为0
		dcb.XonLim=0;                          //指定在XOFF字符发送之前接收到缓冲区中可允许的最小字节数
		dcb.XoffLim=0;                         //指定在XOFF字符发送之前缓冲区中可允许的最小可用字节数
		dcb.XonChar=0;                         //发送和接收的XON字符
		dcb.XoffChar=0;                        //发送和接收的XOFF字符
		dcb.ErrorChar=0;                       //代替接收到奇偶校验错误的字符
		dcb.EofChar=0;                         //用来表示数据的结束
		dcb.EvtChar=0;                         //事件字符，接收到此字符时，会产生一个事件
		dcb.wReserved1=0;                      //没有使用
		//dcb.BaudRate =9600;                  //波特率
		//dcb.Parity=0;                        //奇偶校验
		//dcb.ByteSize=8;                      //数据位
		//dcb.StopBits=0;                      //停止位
		//------------------------------

		if(dcb.Parity==0 )        // 0-4=None,Odd,Even,Mark,Space
		{
			dcb.fParity=FALSE;    //奇偶校验无效
		}
		else
		{
			dcb.fParity=TRUE;     //奇偶校验有效
		}
		sprintf_s(szMsg,"COM%d:%d,%d,%d,%d (InSize:%ld,OutSize:%ld)", lngPort,dcb.BaudRate,dcb.Parity,dcb.ByteSize,dcb.StopBits,lngInSize,lngOutSize);
		//lpMsg.Format(_T("COM%d:%d,%d,%d,%d (InSize:%ld,OutSize:%ld)"),lngPort,dcb.BaudRate,dcb.Parity,dcb.ByteSize,dcb.StopBits,lngInSize,lngOutSize);

		//读写超时设置
		COMMTIMEOUTS CommTimeOuts;
		//西门子参数
		CommTimeOuts.ReadIntervalTimeout =0;                                   //字符允许间隔ms   该参数如果为最大值，会使readfile命令立即返回
		CommTimeOuts.ReadTotalTimeoutMultiplier =0;                             //总的超时时间(对单个字节)
		CommTimeOuts.ReadTotalTimeoutConstant = 2500;                           //多余的超时时间ms
		CommTimeOuts.WriteTotalTimeoutMultiplier =0;                            //总的超时时间(对单个字节)
		CommTimeOuts.WriteTotalTimeoutConstant = 2500;                          //多余的超时时间

		SetCommTimeouts( m_COM_Handle, &CommTimeOuts );

		//获取信号句柄
		m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		if( !SetCommState( m_COM_Handle, &dcb ) ||                   //判断设置参数是否成功
			!SetupComm( m_COM_Handle, lngInSize, lngOutSize ) ||     //设置输入和输出缓冲区是否成功
			m_OverlappedRead.hEvent==NULL || m_OverlappedWrite.hEvent==NULL)
		{
			DWORD dwError = GetLastError();                     //获取最后的错误信息
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
//函 数 名：SendData
//输    入：BYTE *bytBuffer,   数据
//      long lngSize       个数
//输    出：long
//功能描述：发送数据
//全局变量：
//调用模块：
//作    者：叶帆
//日    期：2006年4月4日
//修 改 人：
//日    期：
//版    本：
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
				GetOverlappedResult(m_COM_Handle,&(m_OverlappedWrite),&dwBytesWritten,TRUE); //等待直到发送完毕
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
//函 数 名：AcceptData
//输    入：BYTE *bytBuffer,   数据
//      long lngSize       个数
//输    出：long
//功能描述：读取数据
//全局变量：
//调用模块：
//作    者：叶帆
//日    期：2006年4月4日
//修 改 人：
//日    期：
//版    本：
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

		//读数据
		fReadStat=ReadFile(m_COM_Handle,bytBuffer,lngSize,&lngBytesRead,&(m_OverlappedRead));
		if( !fReadStat )
		{
			if( GetLastError() == ERROR_IO_PENDING )//重叠 I/O 操作在进行中
			{
				dwRes = WaitForSingleObject(m_OverlappedRead.hEvent,1000);   //等待，直到超时
				switch(dwRes)
				{
				case WAIT_OBJECT_0:   //读完成
					if(GetOverlappedResult(m_COM_Handle,&(m_OverlappedRead),&lngBytesRead,FALSE)==0)
					{
						//错误
						return -2;
					}
					break;
				case WAIT_TIMEOUT:    //超时
					return -1;
				default:              //WaitForSingleObject 错误
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
//函 数 名：ClearAcceptBuffer
//输    入：
//输    出：long
//功能描述：清除接收缓冲区
//全局变量：
//调用模块：
//作    者：叶帆
//日    期：2006年4月4日
//修 改 人：
//日    期：
//版    本：
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
//函 数 名：ClearSendBuffer
//输    入：
//输    出：long
//功能描述：清除发送缓冲区
//全局变量：
//调用模块：
//作    者：叶帆
//日    期：2006年4月4日
//修 改 人：
//日    期：
//版    本：
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
	//清空异步读写参数
	memset(&(m_OverlappedRead), 0, sizeof (OVERLAPPED));
	memset(&(m_OverlappedWrite), 0, sizeof (OVERLAPPED));
}

CComConnect::~CComConnect(void)
{
#ifdef ENABLE_INTERFACE_CORR_DATA_ADV			//允许开放亮色校正接口
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
					TRACE("测试：串口%d已连接\n",i);
					m_nComPort = i;
					return m_bIsConnect;
				}
				
				TRACE("测试：串口%d未连接\n",i);
				m_bIsConnect = false;
			}
			else
			{
				TRACE("测试：找不到串口%d\n",i);
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
				TRACE("测试：串口%d已连接\n",m_nComPort);
				return m_bIsConnect;
			}

			TRACE("测试：串口%d未连接\n",m_nComPort);
			m_bIsConnect = false;
		}
		else
		{
			TRACE("测试：找不到串口%d\n",m_nComPort);
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
			AfxMessageBox(_T("发送成功!"));
		else
			AfxMessageBox(_T("保存成功!"));
	}
	else
		AfxMessageBox(_T("串口故障无法发送,请检查是否连接或更换串口!"));
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