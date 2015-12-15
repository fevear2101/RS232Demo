// RS232DemoDlg.h : 头文件
//

#pragma once

#include "USBSerialPort.h"

#define WM_DEVICEARRIVAL WM_USER + 1
#define WM_DEVICEREMOVED WM_USER + 2

// CRS232DemoDlg 对话框
class CRS232DemoDlg : public CDialog
{
// 构造
public:
	CRS232DemoDlg(CWnd* pParent = NULL);	// 标准构造函数

	CRS232DemoDlg::~CRS232DemoDlg();

// 对话框数据
	enum { IDD = IDD_RS232DEMO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

private:
	CUSBSerialPort theUSBPort;
public:
	afx_msg void OnBnClickedBtnUsb1();
	afx_msg void OnBnClickedBtnUsb2();
	afx_msg void OnBnClickedBtnUsb3();
	afx_msg void OnBnClickedBtnUsb4();
	afx_msg void OnBnClickedBtnUsb5();
	afx_msg void OnBnClickedBtnUsb6();
	afx_msg void OnBnClickedBtnUsb7();
	afx_msg void OnBnClickedBtnUsb8();
	afx_msg void OnBnClickedBtnUsb9();
	afx_msg void OnBnClickedBtnUsb10();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg LRESULT OnDeviceArrival(WPARAM wpara,LPARAM lpara);
	afx_msg LRESULT OnDeviceRemoved(WPARAM wpara,LPARAM lpara);
public:
	CString m_DeviceState;
};

void DeviceDidArrival();
void DeviceDidRemoved();
