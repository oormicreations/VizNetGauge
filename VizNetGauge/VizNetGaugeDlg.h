
// VizNetGaugeDlg.h : header file
//

#pragma once

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

#define VNG_MAX_INTERFACES	10
#define VNG_MAX_SAMPLES		256

#define VNG_TIMER			WM_USER + 200
#define WM_TRAY_MESSAGE		WM_USER + 201
#define WM_TRAY_RESTORE		WM_USER + 202


// CVizNetGaugeDlg dialog
class CVizNetGaugeDlg : public CDialogEx
{
// Construction
public:
	CVizNetGaugeDlg(CWnd* pParent = NULL);	// standard constructor
	
	CMenu m_MenuPopup, m_MenuTray;
	NOTIFYICONDATA m_TrayData;
	BOOL m_bMinimized, m_bTopmostMode;

	UINT m_uVMaj, m_uVMin;
	UINT m_uTimer, m_uTimerDelay;
	BOOL m_bIsUpload;
	UINT m_uValueFontSize, m_uTextFontSize;
	CString m_sFont;
	UINT m_uBkIntensity, m_uGridIntensityMin, m_uGridIntensityMaj, m_uGridSpacing;
	COLORREF m_crBarDn, m_crBarUp, m_crBackground;
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
	UINT m_uNetInterfaceCount;
	float m_fAveragingIntervel;
	float m_fAverageDownloadSpeed[VNG_MAX_INTERFACES];
	float m_fAverageUploadSpeed[VNG_MAX_INTERFACES];
	//UINT m_uDownloadSpeed;
	//UINT m_uUploadSpeed;
	//UINT m_uSampleCount;
	UINT m_uMaxValue;

	UINT m_uUnit;
	CString m_sUnitName;
	UINT m_uSelectedInterface;
	CString m_sSelectedInterfaceName;

	void InitDraw();
	void Plot();
	void PaintGauge();
	void RePaint();
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

	void SetupMinimizeToTray();
	void MinimizeToTray();
	void MaximizeFromTray();
	void OnTrayLButtonDown(CPoint pt);
	void OnTrayRButtonDown(CPoint pt);
	void UpdateMenuInterfaceNames();

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
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg LRESULT RestoreHandler(WPARAM wp, LPARAM lp);
	afx_msg void OnOptionsAlwaysontop();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);

	afx_msg void OnOptionsMinimizetotray();
	afx_msg void OnTrayRestore();
	afx_msg void OnTrayExit();
	afx_msg void OnUpdatespeedSlow();
	afx_msg void OnMenuExit();
	afx_msg void OnUpdatespeedMedium();
	afx_msg void OnUpdatespeedFast();
	afx_msg void OnUnitsBitspersec();
	afx_msg void OnUnitsKilobitspersec();
	afx_msg void OnUnitsMegabitspersec();
	afx_msg void OnUnitsGigabitspersec();
	afx_msg void OnAveraging10samples();
	afx_msg void OnAveraging30sample();
	afx_msg void OnAveraging60samples();
	afx_msg void OnAveragingInstantaneous();
	afx_msg void OnThemesAquapale();
	afx_msg void OnThemesRedorange();
	afx_msg void OnThemesBluegreen();
	afx_msg void OnThemesGreywhite();
	afx_msg void OnInterfacesNif1();
	afx_msg void OnUpdateInterfacesNif1(CCmdUI *pCmdUI);
	afx_msg void OnCommandRangeInterfaces(UINT nID);
};
