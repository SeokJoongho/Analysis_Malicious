
// MLV_3Dlg.cpp : 구현 파일
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
// CMLV_3Dlg 대화 상자

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
		DoDataExchange함수
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
		선언된 부분입니다.
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


// CMLV_3Dlg 메시지 처리기

BOOL CMLV_3Dlg::OnInitDialog()
{
	/*
		OnInitDialog함수
		Dialog의 컨트롤들을 초기화 하기 위한 함수.
	*/
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	if(start_db() == 1){
		HBITMAP hbit_1 = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
		logo.SetBitmap(hbit_1);

		CRect rt_1;
		GetClientRect(&rt_1);
		logo.SetWindowPos(NULL, 0, 0, rt_1.Width(),rt_1.Height(), SWP_SHOWWINDOW);

		edit_bk_brush.CreateSolidBrush(RGB(230, 230, 230));
		SetDlgItemText(IDC_MESSAGE, "분석에는 3분에서 5분의 시간이 소요됩니다.");
		SetDlgItemText(IDC_MESSAGE_2, "분석할 파일을 선택해주세요.");

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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

BOOL CMLV_3Dlg::PreTranslateMessage(MSG* pMsg)
	/*
		PreTranslateMessage함수
		
	*/
{

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
		OnPaint함수
		프로그램에서 기본적으로 생성해주는 함수입니다.
	*/
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HBRUSH CMLV_3Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	/*
		OnCtlColor함수
		프로그램에서 기본적으로 생성해주는 함수입니다.
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
	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMLV_3Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMLV_3Dlg::OnDestroy()
	/*
		OnDestroy함수
	*/
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	edit_bk_brush.DeleteObject();

}

void CMLV_3Dlg::OnBnClickedAnalysisButton()
	/*
		onBnClickedAnalysisButton함수
		파일분석모드 버튼의 컨트롤 알림을 처리하기 위한 함수
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/

{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedStatisticsButton함수
		함수분석모드 버튼의 컨트롤 알림을 처리하기 위한 함수
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedHomeButton함수
		메인화면 버튼의 컨트롤 알림을 처리하기 위한 함수
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedFind함수
		파일분석모드에 있는 파일오픈버튼(FIND)의 컨트롤 알림을 처리하기 위한 함수
		exe파일을 오픈하여 분석 후 DB에서 PE header, import, section 정보를 받아와 
		Listcontrol에 배치.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnLbnSelchangeFunctionList함수
		함수분석모드에 있는 함수 목록을 보여주는 Listbox contol의 컨트롤 알림을 처리하기 위한 함수
		분석한 exe파일에 사용된 함수 정보을 DB에서 받아와 Listbox control에 배치.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

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
		OnBnClickedReport함수
		번역모드 버튼의 컨트롤 알림을 처리하기 위한 함수.
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CMLV_3Dlg::OnLvnItemchangedListImport(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void CMLV_3Dlg::OnLvnItemchangedListHeader(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

}

void CMLV_3Dlg::OnBnClickedCancel()
	/*
		OnBnClickedCancel함수	
		종료버튼의 컨트롤 알림을 처리하기 위한 함수.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedAnalysis2함수	
		분석모드의 analysis 버튼의 컨트롤 알림을 처리하기 위한 함수.

	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	//IMAGE_NT_HEADER의 내용을 제대로 가져오지 못하면 에러처리
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

	else if(funcReturnValue == 0) //IMAGE_NT_HEADER의 내용을 제대로 가져왔을 때 이후로 진행
	{
		funcReturnValue = cvt.findIATData();

		if(funcReturnValue == 0)	//IMAGE ADDRESS TABLE의 내용을 제대로 가져왔을 때 이후로 진행
		{
			if(cvt.sendPEHeaderInfo() == 1)	//database에 binary file의 내용이 없으면 database에 binary file의 정보 insert
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

			if(funcReturnValue == -4)	//assembly language로 번역이 제대로 안되었을 경우 에러 처리
			{
				MessageBox("Code Section Offset setting failed");
			}

			else if(funcReturnValue == -5)
			{
				MessageBox("failed to access data section");
			}

			else if(funcReturnValue == 0)	//assembly language로 번역이 제대로 되었을 경우 analysis 진행
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
		OnBnClickedResize1함수
		번역모드의 Assembly 코드를 큰 창으로 보여주는
		Edit contol을 여는 버튼[+]의 컨트롤 알림을 처리하기 위한 함수.
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/

{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedResize2함수
		번역모드의 C언어로 mapping 코드를 큰 창으로 보여주는
		Edit contol을 여는 버튼[+]의 컨트롤 알림을 처리하기 위한 함수.
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/

{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedResizeClose1함수
		번역모드의 Aseembly 코드를 큰 창으로 보여주는
		Edit contol의 닫기 버튼 컨트롤 알림을 처리하기 위한 함수.
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedResizeClose2함수
		번역모드의 C언어로 mapping된 코드를 큰 창으로 보여주는
		Edit contol의 닫기 버튼 컨트롤 알림을 처리하기 위한 함수.
		각각의 컨트롤들을 ShowWindow함수를 통해 프로그램UI에서 visualization하였음.
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		setTextItem함수
		
		fileName 파일의 내용을 nID1, nID2, nID3에 나누어 삽입하는 함수
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
		setTextItem함수

		nID에 fileData의 내용을 삽입하는 함수
	*/
{
	SetDlgItemText(nID, fileData);
}

void CMLV_3Dlg::setTextItem(int nID, char* fileName)
	/*
		setTextItem함수

		nID에 fileName 파일의 내용을 삽입하는 함수
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
		setTextImte 함수
		
		CMLV_3Dlg 클래스에서 가지고 있는 fString의 내용을 
		nID에 삽입한다.
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
		setTextItem함수
		
		CListCtrl에 fileName 파일의 내용을 삽입하는 함수
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
		OnTvnSelchangedTree1함수
		버튼 클릭으로 TreeView 에서 텍스트 값을 받아온다.
	*/
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnBnClickedCodeButton함수
	*/
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		OnNcDestroy함수
	*/
{
	((convertDialog*)AfxGetMainWnd())->convertDlg = NULL;
	//delete this;
	CDialogEx::OnNcDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMLV_3Dlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
	/*
		OnGetMinMaxInfo함수
		다이얼로그의 최대,최소 크기를 지정하는 함수
	*/
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	LPMINMAXINFO lpminmax = (LPMINMAXINFO)lpMMI;

	lpminmax->ptMinTrackSize.x = 1280; //최소값
	lpminmax->ptMinTrackSize.y = 768;
	lpminmax->ptMaxTrackSize.x = 1280;  //최대값
	lpminmax->ptMaxTrackSize.y = 768;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}
