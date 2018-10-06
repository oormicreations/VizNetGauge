
// VizNetGaugeDlg.h : header file
//

#pragma once


// CVizNetGaugeDlg dialog
class CVizNetGaugeDlg : public CDialogEx
{
// Construction
public:
	CVizNetGaugeDlg(CWnd* pParent = NULL);	// standard constructor

	UINT m_uVMaj, m_uVMin;
	UINT m_Timer;
	UINT m_uValueFontSize, m_uTextFontSize;
	CString m_sFont;
	UINT m_uDownloadSpeed, m_uUploadSpeed;
	UINT m_uBkIntensity, m_uGridIntensityMin, m_uGridIntensityMaj, m_uGridSpacing;
	COLORREF m_crBarDn, m_crBarUp;
	UINT m_uWinSzMin, m_uWinSzMax;
	UINT m_uBarCount, m_uBarWidth, m_uBarHeight, m_uBarSpacing;
	UINT m_uTextIntensityTitle, m_uTextIntensityValue;


	void InitDraw();
	void PaintGauge();
	void DrawBackground(CDC * pDC, CRect clRect);
	void DrawGrid(CDC * pDC, CRect clRect, BOOL bMajor);
	void DrawBars(CDC * pDC, CRect clRect);
	void DrawTextBackground(CDC * pDC, CRect clRect);
	void DrawText(CDC * pDC, CRect clRect);


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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
