
// VizNetGaugeDlg.h : header file
//

#pragma once

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include "NetHelper.h"
#include "AboutDlg.h"
#include "DataUseDlg.h"

#pragma comment(lib, "wbemuuid.lib")

#define VNG_MAX_INTERFACES		10
#define VNG_MAX_SAMPLES			256
#define VNG_BACKUP_INTERVAL		120

#define VNG_DEFAULT_TOPMOST		0
#define VNG_DEFAULT_UPDATEFREQ	1000
#define VNG_DEFAULT_UNIT		1000
#define VNG_DEFAULT_AVE			10.0f
#define VNG_DEFAULT_THEME		0
#define VNG_DEFAULT_INTERFACE	0
//#define VNG_DEFAULT_

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
	CNetHelper m_NetHelper;

	UINT m_uVMaj, m_uVMin;
	UINT m_uTimer, m_uTimerDelay;
	BOOL m_bIsUpload;
	UINT m_uValueFontSize, m_uTextFontSize;
	CString m_sFont;
	UINT m_uBkIntensity, m_uGridIntensityMin, m_uGridIntensityMaj, m_uGridSpacing;
	COLORREF m_crBarDn, m_crBarUp, m_crBackground, m_crDataBackground;
	UINT m_uWinSzMin, m_uWinSzMax;
	UINT m_uBarCount, m_uBarWidth, m_uBarHeight, m_uBarSpacing;
	UINT m_uTextIntensityTitle, m_uTextIntensityValue, m_uSelectedTheme;

	IWbemRefresher			*pRefresher, *pRefresherRaw;
	IWbemHiPerfEnum			*pEnum, *pEnumRaw;
	IWbemConfigureRefresher *pConfig, *pConfigRaw;
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

	BOOL					m_bIsInitHandles2;
	CIMTYPE					m_lBytesReceivedPerSecType2;
	CIMTYPE					m_lBytesSentPerSecType2;
	LONG					m_lBytesReceivedPerSecHandle2;
	LONG					m_lBytesSentPerSecHandle2;

	LONGLONG				m_lBytesDownHisLast;
	LONGLONG				m_lBytesUpHisLast;
	LONGLONG				m_lBytesDownHis;
	LONGLONG				m_lBytesUpHis;
	LONGLONG				m_lBytesDown;
	LONGLONG				m_lBytesUp;
	LONGLONG				m_lBytesTotal;
	LONGLONG				m_lBytesRem;
	UINT					m_uBackUpInterval;

	UINT m_uDownloadSamples[VNG_MAX_INTERFACES][VNG_MAX_SAMPLES];
	UINT m_uUploadSamples[VNG_MAX_INTERFACES][VNG_MAX_SAMPLES];
	CString m_sNetInterfaces[VNG_MAX_INTERFACES];
	UINT m_uNetInterfaceCount;
	UINT m_uMaxValue;
	float m_fAveragingIntervel;
	float m_fAverageDownloadSpeed[VNG_MAX_INTERFACES];
	float m_fAverageUploadSpeed[VNG_MAX_INTERFACES];
	float m_fAverageTotalDownloadSpeed;
	float m_fAverageTotalUploadSpeed;

	UINT m_uUnit;
	CString m_sUnitName;
	UINT m_uSelectedInterface;
	CString m_sSelectedInterfaceName;
	BOOL m_IsAutoDetect;
	BOOL m_IsWarnSent;

	typedef struct tagVNGSET
	{
		BOOL m_bSetTopMost;
		UINT m_uSetUpdateFreq;
		UINT m_uSetUnit;
		float m_uSetAve;
		UINT m_uSetTheme;
		UINT m_uSetSelInterface;
		UINT m_uSetUnused1;
		UINT m_uSetUnused2;
		UINT m_uSetUnused3;
		UINT m_uSetUnused4;
		LONG m_lMaxData;
		LONG m_lThreshold;
		UINT m_uPeriod1;
		UINT m_uPeriod2;
		BOOL m_bWarn;
		BOOL m_bWarnPop;
		BOOL m_bWarnDing;
		BOOL m_bWarnVoice;
		BOOL m_bRenewDay;
		BOOL m_bRenewMonth;
		LONG m_lBytesDown;
		LONG m_lBytesUp;

	} VNGSET;

	CDataUseDlg m_DataUseDlg;

	void InitDraw();
	void Plot();
	void PaintGauge();
	void RePaint();
	void DrawBackground(CDC * pDC, CRect clRect, CRect r);
	void DrawGrid(CDC * pDC, CRect clRect, BOOL bMajor);
	void DrawBars(CDC * pDC, CRect clRect);
	void DrawTextBackground(CDC * pDC, CRect clRect);
	void DrawSpeedText(CDC * pDC, CRect clRect);
	void DrawDataText(CDC * pDC, CRect clRect);
	void DrawInfoText(CDC * pDC, CRect clRect);
	void DrawVignette(CDC * pDC, CRect clRect);
	float GetScaleRatio();

	BOOL InitWMI();
	BOOL GetStatsRefresher();
	BOOL GetStatsRefresherRaw();
	void WMICleanup();

	void InitSamples();
	void InsertSample(int iInterface, DWORD dwDnValue, DWORD dwUpValue);
	void GetTotalAllInterfaces();
	void AutodetectActiveInterface();

	void SetupMinimizeToTray();
	void MinimizeToTray();
	void MaximizeFromTray();
	void OnTrayLButtonDown(CPoint pt);
	void OnTrayRButtonDown(CPoint pt);
	void UpdateMenuInterfaceNames();
	void ClearCheckInterface();

	void SaveSettings();
	BOOL LoadSettings();
	void ResetSettings();
	void ApplySettingsToMenu();
	void SaveHis();
	void Warn();

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
	afx_msg void OnCommandRangeInterfaces(UINT nID);
	afx_msg void OnInterfacesTotalofall();
	afx_msg void OnInterfacesAuto();
	afx_msg void OnHelpOnlinehelp();
	afx_msg void OnHelpGetmoreapps();
	afx_msg void OnHelpCheckforupdates();
	afx_msg void OnHelpAbout();
	afx_msg void OnOptionsSettings();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnOptionsConfiguredatausage();
	afx_msg void OnOptionsResetdatausage();
	afx_msg void OnOptionsRestoredatabacktohistorical();
};
