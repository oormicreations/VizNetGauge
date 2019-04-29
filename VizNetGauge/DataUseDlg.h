#pragma once


// CDataUseDlg dialog

class CDataUseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDataUseDlg)

public:
	CDataUseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataUseDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DATAUSE };
#endif

	LONGLONG m_lMaxData;
	LONG m_lThreshold;
	UINT m_uPeriod1;
	UINT m_uPeriod2;

	BOOL m_bWarn;
	BOOL m_bWarnPop;
	BOOL m_bWarnDing;
	BOOL m_bWarnVoice;
	BOOL m_bRenewDay;
	BOOL m_bRenewMonth;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckWarn();
	afx_msg void OnBnClickedCheckRen1();
	afx_msg void OnBnClickedCheckRen2();
};
