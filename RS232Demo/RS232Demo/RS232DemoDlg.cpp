// RS232DemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RS232Demo.h"
#include "RS232DemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRS232DemoDlg �Ի���




CRS232DemoDlg::CRS232DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRS232DemoDlg::IDD, pParent)
	, m_DeviceState(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRS232DemoDlg::~CRS232DemoDlg()
{
}

void CRS232DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DEVICESTATE, m_DeviceState);
}

BEGIN_MESSAGE_MAP(CRS232DemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_USB1, &CRS232DemoDlg::OnBnClickedBtnUsb1)
	ON_BN_CLICKED(IDC_BTN_USB2, &CRS232DemoDlg::OnBnClickedBtnUsb2)
	ON_BN_CLICKED(IDC_BTN_USB3, &CRS232DemoDlg::OnBnClickedBtnUsb3)
	ON_BN_CLICKED(IDC_BTN_USB4, &CRS232DemoDlg::OnBnClickedBtnUsb4)
	ON_BN_CLICKED(IDC_BTN_USB5, &CRS232DemoDlg::OnBnClickedBtnUsb5)
	ON_BN_CLICKED(IDC_BTN_USB6, &CRS232DemoDlg::OnBnClickedBtnUsb6)
	ON_BN_CLICKED(IDC_BTN_USB7, &CRS232DemoDlg::OnBnClickedBtnUsb7)
	ON_BN_CLICKED(IDC_BTN_USB8, &CRS232DemoDlg::OnBnClickedBtnUsb8)
	ON_BN_CLICKED(IDC_BTN_USB9, &CRS232DemoDlg::OnBnClickedBtnUsb9)
	ON_BN_CLICKED(IDC_BTN_USB10, &CRS232DemoDlg::OnBnClickedBtnUsb10)
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_DEVICEARRIVAL,OnDeviceArrival)
	ON_MESSAGE(WM_DEVICEREMOVED,OnDeviceRemoved)
END_MESSAGE_MAP()


// CRS232DemoDlg ��Ϣ�������

BOOL CRS232DemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	theUSBPort.BeginSearch();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CRS232DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRS232DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CRS232DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRS232DemoDlg::OnClose()
{
	theUSBPort.EndSearch();	
	CDialog::OnClose();
}

void CRS232DemoDlg::OnBnClickedBtnUsb1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x01};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x02};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x03};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x04};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x05};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x06};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x07};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
	
}

void CRS232DemoDlg::OnBnClickedBtnUsb8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x08};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x09};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

void CRS232DemoDlg::OnBnClickedBtnUsb10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(theUSBPort.IsDeviceConnected())
	{
		BYTE data[7] = {0xff,0x01,0x00,0x08,0x00,0xff,0x0a};
		theUSBPort.Send(data,sizeof(data)/sizeof(BYTE),0);
	}
}

BOOL CRS232DemoDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	//0x4d36e978L, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18  
	//DEV_BROADCAST_DEVICEINTERFACE* dbd = (DEV_BROADCAST_DEVICEINTERFACE*) dwData;  
	theUSBPort.ShouldCheckDeviceRemoved(nEventType,(DEV_BROADCAST_HDR *)dwData,(DeviceDidArrivalCallback)DeviceDidArrival,(DeviceDidRemovedCallback)DeviceDidRemoved);
	return TRUE;  
}

void DeviceDidArrival()
{
	PostMessage(theApp.GetMainWnd()->GetSafeHwnd(),WM_DEVICEARRIVAL,0,0);
}

void DeviceDidRemoved()
{
	PostMessage(theApp.GetMainWnd()->GetSafeHwnd(),WM_DEVICEREMOVED,0,0);
}

LRESULT CRS232DemoDlg::OnDeviceArrival(WPARAM wpara,LPARAM lpara)
{
	m_DeviceState.Format(_T("USB Device Arrival!"));
	UpdateData(FALSE); 
	return 0L;
}

LRESULT CRS232DemoDlg::OnDeviceRemoved(WPARAM wpara,LPARAM lpara)
{
	m_DeviceState.Format(_T("USB Device Removed!"));
	UpdateData(FALSE);
	return 0L;
}