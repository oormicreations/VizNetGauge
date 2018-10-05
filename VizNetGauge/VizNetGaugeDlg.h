
// VizNetGaugeDlg.h : header file
//

#pragma once


// CVizNetGaugeDlg dialog
class CVizNetGaugeDlg : public CDialogEx
{
// Construction
public:
	CVizNetGaugeDlg(CWnd* pParent = NULL);	// standard constructor

	UINT m_Timer;
	UINT m_uValueFontSize, m_uTextFontSize;
	CString m_sFont;
	UINT m_uDownloadSpeed, m_uUploadSpeed;

	void PaintGauge();


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIZNETGAUGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
