
// VizNetGaugeDlg.h : header file
//

#pragma once

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

#define VNG_TIMER WM_USER + 200



// CVizNetGaugeDlg dialog
class CVizNetGaugeDlg : public CDialogEx
{
// Construction
public:
	CVizNetGaugeDlg(CWnd* pParent = NULL);	// standard constructor

	UINT m_uVMaj, m_uVMin;
	UINT m_uTimer, m_uTimerDelay;
	BOOL m_bIsUpload;
	UINT m_uValueFontSize, m_uTextFontSize;
	CString m_sFont;
	UINT m_uDownloadSpeed, m_uUploadSpeed;
	UINT m_uBkIntensity, m_uGridIntensityMin, m_uGridIntensityMaj, m_uGridSpacing;
	COLORREF m_crBarDn, m_crBarUp;
	UINT m_uWinSzMin, m_uWinSzMax;
	UINT m_uBarCount, m_uBarWidth, m_uBarHeight, m_uBarSpacing;
	UINT m_uTextIntensityTitle, m_uTextIntensityValue;

	IWbemRefresher *pRefresher;// = NULL;
	IWbemHiPerfEnum *pEnum;// = NULL;


	void InitDraw();
	void Plot();
	void PaintGauge();
	void DrawBackground(CDC * pDC, CRect clRect);
	void DrawGrid(CDC * pDC, CRect clRect, BOOL bMajor);
	void DrawBars(CDC * pDC, CRect clRect);
	void DrawTextBackground(CDC * pDC, CRect clRect);
	void DrawSpeedText(CDC * pDC, CRect clRect);
	void DrawInfoText(CDC * pDC, CRect clRect);
	void DrawVignette(CDC * pDC, CRect clRect);

	BOOL InitWMI();
	BOOL GetStats();
	BOOL GetStatsRefresher();


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
	afx_msg void OnEnterSizeMove();
	afx_msg void OnExitSizeMove();
};
