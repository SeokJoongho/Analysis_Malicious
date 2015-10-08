
// MLV_3Dlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "convertDialog.h"

// CMLV_3Dlg ��ȭ
class CMLV_3Dlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMLV_3Dlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MLV_3_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CBrush edit_bk_brush;
	CToolTipCtrl m_tip_ctrl;
	int dberr;
	int newFile;
	HTREEITEM* imageSectionTree;
	HTREEITEM* sectionTree;
	HTREEITEM top;
	HTREEITEM do_1;
	HTREEITEM do_2;
	HTREEITEM do_3;
	
public:
	CListCtrl m_ListH;
	CListCtrl m_ListS;
	CListBox functionList;
	CBitmapButton reportButton;
	CBitmapButton functionButton;
	CFont m_font;
	CString filename;
	CString fString;
	CStatic logo;
	CBitmapButton analysisButton;
	CBitmapButton statisticsButton;
	CBitmapButton homeButton;
	CBitmapButton closeButton;
	CBitmapButton findButton;
	CBitmapButton assemblyButton;
	CBitmapButton convertButton;
	CBitmapButton resizeButton_1;
	CBitmapButton resizeButton_2;
	CListCtrl m_ListI;
	CBitmapButton resizeCloseButton_1;
	CBitmapButton resizeCloseButton_2;
	CAnimateCtrl loadingAnimation;
	CBitmapButton analysisButton_2;
	CEdit assemblyEdit;
	CEdit convertEdit;
	CTreeCtrl treeCtrl;
	CBitmapButton parameterButton;
	CBitmapButton referenceButton;
	CString pathName;
	CStatic message;
	CBitmapButton codeButton;

	afx_msg void OnBnClickedAnalysisButton();
	afx_msg void OnBnClickedStatisticsButton();
	afx_msg void OnBnClickedHomeButton();
	afx_msg void OnBnClickedFind();
	afx_msg void OnBnClickedResize1();
	afx_msg void OnBnClickedResize2();
	afx_msg void OnDestroy();
	afx_msg void OnLbnSelchangeFunctionList();
	afx_msg void OnBnClickedReport();
	afx_msg void OnLvnItemchangedListSection(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListImport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditsection();
	afx_msg void OnLvnItemchangedListHeader(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAnalysis2();
	afx_msg void OnBnClickedResizeClose1();
	afx_msg void OnBnClickedResizeClose2();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeAssemblyEdit();
	void setTextItem(int nID1, int nID2, int nID3, char* fileName);
	void setTextItem(int nID, CString fileData);
	void setTextItem(int nID, char* fileName);
	void setTextItem(int nID);
	void setTextItem(CListCtrl* list, char* fileName);
	afx_msg void OnNcDestroy();	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCodeButton();
	CListCtrl homeList;
	CString m_Edit1;
//	afx_msg void OnChangeEdit1();
	CString m_Edit2;
//	afx_msg void OnChangeEdit2();
	CString m_Edit3;
//	afx_msg void OnChangeEdit3();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
