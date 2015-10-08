// convertDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MLV_3.h"
#include "convertDialog.h"
#include "afxdialogex.h"
#include "MLV_3Dlg.h"


// convertDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(convertDialog, CDialogEx)

convertDialog::convertDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(convertDialog::IDD, pParent)
{
	
}

convertDialog::~convertDialog()
{
}

void convertDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ASSEMBLY_EDIT2, assemblyEdit_2);
	DDX_Control(pDX, IDC_CONVERT_EDIT2, convertEdit_2);
}


BEGIN_MESSAGE_MAP(convertDialog, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
//	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// convertDialog 메시지 처리기입니다.

BOOL convertDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CRect rt_2;
	GetClientRect(&rt_2);
	
	pDC->FillSolidRect(rt_2, RGB(52,56,56));
	return TRUE;

	return CDialogEx::OnEraseBkgnd(pDC);
}

void convertDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	//SetDlgItemText(IDC_ASSEMBLY_EDIT2, assemblyEdit);
	assemblyEdit_2.SetSel(-1,-1);
	assemblyEdit_2.SetFocus();
	setTextItem(IDC_ASSEMBLY_EDIT2, "convert.txt");

	convertEdit_2.SetSel(-1,-1);
	convertEdit_2.SetFocus();
	setTextItem(IDC_CONVERT_EDIT2, "completeCfile.txt");
}

void convertDialog::setTextItem(int nID, char* fileName)
{
	char fileData[1024];
	FILE* fp;

	fString = "";
	fp = fopen(fileName, "r");

	if(fp == NULL)
	{
		MessageBox("Error!");//show error msg box 
	}

	while(1)
	{
		if( fgets(fileData, 1023, fp) != NULL)
		{
			fString += fileData;
			fString += "\r\n";
		}

		else
		{
			break;
		}
	}

	SetDlgItemText(nID, fString);

	fclose(fp);
}

void convertDialog::setTextItem(int nID)
{
	SetDlgItemText(nID, fString);
}

void convertDialog::PostNcDestroy()
{
	DestroyWindow();
	delete this;
	CDialogEx::PostNcDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL convertDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE){
		return TRUE;
	}
	
	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_ASSEMBLY_EDIT2)->m_hWnd){
		
		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_CONVERT_EDIT2)->m_hWnd){
		
		return TRUE;
	}

	if(pMsg->message == WM_KEYDOWN){

		BOOL bShift = ((GetKeyState(VK_SHIFT) & 0x8000) != 0);
		BOOL bControl = ((GetKeyState(VK_CONTROL) & 0x8000) != 0); 
		BOOL bAlt = ((GetKeyState(VK_MENU) & 0x8000) != 0);

		if(bControl && !bShift && !bAlt){

			if(pMsg->wParam == 'v' || pMsg->wParam == 'V'){
				return TRUE;
			}
			if(pMsg->wParam == 'c' || pMsg->wParam == 'C'){
				return TRUE;
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
