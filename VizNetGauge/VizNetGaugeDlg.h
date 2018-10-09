
// VizNetGaugeDlg.h : header file
//

#pragma once

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

#define VNG_TIMER			WM_USER + 200
#define VNG_MAX_INTERFACES	16
#define VNG_MAX_SAMPLES		256


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
	UINT m_uBkIntensity, m_uGridIntensityMin, m_uGridIntensityMaj, m_uGridSpacing;
	COLORREF m_crBarDn, m_crBarUp;
	UINT m_uWinSzMin, m_uWinSzMax;
	UINT m_uBarCount, m_uBarWidth, m_uBarHeight, m_uBarSpacing;
	UINT m_uTextIntensityTitle, m_uTextIntensityValue;

	IWbemRefresher			*pRefresher;
	IWbemHiPerfEnum			*pEnum;
	IWbemConfigureRefresher *pConfig;
	IWbemServices           *pNameSpace;
	IWbemLocator            *pWbemLocator;
	HRESULT					m_hrResult;
	BOOL					m_bIsInitHandles;
	CIMTYPE					m_lInterfaceNameType;
	CIMTYPE					m_lBytesReceivedPerSecType;
	CIMTYPE					m_lBytesSentPerSecType;
	LONG					m_lInterfaceNameHandle;
	LONG					m_lBytesReceivedPerSecHandle;
	LONG					m_lBytesSentPerSecHandle;

	UINT m_uDownloadSamples[VNG_MAX_INTERFACES][VNG_MAX_SAMPLES];
	UINT m_uUploadSamples[VNG_MAX_INTERFACES][VNG_MAX_SAMPLES];
	CString m_sNetInterfaces[VNG_MAX_INTERFACES];
	float m_fAveragingIntervel;
	float m_fAverageDownloadSpeed[VNG_MAX_INTERFACES];
	float m_fAverageUploadSpeed[VNG_MAX_INTERFACES];
	UINT m_uDownloadSpeed;
	UINT m_uUploadSpeed;
	UINT m_uSampleCount;
	UINT m_uMaxValue;

	UINT m_uUnit;
	CString m_sUnitName;
	UINT m_uSelectedInterface;
	CString m_sSelectedInterfaceName;

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
	float GetScaleRatio();

	BOOL InitWMI();
	BOOL GetStatsRefresher();
	void WMICleanup();

	void InitSamples();
	void InsertSample(int iInterface, DWORD dwDnValue, DWORD dwUpValue);


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
