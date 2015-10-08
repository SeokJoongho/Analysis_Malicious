
// MLV_3Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MLV_3.h"
#include "MLV_3Dlg.h"
#include "afxdialogex.h"
#include "convertDialog.h"
#include "ui_connector.h"
#include "convert.h"
#include "analysis.h"

#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

convert cvt;
analysis analy;
convertDialog* convertDlg = NULL;
// CMLV_3Dlg ��ȭ ����

CMLV_3Dlg::CMLV_3Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMLV_3Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_Edit1 = _T("");
	m_Edit2 = _T("");
	m_Edit3 = _T("");

	imageSectionTree = NULL;
	sectionTree = NULL;
}

void CMLV_3Dlg::DoDataExchange(CDataExchange* pDX)
{
	/*
		DoDataExchange�Լ�
	*/
	CDialogEx::DoDataExchange(pDX);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGO, logo);
	DDX_Control(pDX, IDC_ANALYSIS_BUTTON, analysisButton);
	DDX_Control(pDX, IDC_STATISTICS_BUTTON, statisticsButton);
	DDX_Control(pDX, IDC_HOME_BUTTON, homeButton);
	DDX_Control(pDX, IDCANCEL, closeButton);
	DDX_Control(pDX, IDC_FIND, findButton);
	DDX_Control(pDX, IDC_ASSEMBLY, assemblyButton);
	DDX_Control(pDX, IDC_CONVERTC, convertButton);
	DDX_Control(pDX, IDC_RESIZE_1, resizeButton_1);
	DDX_Control(pDX, IDC_RESIZE_2, resizeButton_2);
	DDX_Control(pDX, IDC_FUNCTION_LIST, functionList);
	DDX_Control(pDX, IDC_REPORT, reportButton);
	DDX_Control(pDX, IDC_FUNCTION, functionButton);
	DDX_Control(pDX, IDC_LIST_Header, m_ListH);
	DDX_Control(pDX, IDC_LIST_Section, m_ListS);
	DDX_Control(pDX, IDC_LIST_Import, m_ListI);
	DDX_Control(pDX, IDC_ANALYSIS_2, analysisButton_2);
	DDX_Control(pDX, IDC_RESIZE_CLOSE_1, resizeCloseButton_1);
	DDX_Control(pDX, IDC_RESIZE_CLOSE_2, resizeCloseButton_2);
	DDX_Control(pDX, IDC_ANIMATE1, loadingAnimation);
	DDX_Control(pDX, IDC_ASSEMBLY_EDIT, assemblyEdit);
	DDX_Control(pDX, IDC_CONVERT_EDIT, convertEdit);
	DDX_Control(pDX, IDC_TREE1, treeCtrl);
	DDX_Control(pDX, IDC_PARAMETER_BUTTON, parameterButton);
	DDX_Control(pDX, IDC_REFERENCE_BUTTON, referenceButton);
	DDX_Control(pDX, IDC_MESSAGE, message);
	DDX_Control(pDX, IDC_CODE_BUTTON, codeButton);
	DDX_Control(pDX, IDC_HOME_LIST, homeList);
	DDX_Text(pDX, IDC_EDIT1, m_Edit1);
	DDX_Text(pDX, IDC_EDIT2, m_Edit2);
	DDX_Text(pDX, IDC_EDIT3, m_Edit3);
}

BEGIN_MESSAGE_MAP(CMLV_3Dlg, CDialogEx)
	/*
		����� �κ��Դϴ�.
	*/
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ANALYSIS_BUTTON, &CMLV_3Dlg::OnBnClickedAnalysisButton)
	ON_BN_CLICKED(IDC_STATISTICS_BUTTON, &CMLV_3Dlg::OnBnClickedStatisticsButton)
	ON_BN_CLICKED(IDC_HOME_BUTTON, &CMLV_3Dlg::OnBnClickedHomeButton)
	ON_BN_CLICKED(IDC_FIND, &CMLV_3Dlg::OnBnClickedFind)
	ON_BN_CLICKED(IDC_RESIZE_1, &CMLV_3Dlg::OnBnClickedResize1)
	ON_BN_CLICKED(IDC_RESIZE_2, &CMLV_3Dlg::OnBnClickedResize2)

	ON_LBN_SELCHANGE(IDC_FUNCTION_LIST, &CMLV_3Dlg::OnLbnSelchangeFunctionList)
	ON_BN_CLICKED(IDC_REPORT, &CMLV_3Dlg::OnBnClickedReport)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_Section, &CMLV_3Dlg::OnLvnItemchangedListSection)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_Import, &CMLV_3Dlg::OnLvnItemchangedListImport)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_Header, &CMLV_3Dlg::OnLvnItemchangedListHeader)
	ON_BN_CLICKED(IDC_REPORT, &CMLV_3Dlg::OnBnClickedReport)
	ON_BN_CLICKED(IDCANCEL, &CMLV_3Dlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ANALYSIS_2, &CMLV_3Dlg::OnBnClickedAnalysis2)
	ON_BN_CLICKED(IDC_RESIZE_CLOSE_1, &CMLV_3Dlg::OnBnClickedResizeClose1)
	ON_BN_CLICKED(IDC_RESIZE_CLOSE_2, &CMLV_3Dlg::OnBnClickedResizeClose2)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CMLV_3Dlg::OnTvnSelchangedTree1)

	ON_WM_NCDESTROY()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CODE_BUTTON, &CMLV_3Dlg::OnBnClickedCodeButton)
//	ON_EN_CHANGE(IDC_EDIT1, &CMLV_3Dlg::OnChangeEdit1)
//	ON_EN_CHANGE(IDC_EDIT2, &CMLV_3Dlg::OnChangeEdit2)
//	ON_EN_CHANGE(IDC_EDIT3, &CMLV_3Dlg::OnChangeEdit3)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CMLV_3Dlg �޽��� ó����

