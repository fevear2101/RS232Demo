// RS232Demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRS232DemoApp:
// �йش����ʵ�֣������ RS232Demo.cpp
//

class CRS232DemoApp : public CWinApp
{
public:
	CRS232DemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRS232DemoApp theApp;