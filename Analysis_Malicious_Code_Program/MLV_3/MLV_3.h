
// MLV_3.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMLV_3App:
// �� Ŭ������ ������ ���ؼ��� MLV_3.cpp�� �����Ͻʽÿ�.
//

class CMLV_3App : public CWinApp
{
public:
	CMLV_3App();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMLV_3App theApp;