BOOL CMLV_3Dlg::OnInitDialog()
{
	/*
		OnInitDialog�Լ�
		Dialog�� ��Ʈ�ѵ��� �ʱ�ȭ �ϱ� ���� �Լ�.
	*/
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if(start_db() == 1){
		HBITMAP hbit_1 = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
		logo.SetBitmap(hbit_1);

		CRect rt_1;
		GetClientRect(&rt_1);
		logo.SetWindowPos(NULL, 0, 0, rt_1.Width(),rt_1.Height(), SWP_SHOWWINDOW);

		edit_bk_brush.CreateSolidBrush(RGB(230, 230, 230));
		SetDlgItemText(IDC_MESSAGE, "�м����� 3�п��� 5���� �ð��� �ҿ�˴ϴ�.");
		SetDlgItemText(IDC_MESSAGE_2, "�м��� ������ �������ּ���.");

		closeButton.LoadBitmaps(IDB_BITMAP5, IDB_BITMAP5, IDB_BITMAP5, IDB_BITMAP5);
		closeButton.SizeToContent();

		analysisButton.LoadBitmaps(IDB_BITMAP2, IDB_BITMAP2, IDB_BITMAP2, IDB_BITMAP2);
		analysisButton.SizeToContent();

		homeButton.LoadBitmaps(IDB_BITMAP4, IDB_BITMAP4, IDB_BITMAP4, IDB_BITMAP4);
		homeButton.SizeToContent();

		statisticsButton.LoadBitmaps(IDB_BITMAP3, IDB_BITMAP3, IDB_BITMAP3, IDB_BITMAP3);
		statisticsButton.SizeToContent();

		convertButton.LoadBitmaps(IDB_BITMAP8, IDB_BITMAP8, IDB_BITMAP8, IDB_BITMAP8);
		convertButton.SizeToContent();

		assemblyButton.LoadBitmaps(IDB_BITMAP7, IDB_BITMAP7, IDB_BITMAP7, IDB_BITMAP7);
		assemblyButton.SizeToContent();

		findButton.LoadBitmaps(IDB_BITMAP6, IDB_BITMAP6, IDB_BITMAP6, IDB_BITMAP6);
		findButton.SizeToContent();

		resizeButton_1.LoadBitmaps(IDB_BITMAP9, IDB_BITMAP9, IDB_BITMAP9, IDB_BITMAP9);
		resizeButton_1.SizeToContent();

		resizeButton_2.LoadBitmaps(IDB_BITMAP9, IDB_BITMAP9, IDB_BITMAP9, IDB_BITMAP9);
		resizeButton_2.SizeToContent();

		reportButton.LoadBitmaps(IDB_BITMAP11, IDB_BITMAP11, IDB_BITMAP11, IDB_BITMAP11);
		reportButton.SizeToContent();

		functionButton.LoadBitmaps(IDB_BITMAP14, IDB_BITMAP14, IDB_BITMAP14, IDB_BITMAP14);
		functionButton.SizeToContent();

		analysisButton_2.LoadBitmaps(IDB_BITMAP16, IDB_BITMAP16, IDB_BITMAP16, IDB_BITMAP16);
		analysisButton_2.SizeToContent();

		resizeCloseButton_1.LoadBitmaps(IDB_BITMAP10, IDB_BITMAP10, IDB_BITMAP10, IDB_BITMAP10);
		resizeCloseButton_1.SizeToContent();

		resizeCloseButton_2.LoadBitmaps(IDB_BITMAP10, IDB_BITMAP10, IDB_BITMAP10, IDB_BITMAP10);
		resizeCloseButton_2.SizeToContent();

		parameterButton.LoadBitmaps(IDB_BITMAP17, IDB_BITMAP17, IDB_BITMAP17, IDB_BITMAP17);
		parameterButton.SizeToContent();

		referenceButton.LoadBitmaps(IDB_BITMAP18, IDB_BITMAP18, IDB_BITMAP18, IDB_BITMAP18);
		referenceButton.SizeToContent();

		codeButton.LoadBitmaps(IDB_BITMAP19, IDB_BITMAP19, IDB_BITMAP19, IDB_BITMAP19);
		codeButton.SizeToContent();

		m_font.CreatePointFont(120,TEXT("Miriam Fixed"));

		GetDlgItem(IDC_FUNCTION_LIST)->SetFont(&m_font);
		GetDlgItem(IDC_ANALYSIS_EDIT2)->SetFont(&m_font);
		GetDlgItem(IDC_ANALYSIS_EDIT3)->SetFont(&m_font);
		GetDlgItem(IDC_STATISTICS_EDIT1)->SetFont(&m_font);
		GetDlgItem(IDC_REFERENCE_EDIT)->SetFont(&m_font);
		GetDlgItem(IDC_PARAMETER_EDIT)->SetFont(&m_font);
		GetDlgItem(IDC_MESSAGE)->SetFont(&m_font);
		GetDlgItem(IDC_MESSAGE_2)->SetFont(&m_font);

		m_ListH.InsertColumn(0, "Target Machine", LVCFMT_LEFT, 200);
		m_ListH.InsertColumn(1, "Compliction TimeStamp", LVCFMT_LEFT, 200);
		m_ListH.InsertColumn(2, "Entry Point", LVCFMT_LEFT, 200);
		m_ListH.InsertColumn(3, "Number of Sections", LVCFMT_LEFT, 200);

		m_ListS.InsertColumn(0, "Name", LVCFMT_LEFT, 150);
		m_ListS.InsertColumn(1, "Virtual address", LVCFMT_LEFT, 200);
		m_ListS.InsertColumn(2, "Virtual size", LVCFMT_LEFT, 180);
		m_ListS.InsertColumn(3, "Raw size", LVCFMT_LEFT, 180);

		m_ListI.InsertColumn(0, "DLL Name", LVCFMT_LEFT, 200);
		m_ListI.InsertColumn(1, "Function Name", LVCFMT_LEFT, 200);

		loadingAnimation.Open(IDR_AVI1);

		m_tip_ctrl.Create(this);

		m_tip_ctrl.AddTool(GetDlgItem(IDC_ANALYSIS_BUTTON), "Convert to Assembly language and C language");
		m_tip_ctrl.AddTool(GetDlgItem(IDC_REPORT), "Offer a Report sheet");
		m_tip_ctrl.AddTool(GetDlgItem(IDC_STATISTICS_BUTTON), "Offer Parameters and References about functions");

		m_tip_ctrl.SetDelayTime(TTDT_AUTOPOP,20000);
		m_tip_ctrl.SetTipBkColor(RGB(220,220,220));

		(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(FALSE);
		(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(FALSE);
		(GetDlgItem(IDC_CONVERTC)->ShowWindow(FALSE));
		(GetDlgItem(IDC_STATISTICS_EDIT1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_RESIZE_1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_RESIZE_2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_FUNCTION))->ShowWindow(FALSE);
		(GetDlgItem(IDC_FUNCTION_LIST))->ShowWindow(FALSE);
		(GetDlgItem(IDC_RESIZE_CLOSE_1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_RESIZE_CLOSE_2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_ASSEMBLY_EDIT))->ShowWindow(FALSE);
		(GetDlgItem(IDC_CONVERT_EDIT))->ShowWindow(FALSE);
		(GetDlgItem(IDC_ANIMATE1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_TREE1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
		(GetDlgItem(IDC_REFERENCE_EDIT))->ShowWindow(FALSE);
		(GetDlgItem(IDC_PARAMETER_EDIT))->ShowWindow(FALSE);
		(GetDlgItem(IDC_PARAMETER_BUTTON))->ShowWindow(FALSE);
		(GetDlgItem(IDC_REFERENCE_BUTTON))->ShowWindow(FALSE);
		(GetDlgItem(IDC_MESSAGE))->ShowWindow(FALSE);
		(GetDlgItem(IDC_CODE_BUTTON))->ShowWindow(FALSE);

		(GetDlgItem(IDC_ANALYSIS_EDIT1))->ShowWindow(TRUE);
		(GetDlgItem(IDC_FIND))->ShowWindow(TRUE);
		(GetDlgItem(IDC_LIST_Header))->ShowWindow(TRUE);
		(GetDlgItem(IDC_LIST_Section))->ShowWindow(TRUE);
		(GetDlgItem(IDC_LIST_Import))->ShowWindow(TRUE);
		(GetDlgItem(IDC_ANALYSIS_2))->ShowWindow(TRUE);
		(GetDlgItem(IDC_MESSAGE_2))->ShowWindow(TRUE);

		(GetDlgItem(IDC_ANALYSIS_BUTTON))->EnableWindow(FALSE);
		(GetDlgItem(IDC_HOME_BUTTON))->EnableWindow(FALSE);
		(GetDlgItem(IDC_STATISTICS_BUTTON))->EnableWindow(FALSE);
		(GetDlgItem(IDC_ANALYSIS_2))->EnableWindow(FALSE);
	}

	else{
		MessageBox("Error!");
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

BOOL CMLV_3Dlg::PreTranslateMessage(MSG* pMsg)
	/*
		PreTranslateMessage�Լ�
		
	*/
{

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_tip_ctrl.RelayEvent(pMsg);

	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE){
		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_EDIT1)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_EDIT2)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_EDIT3)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_ANALYSIS_EDIT2)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_ANALYSIS_EDIT3)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_ASSEMBLY)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_CONVERTC)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_STATISTICS_EDIT1)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_ASSEMBLY_EDIT)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_CONVERT_EDIT)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_REFERENCE_EDIT)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_PARAMETER_EDIT)->m_hWnd){

		return TRUE;
	}

	if(pMsg->message == WM_RBUTTONDOWN && pMsg->hwnd == GetDlgItem(IDC_ANALYSIS_EDIT1)->m_hWnd){

		return TRUE;
	}
	if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_SNAPSHOT){
		::OpenClipboard(NULL);
		::EmptyClipboard();
		::CloseClipboard();
	}

	if (GetKeyState(VK_MENU)&& pMsg->wParam == VK_SNAPSHOT){
		::OpenClipboard(NULL);
		::EmptyClipboard();
		::CloseClipboard();
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

void CMLV_3Dlg::OnPaint()
	/*
		OnPaint�Լ�
		���α׷����� �⺻������ �������ִ� �Լ��Դϴ�.
	*/
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HBRUSH CMLV_3Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	/*
		OnCtlColor�Լ�
		���α׷����� �⺻������ �������ִ� �Լ��Դϴ�.
	*/

{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor){
	case CTLCOLOR_STATIC:
		{
			if(pWnd->GetDlgCtrlID() == IDC_MESSAGE){

				pDC->SetTextColor(RGB(240,240,240));
				pDC->SetBkMode(TRANSPARENT);

				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
			if(pWnd->GetDlgCtrlID() == IDC_MESSAGE_2){

				pDC->SetTextColor(RGB(240,240,240));
				pDC->SetBkMode(TRANSPARENT);

				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}
		}
	}
	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMLV_3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMLV_3Dlg::OnDestroy()
	/*
		OnDestroy�Լ�
	*/
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	edit_bk_brush.DeleteObject();

}

void CMLV_3Dlg::OnBnClickedAnalysisButton()
	/*
		onBnClickedAnalysisButton�Լ�
		���Ϻм���� ��ư�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/

{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	(GetDlgItem(IDC_STATISTICS_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FIND))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FUNCTION))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FUNCTION_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Header))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Section))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Import))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_TREE1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REFERENCE_EDIT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_PARAMETER_EDIT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_PARAMETER_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REFERENCE_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CODE_BUTTON))->ShowWindow(FALSE);

	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(TRUE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(TRUE));
	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(TRUE);
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(TRUE);

	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);
}

void CMLV_3Dlg::OnBnClickedStatisticsButton()
	/*
		OnBnClickedStatisticsButton�Լ�
		�Լ��м���� ��ư�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	(GetDlgItem(IDC_ANALYSIS_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FIND))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(FALSE));
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Header))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Section))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Import))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_TREE1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);

	(GetDlgItem(IDC_REFERENCE_EDIT))->ShowWindow(TRUE);
	(GetDlgItem(IDC_PARAMETER_EDIT))->ShowWindow(TRUE);
	(GetDlgItem(IDC_PARAMETER_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDC_REFERENCE_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDC_STATISTICS_EDIT1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_FUNCTION))->ShowWindow(TRUE);
	(GetDlgItem(IDC_FUNCTION_LIST))->ShowWindow(TRUE);
	(GetDlgItem(IDC_CODE_BUTTON))->ShowWindow(TRUE);

	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);
}

void CMLV_3Dlg::OnBnClickedHomeButton()
	/*
		OnBnClickedHomeButton�Լ�
		����ȭ�� ��ư�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	(GetDlgItem(IDC_ANALYSIS_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FIND))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(FALSE));
	(GetDlgItem(IDC_STATISTICS_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FUNCTION))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FUNCTION_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Header))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Section))->ShowWindow(FALSE);
	(GetDlgItem(IDC_LIST_Import))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REFERENCE_EDIT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_PARAMETER_EDIT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_PARAMETER_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REFERENCE_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CODE_BUTTON))->ShowWindow(FALSE);

	(GetDlgItem(IDC_TREE1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);

	char sectionName[1024];

	if(newFile == 0)
	{
		imageSectionTree = (HTREEITEM *)malloc(sizeof(HTREEITEM) * cvt.getNumberOfSections());
		sectionTree = (HTREEITEM *)malloc(sizeof(HTREEITEM) * cvt.getNumberOfSections());

		top = treeCtrl.InsertItem(cvt.getPath(),TVI_ROOT,TVI_LAST);
		do_1 = treeCtrl.InsertItem("IMAGE_DOS_HEADER", top, TVI_LAST);
		do_2 = treeCtrl.InsertItem("MS-DOS Stub Program", top, TVI_LAST);
		do_3 = treeCtrl.InsertItem("IMAGE_NT_HEADERS", top, TVI_LAST);

		treeCtrl.InsertItem("Signature", do_3, TVI_LAST);
		treeCtrl.InsertItem("IMAGE_FILE_HEADER", do_3, TVI_LAST);
		treeCtrl.InsertItem("IMAGE_OPTIONAL_HEADER", do_3, TVI_LAST);

		for(int i = 0; i < cvt.getNumberOfSections(); i++)
		{
			strcpy_s(sectionName, "IMAGE_SECTION_HEADER ");
			strcat_s(sectionName, cvt.getSectionName(i));
			imageSectionTree[i] = treeCtrl.InsertItem(sectionName, top, TVI_LAST);
		}

		for(int i = 0; i < cvt.getNumberOfSections(); i++)
		{
			strcpy_s(sectionName, "SECTION ");
			strcat_s(sectionName, cvt.getSectionName(i));
			sectionTree[i] = treeCtrl.InsertItem(sectionName, top, TVI_LAST);
		}

		top = treeCtrl.GetRootItem();
		treeCtrl.Expand(top, TVE_EXPAND);
		treeCtrl.Expand(do_1, TVE_EXPAND);
		treeCtrl.Expand(do_2, TVE_EXPAND);
		treeCtrl.Expand(do_3, TVE_EXPAND);

		newFile = 1;
	}
}

void CMLV_3Dlg::OnBnClickedFind()
	/*
		OnBnClickedFind�Լ�
		���Ϻм���忡 �ִ� ���Ͽ��¹�ư(FIND)�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�
		exe������ �����Ͽ� �м� �� DB���� PE header, import, section ������ �޾ƿ� 
		Listcontrol�� ��ġ.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString str;
	char** headerInfo;
	char*** sectionInfo;
	char** importInfo;

	char* dllData;
	char* ptr;

	char* tokPtr;
	int numberOfSections;
	int count;

	LPCTSTR szFilter = _T("EXE Files(*.exe)| *.exe|");
	CFileDialog openDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, szFilter);

	if (openDlg.DoModal() == IDOK)
	{
		cvt.clear();
		analy.clear();

		m_ListH.DeleteAllItems();
		m_ListS.DeleteAllItems();
		m_ListI.DeleteAllItems();
		functionList.ResetContent();
		functionList.SetSel(-1,-1);
		SetCursor(LoadCursor( NULL, IDC_WAIT ));

		str = openDlg.GetFileName();
		pathName = openDlg.GetPathName();

		SetDlgItemText(IDC_ANALYSIS_EDIT1, pathName);

		set_path((LPSTR)(LPCTSTR)pathName);

		headerInfo = header_info(&dberr);

		if(dberr != -1)
		{
			m_ListH.InsertItem(0, _T(headerInfo[0]));
			m_ListH.SetItemText(0, 1, headerInfo[1]);
			m_ListH.SetItemText(0, 2, headerInfo[2]);
			m_ListH.SetItemText(0, 3, headerInfo[3]);

			numberOfSections = atoi(headerInfo[3]);
			sectionInfo = section_info();

			for(int i = 0; i < numberOfSections; i++)
			{
				m_ListS.InsertItem(i, _T(sectionInfo[i][0]));
				m_ListS.SetItemText(i, 1, sectionInfo[i][1]);
				m_ListS.SetItemText(i, 2, sectionInfo[i][2]);
				m_ListS.SetItemText(i, 3, sectionInfo[i][3]);
			}

			importInfo = import_info(&count);

			for(int i = 0; i < count; i++)
			{
				dllData = importInfo[i];
				ptr = strtok_s(dllData, ":", &tokPtr);
				m_ListI.InsertItem(i, _T(ptr));
				ptr = strtok_s(NULL, ":", &tokPtr);
				m_ListI.SetItemText(i, 1, ptr);
			}
		}

		SetCursor(LoadCursor( NULL, IDC_ARROW ));
		(GetDlgItem(IDC_ANALYSIS_2))->EnableWindow(TRUE);
		(GetDlgItem(IDC_MESSAGE))->ShowWindow(TRUE);
		(GetDlgItem(IDC_MESSAGE_2))->ShowWindow(FALSE);

		if(imageSectionTree != NULL)
		{
			free(imageSectionTree);
			imageSectionTree = NULL;
		}

		if(sectionTree != NULL)
		{
			free(sectionTree);
			sectionTree = NULL;
		}

		if(treeCtrl.GetCount() > 0)
		{
			treeCtrl.DeleteAllItems();
		}

		newFile = 0;
	}
}

void CMLV_3Dlg::OnLbnSelchangeFunctionList()
	/*
		OnLbnSelchangeFunctionList�Լ�
		�Լ��м���忡 �ִ� �Լ� ����� �����ִ� Listbox contol�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�
		�м��� exe���Ͽ� ���� �Լ� ������ DB���� �޾ƿ� Listbox control�� ��ġ.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CEdit* e = (CEdit*)GetDlgItem(IDC_REFERENCE_EDIT);
	e->HideCaret();

	int index = functionList.GetCurSel();

	char **ref;
	CString reference;
	CString parameter;

	if(index != LB_ERR)
	{
		functionList.GetText(index, filename);
	}

	char *func_name = (LPSTR)(LPCTSTR)filename;

	ref = print_refer(func_name);
	reference = "";

	parameter = (LPCSTR)(LPSTR)ref[0];
	reference = (LPCSTR)(LPSTR)ref[1];

	parameter.Replace("\n", "\r\n\r\n");
	parameter.Replace(",", ",\r\n");

	SetDlgItemText(IDC_PARAMETER_EDIT, parameter);
	SetDlgItemText(IDC_REFERENCE_EDIT, reference);

}

void CMLV_3Dlg::OnBnClickedReport()
	/*
		OnBnClickedReport�Լ�
		������� ��ư�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(FALSE));
	(GetDlgItem(IDC_STATISTICS_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FUNCTION))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FUNCTION_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_TREE1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REFERENCE_EDIT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_PARAMETER_EDIT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_PARAMETER_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REFERENCE_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CODE_BUTTON))->ShowWindow(FALSE);

	(GetDlgItem(IDC_FIND))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_EDIT1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_LIST_Header))->ShowWindow(TRUE);
	(GetDlgItem(IDC_LIST_Section))->ShowWindow(TRUE);
	(GetDlgItem(IDC_LIST_Import))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_2))->ShowWindow(TRUE);

	(GetDlgItem(IDC_ANALYSIS_2))->EnableWindow(FALSE);

	(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);
}

void CMLV_3Dlg::OnLvnItemchangedListSection(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CMLV_3Dlg::OnLvnItemchangedListImport(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void CMLV_3Dlg::OnLvnItemchangedListHeader(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

}

void CMLV_3Dlg::OnBnClickedCancel()
	/*
		OnBnClickedCancel�Լ�	
		�����ư�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(convertDlg != NULL)
	{
		convertDlg->DestroyWindow();
		//delete convertDlg;
	}	

	cvt.clear();
	CDialogEx::OnCancel();

	exit_db();
}

void CMLV_3Dlg::OnBnClickedAnalysis2()
	/*
		OnBnClickedAnalysis2�Լ�	
		�м������ analysis ��ư�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.

	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	(GetDlgItem(IDC_ANALYSIS_BUTTON))->EnableWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_2))->EnableWindow(FALSE);
	(GetDlgItem(IDC_FIND))->EnableWindow(FALSE);

	(GetDlgItem(IDC_ANALYSIS_BUTTON))->EnableWindow(TRUE);
	(GetDlgItem(IDC_HOME_BUTTON))->EnableWindow(TRUE);
	(GetDlgItem(IDC_STATISTICS_BUTTON))->EnableWindow(TRUE);

	SetCursor(LoadCursor( NULL, IDC_WAIT ));
	(GetDlgItem(IDC_ANIMATE1))->ShowWindow(TRUE);

	loadingAnimation.Play(0,-1,-1);

	char timeStamp[512];
	char integerValue[1024];
	struct tm* t;
	char **f;
	int count = 0;
	int funcReturnValue = 0;

	CString str;

	char dirPath[1024];

	GetDlgItemText(IDC_ANALYSIS_EDIT1, dirPath, 1024);

	cvt.reOpenFile(dirPath);
	cvt.doParseImageDosHeader();
	cvt.doSkip();
	funcReturnValue = cvt.doParseImageNTHeader();

	//IMAGE_NT_HEADER�� ������ ����� �������� ���ϸ� ����ó��
	if(funcReturnValue == -1)
	{
		MessageBox("Wrong Section Name");
	}

	else if(funcReturnValue == -2)
	{
		MessageBox("File was packed");
	}

	else if(funcReturnValue == -3)
	{
		MessageBox("Raw file pointer or Raw file size wrong");
	}

	else if(funcReturnValue == -99)
	{
		MessageBox("64bit architecture file, not yet supported");
	}

	else if(funcReturnValue == 0) //IMAGE_NT_HEADER�� ������ ����� �������� �� ���ķ� ����
	{
		funcReturnValue = cvt.findIATData();

		if(funcReturnValue == 0)	//IMAGE ADDRESS TABLE�� ������ ����� �������� �� ���ķ� ����
		{
			if(cvt.sendPEHeaderInfo() == 1)	//database�� binary file�� ������ ������ database�� binary file�� ���� insert
			{
				cvt.sendPESectionInfo();
				cvt.sendPEImportInfo();

				if(cvt.getMachine() == 0x014C)
				{
					m_ListH.InsertItem(0, _T("Intel 386"));
				}

				else
				{
					m_ListH.InsertItem(0, _T("Unknown Machine"));
				}

				t = cvt.getTimeStamp();

				sprintf_s(timeStamp, "%d-%d-%d %d:%d:%d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

				m_ListH.SetItemText(0, 1, timeStamp);

				sprintf_s(integerValue, "0x%08X", cvt.getAddressOfEntryPoint());
				m_ListH.SetItemText(0, 2, integerValue);

				memset(integerValue, 0, 1024);
				_itoa_s(cvt.getNumberOfSections(), integerValue, 10);

				m_ListH.SetItemText(0, 3, integerValue);

				for(int i=0; i < cvt.getNumberOfSections(); i++)
				{
					m_ListS.InsertItem(i, _T(cvt.getSectionName(i)));
					memset(integerValue, 0, 1024);
					sprintf_s(integerValue, "0x%08X", cvt.getVirtualAddress(i));
					m_ListS.SetItemText(i, 1, integerValue);
					memset(integerValue, 0, 1024);
					sprintf_s(integerValue, "0x%08X", cvt.getVirtualSize(i));
					m_ListS.SetItemText(i, 2, integerValue);
					memset(integerValue, 0, 1024);
					sprintf_s(integerValue, "0x%08X", cvt.getSizeOfRawData(i));
					m_ListS.SetItemText(i, 3, integerValue);

				}

				for(int i=0; i < cvt.getIibnSize(); i++)
				{
					m_ListI.InsertItem(i, _T(cvt.getDllName(i)));
					m_ListI.SetItemText(i, 1, cvt.getFuncName(i));
				}
			}

			cvt.separateSection();
			funcReturnValue = cvt.doConvertHexToAsm();

			if(funcReturnValue == -4)	//assembly language�� ������ ����� �ȵǾ��� ��� ���� ó��
			{
				MessageBox("Code Section Offset setting failed");
			}

			else if(funcReturnValue == -5)
			{
				MessageBox("failed to access data section");
			}

			else if(funcReturnValue == 0)	//assembly language�� ������ ����� �Ǿ��� ��� analysis ����
			{
				setTextItem(IDC_ANALYSIS_EDIT2, "convert.txt");
				setTextItem(IDC_ASSEMBLY_EDIT);

				analy.setExeName(cvt.getPath());
				analy.setBasicFile("convert.txt", "mainAddress.txt");
				analy.doSetInformation();
				analy.doSeparateCode();
				analy.doSetOrder();
				analy.makeCfile();

				setTextItem(IDC_ANALYSIS_EDIT3, "completeCFile.txt");
				setTextItem(IDC_CONVERT_EDIT);

				f = print_exe_list();
				count = atoi(f[0]);

				for(int i = 1;i < count;i++){
					str = (LPCSTR)(LPSTR)f[i];
					functionList.AddString(str);
				}

			}

			SetDlgItemText(IDC_STATISTICS_EDIT1, pathName);
		}

		else if(funcReturnValue == -7)
		{
			MessageBox("failed to find IAT data");
		}
	}

	loadingAnimation.Stop();
	SetCursor(LoadCursor( NULL, IDC_ARROW ));
	(GetDlgItem(IDC_ANIMATE1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FIND))->EnableWindow(TRUE);

	if(funcReturnValue == 0)
	{		
		(GetDlgItem(IDC_MESSAGE))->ShowWindow(FALSE);	

		(GetDlgItem(IDC_ANALYSIS_BUTTON))->EnableWindow(TRUE);
		(GetDlgItem(IDC_ANALYSIS_2))->EnableWindow(TRUE);

		OnBnClickedAnalysisButton();
	}	
}

void CMLV_3Dlg::OnBnClickedResize1()
	/*
		OnBnClickedResize1�Լ�
		��������� Assembly �ڵ带 ū â���� �����ִ�
		Edit contol�� ���� ��ư[+]�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/

{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	assemblyEdit.SetSel(-1,-1);
	assemblyEdit.SetFocus();

	(GetDlgItem(IDC_RESIZE_CLOSE_1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ASSEMBLY_EDIT))->ShowWindow(TRUE);

	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FIND))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(FALSE));
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_HOME_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REPORT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_STATISTICS_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDCANCEL))->ShowWindow(FALSE);
}

void CMLV_3Dlg::OnBnClickedResize2()
	/*
		OnBnClickedResize2�Լ�
		��������� C���� mapping �ڵ带 ū â���� �����ִ�
		Edit contol�� ���� ��ư[+]�� ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/

{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	convertEdit.SetSel(-1,-1);
	convertEdit.SetFocus();

	(GetDlgItem(IDC_RESIZE_CLOSE_2))->ShowWindow(TRUE);
	(GetDlgItem(IDC_CONVERT_EDIT))->ShowWindow(TRUE);

	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(FALSE);
	(GetDlgItem(IDC_FIND))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(FALSE));
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ANALYSIS_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_HOME_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDC_REPORT))->ShowWindow(FALSE);
	(GetDlgItem(IDC_STATISTICS_BUTTON))->ShowWindow(FALSE);
	(GetDlgItem(IDCANCEL))->ShowWindow(FALSE);
}

void CMLV_3Dlg::OnBnClickedResizeClose1()
	/*
		OnBnClickedResizeClose1�Լ�
		��������� Aseembly �ڵ带 ū â���� �����ִ�
		Edit contol�� �ݱ� ��ư ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	assemblyEdit.SetSel(-1,-1);
	assemblyEdit.SetFocus();

	(GetDlgItem(IDC_RESIZE_CLOSE_1))->ShowWindow(FALSE);
	(GetDlgItem(IDC_ASSEMBLY_EDIT))->ShowWindow(FALSE);

	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(TRUE);
	(GetDlgItem(IDC_FIND))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(TRUE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(TRUE));
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDC_HOME_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDC_REPORT))->ShowWindow(TRUE);
	(GetDlgItem(IDC_STATISTICS_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDCANCEL))->ShowWindow(TRUE);
}

void CMLV_3Dlg::OnBnClickedResizeClose2()
	/*
		OnBnClickedResizeClose2�Լ�
		��������� C���� mapping�� �ڵ带 ū â���� �����ִ�
		Edit contol�� �ݱ� ��ư ��Ʈ�� �˸��� ó���ϱ� ���� �Լ�.
		������ ��Ʈ�ѵ��� ShowWindow�Լ��� ���� ���α׷�UI���� visualization�Ͽ���.
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	convertEdit.SetSel(-1,-1);
	convertEdit.SetFocus();

	(GetDlgItem(IDC_RESIZE_CLOSE_2))->ShowWindow(FALSE);
	(GetDlgItem(IDC_CONVERT_EDIT))->ShowWindow(FALSE);

	(GetDlgItem(IDC_ANALYSIS_EDIT2))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_EDIT3))->ShowWindow(TRUE);
	(GetDlgItem(IDC_FIND))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ASSEMBLY))->ShowWindow(TRUE);
	(GetDlgItem(IDC_CONVERTC)->ShowWindow(TRUE));
	(GetDlgItem(IDC_RESIZE_1))->ShowWindow(TRUE);
	(GetDlgItem(IDC_RESIZE_2))->ShowWindow(TRUE);
	(GetDlgItem(IDC_ANALYSIS_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDC_HOME_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDC_REPORT))->ShowWindow(TRUE);
	(GetDlgItem(IDC_STATISTICS_BUTTON))->ShowWindow(TRUE);
	(GetDlgItem(IDCANCEL))->ShowWindow(TRUE);
}

void CMLV_3Dlg::setTextItem(int nID1, int nID2, int nID3, char* fileName)
	/*
		setTextItem�Լ�
		
		fileName ������ ������ nID1, nID2, nID3�� ������ �����ϴ� �Լ�
	*/
{
	FILE* fp;
	char fileData[1024];
	char* tokTemp;
	char* tokPtr;

	CString editString1;
	CString editString2;
	CString editString3;

	editString1.Empty();
	editString2.Empty();
	editString3.Empty();

	editString1 = "";
	editString2 = "";
	editString3 = "";


	fopen_s(&fp, fileName, "r");

	if(fp == NULL)
	{
		MessageBox("Error!");//show error msg box 
	}

	while(1)
	{
		if( fgets(fileData, 1023, fp) != NULL)
		{
			tokTemp = strtok_s(fileData, "\t", &tokPtr);
			editString1 += tokTemp;
			editString1 += "\r\n";

			tokTemp = strtok_s(NULL, "\t", &tokPtr);
			editString2 += tokTemp;
			editString2 += "\r\n";

			tokTemp = strtok_s(NULL, "\t", &tokPtr);
			editString3 += tokTemp;
			editString3 += "\r\n";
		}

		else
		{
			break;
		}
	}

	setTextItem(nID1, editString1);
	setTextItem(nID2, editString2);
	setTextItem(nID3, editString3);
}

void CMLV_3Dlg::setTextItem(int nID, CString fileData)
	/*
		setTextItem�Լ�

		nID�� fileData�� ������ �����ϴ� �Լ�
	*/
{
	SetDlgItemText(nID, fileData);
}

void CMLV_3Dlg::setTextItem(int nID, char* fileName)
	/*
		setTextItem�Լ�

		nID�� fileName ������ ������ �����ϴ� �Լ�
	*/
{
	char fileData[1024];
	FILE* fp;

	fString = "";
	fopen_s(&fp, fileName, "r");

	if(fp == NULL)
	{
		MessageBox("Error!");//show error msg box 
	}

	else
	{
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
}

void CMLV_3Dlg::setTextItem(int nID)
	/*
		setTextImte �Լ�
		
		CMLV_3Dlg Ŭ�������� ������ �ִ� fString�� ������ 
		nID�� �����Ѵ�.
	*/
{
	if(fString != "")
		SetDlgItemText(nID, fString);
	else
	{
		MessageBox("No has C file");
	}
}

void CMLV_3Dlg::setTextItem(CListCtrl* list, char* fileName)
	/*
		setTextItem�Լ�
		
		CListCtrl�� fileName ������ ������ �����ϴ� �Լ�
	*/
{
	char fileData[1024];

	char* tokTemp;
	char* tokPtr;
	CString listString0;
	CString listString1;
	CString listString2;
	CString listString3;
	CString listString4;
	CString listString5;

	FILE* fp;
	int colCount;
	int i;

	listString0.Empty();
	listString1.Empty();
	listString2.Empty();
	listString3.Empty();
	listString4.Empty();
	listString5.Empty();

	colCount = list->GetHeaderCtrl()->GetItemCount();
	i = 0;

	fopen_s(&fp, fileName, "r");

	if(fp == NULL)
	{
		MessageBox("Error!");//show error msg box 
	}

	while(1)
	{
		if( fgets(fileData, 1023, fp) != NULL)
		{
			if(colCount == 3)
			{
				tokTemp = strtok_s(fileData, "\t", &tokPtr);
				listString0 = tokTemp;
				list->InsertItem(i, listString0);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString1 = tokTemp;
				list->SetItemText(i, 1, listString1);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString2 = tokTemp;
				list->SetItemText(i, 2, listString2);

				i++;
			}

			else if(colCount == 5)
			{
				tokTemp = strtok_s(fileData, "\t", &tokPtr);
				listString0 = tokTemp;
				list->InsertItem(i, listString0);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString1 = tokTemp;
				list->SetItemText(i, 1, listString1);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString2 = tokTemp;
				list->SetItemText(i, 2, listString2);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString3 = tokTemp;
				list->SetItemText(i, 3, listString3);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString4 = tokTemp;
				list->SetItemText(i, 4, listString4);

				i++;
			}

			else if(colCount == 6)
			{
				tokTemp = strtok_s(fileData, "\t", &tokPtr);
				listString0 = tokTemp;
				list->InsertItem(i, listString0);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString1 = tokTemp;
				list->SetItemText(i, 1, listString1);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString2 = tokTemp;
				list->SetItemText(i, 2, listString2);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString3 = tokTemp;
				list->SetItemText(i, 3, listString3);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString4 = tokTemp;
				list->SetItemText(i, 4, listString4);

				tokTemp = strtok_s(NULL, "\t", &tokPtr);
				listString5 = tokTemp;
				list->SetItemText(i, 5, listString5);

				i++;
			}
		}

		else
		{
			break;
		}
	}
}

void CMLV_3Dlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
	/*
		OnTvnSelchangedTree1�Լ�
		��ư Ŭ������ TreeView ���� �ؽ�Ʈ ���� �޾ƿ´�.
	*/
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
	CString strData;

	char sectionNameBuf[512];
	char sectionHeaderBuf[512];

	HTREEITEM hSelectedItem = treeCtrl.GetSelectedItem();

	strcpy_s(sectionHeaderBuf, "IMAGE_SECTION_HEADER ");
	strcpy_s(sectionNameBuf, "SECTION ");

	if(hSelectedItem)
		strData = treeCtrl.GetItemText(hSelectedItem);

	if(strData.Compare(cvt.getPath()) == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(TRUE);

		setTextItem(IDC_EDIT1, IDC_EDIT2, IDC_EDIT3, "wholeEXEdata.txt");

	}

	else if(strData.Compare("IMAGE_DOS_HEADER") == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);

		homeList.DeleteAllItems();

		while(homeList.GetHeaderCtrl()->GetItemCount() > 0)
		{
			homeList.DeleteColumn(0);
		}

		homeList.InsertColumn(0, "Offset", LVCFMT_LEFT, 90);
		homeList.InsertColumn(1, "Data", LVCFMT_LEFT, 90);
		homeList.InsertColumn(2, "Field", LVCFMT_LEFT, 150);
		homeList.InsertColumn(3, "Size", LVCFMT_LEFT, 90);
		homeList.InsertColumn(4, "Name", LVCFMT_LEFT, 150);
		homeList.InsertColumn(5, "Explanation", LVCFMT_LEFT, 150);
		setTextItem(&homeList, "idhData.txt");
		setTextItem(IDC_HOME_LIST, "idhData.txt");
	}

	else if(strData.Compare("MS-DOS Stub Program") == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(TRUE);

		setTextItem(IDC_EDIT1, IDC_EDIT2, IDC_EDIT3, "dosStubData.txt");
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT3);
		pEdit->SetSel(5, 10);
	}

	else if(strData.Compare("IMAGE_NT_HEADERS") == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(TRUE);

		setTextItem(IDC_EDIT1, IDC_EDIT2, IDC_EDIT3, "inhOverView.txt");

	}

	else if(strData.Compare("Signature") == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);

		homeList.DeleteAllItems();

		while(homeList.GetHeaderCtrl()->GetItemCount() > 0)
		{
			homeList.DeleteColumn(0);
		}

		homeList.InsertColumn(0, "Offset", LVCFMT_LEFT, 90);
		homeList.InsertColumn(1, "Data", LVCFMT_LEFT, 90);
		homeList.InsertColumn(2, "Name", LVCFMT_LEFT, 150);
		homeList.InsertColumn(3, "Size", LVCFMT_LEFT, 90);
		homeList.InsertColumn(4, "Explanation", LVCFMT_LEFT, 150);

		setTextItem(&homeList, "inhSignature.txt");
	}

	else if(strData.Compare("IMAGE_FILE_HEADER") == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);

		homeList.DeleteAllItems();

		while(homeList.GetHeaderCtrl()->GetItemCount() > 0)
		{
			homeList.DeleteColumn(0);
		}

		homeList.InsertColumn(0, "Offset", LVCFMT_LEFT, 90);
		homeList.InsertColumn(1, "Data", LVCFMT_LEFT, 90);
		homeList.InsertColumn(2, "Size", LVCFMT_LEFT, 90);
		homeList.InsertColumn(3, "Name", LVCFMT_LEFT, 150);
		homeList.InsertColumn(4, "Explanation", LVCFMT_LEFT, 150);
		setTextItem(&homeList, "ifhData.txt");

	}

	else if(strData.Compare("IMAGE_OPTIONAL_HEADER") == 0)
	{
		(GetDlgItem(IDC_HOME_LIST))->ShowWindow(TRUE);
		(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
		(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);

		homeList.DeleteAllItems();

		while(homeList.GetHeaderCtrl()->GetItemCount() > 0)
		{
			homeList.DeleteColumn(0);
		}

		homeList.InsertColumn(0, "Offset", LVCFMT_LEFT, 90);
		homeList.InsertColumn(1, "Data", LVCFMT_LEFT, 90);
		homeList.InsertColumn(2, "Size", LVCFMT_LEFT, 90);
		homeList.InsertColumn(3, "Name", LVCFMT_LEFT, 150);
		homeList.InsertColumn(4, "Explanation", LVCFMT_LEFT, 150);

		setTextItem(&homeList, "iohData.txt");
	}

	else
	{
		for(int i = 0; i < 4; i++)
		{
			strcat_s(sectionHeaderBuf, cvt.getSectionName(i));
			strcat_s(sectionNameBuf, cvt.getSectionName(i));

			if(strData.Compare(sectionHeaderBuf) == 0)
			{
				(GetDlgItem(IDC_HOME_LIST))->ShowWindow(TRUE);
				(GetDlgItem(IDC_EDIT1))->ShowWindow(FALSE);
				(GetDlgItem(IDC_EDIT2))->ShowWindow(FALSE);
				(GetDlgItem(IDC_EDIT3))->ShowWindow(FALSE);
				homeList.DeleteAllItems();

				while(homeList.GetHeaderCtrl()->GetItemCount() > 0)
				{
					homeList.DeleteColumn(0);
				}

				homeList.InsertColumn(0, "Offset", LVCFMT_LEFT, 90);
				homeList.InsertColumn(1, "Size", LVCFMT_LEFT, 90);
				homeList.InsertColumn(2, "Data", LVCFMT_LEFT, 90);
				homeList.InsertColumn(3, "Name", LVCFMT_LEFT, 150);
				homeList.InsertColumn(4, "Explanation", LVCFMT_LEFT, 150);

				strcpy_s(sectionHeaderBuf, cvt.getSectionName(i));
				strcat_s(sectionHeaderBuf, "SectionInfo.txt");

				setTextItem(&homeList, sectionHeaderBuf);
				//setTextItem(IDC_HOME_LIST, sectionHeaderBuf);								
			}

			else if(strData.Compare(sectionNameBuf) == 0)
			{
				(GetDlgItem(IDC_HOME_LIST))->ShowWindow(FALSE);
				(GetDlgItem(IDC_EDIT1))->ShowWindow(TRUE);
				(GetDlgItem(IDC_EDIT2))->ShowWindow(TRUE);
				(GetDlgItem(IDC_EDIT3))->ShowWindow(TRUE);

				strcpy_s(sectionNameBuf, cvt.getSectionName(i));
				strcat_s(sectionNameBuf, "Section.txt");
				setTextItem(IDC_EDIT1, IDC_EDIT2, IDC_EDIT3, sectionNameBuf);
			}

			strcpy_s(sectionHeaderBuf, "IMAGE_SECTION_HEADER ");
			strcpy_s(sectionNameBuf, "SECTION ");
		}
	}
}

