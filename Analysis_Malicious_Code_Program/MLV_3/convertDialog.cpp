// convertDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MLV_3.h"
#include "convertDialog.h"
#include "afxdialogex.h"
#include "MLV_3Dlg.h"


// convertDialog ��ȭ �����Դϴ�.

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


// convertDialog �޽��� ó�����Դϴ�.

BOOL convertDialog::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CRect rt_2;
	GetClientRect(&rt_2);
	
	pDC->FillSolidRect(rt_2, RGB(52,56,56));
	return TRUE;

	return CDialogEx::OnEraseBkgnd(pDC);
}

void convertDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialogEx::OnPaint()��(��) ȣ������ ���ʽÿ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

BOOL convertDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
