#pragma once
#include "afxwin.h"


// convertDialog 대화 상자입니다.

class convertDialog : public CDialogEx
{
	DECLARE_DYNAMIC(convertDialog)

public:
	convertDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~convertDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONVERT_DIALOG };
private:
	void setTextItem(int nID);
	void setTextItem(int nID, char* fileName);
	virtual void PostNcDestroy();
	CString fString;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit assemblyEdit_2;
	convertDialog* convertDlg;
	CEdit convertEdit_2;
//	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