void CMLV_3Dlg::OnBnClickedCodeButton()
	/*
		OnBnClickedCodeButton�Լ�
	*/
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(convertDlg != NULL){
		convertDlg->SetFocus();
		//convertDlg->AnimateWindow(500,AW_BLEND);
		convertDlg->ShowWindow(SW_SHOW);
	}
	else{
		convertDlg = new convertDialog();
		convertDlg->Create(IDD_CONVERT_DIALOG);
		//convertDlg->AnimateWindow(500,AW_BLEND);
		convertDlg->ShowWindow(SW_SHOW);
	}
}

void CMLV_3Dlg::OnNcDestroy()
	/*
		OnNcDestroy�Լ�
	*/
{
	((convertDialog*)AfxGetMainWnd())->convertDlg = NULL;
	//delete this;
	CDialogEx::OnNcDestroy();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CMLV_3Dlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
	/*
		OnGetMinMaxInfo�Լ�
		���̾�α��� �ִ�,�ּ� ũ�⸦ �����ϴ� �Լ�
	*/
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	LPMINMAXINFO lpminmax = (LPMINMAXINFO)lpMMI;

	lpminmax->ptMinTrackSize.x = 1280; //�ּҰ�
	lpminmax->ptMinTrackSize.y = 768;
	lpminmax->ptMaxTrackSize.x = 1280;  //�ִ밪
	lpminmax->ptMaxTrackSize.y = 768;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}
