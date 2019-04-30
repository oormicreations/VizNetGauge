
// VizNetGaugeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VizNetGauge.h"
#include "VizNetGaugeDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>
#pragma comment(lib,"Winmm.lib")



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVizNetGaugeDlg dialog



CVizNetGaugeDlg::CVizNetGaugeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VIZNETGAUGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_uWinSzMin = 150;
	m_uWinSzMax = 800;

}

void CVizNetGaugeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVizNetGaugeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
	ON_WM_ENTERSIZEMOVE()
	ON_WM_EXITSIZEMOVE()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(WM_TRAY_MESSAGE, OnTrayNotify)
	ON_MESSAGE(WM_TRAY_RESTORE, RestoreHandler)
	ON_COMMAND(ID_OPTIONS_ALWAYSONTOP, &CVizNetGaugeDlg::OnOptionsAlwaysontop)
	ON_COMMAND(ID_OPTIONS_MINIMIZETOTRAY, &CVizNetGaugeDlg::OnOptionsMinimizetotray)
	ON_COMMAND(ID_TRAY_RESTORE, &CVizNetGaugeDlg::OnTrayRestore)
	ON_COMMAND(ID_TRAY_EXIT, &CVizNetGaugeDlg::OnTrayExit)
	ON_COMMAND(ID_UPDATESPEED_SLOW, &CVizNetGaugeDlg::OnUpdatespeedSlow)
	ON_COMMAND(ID_MENU_EXIT, &CVizNetGaugeDlg::OnMenuExit)
	ON_COMMAND(ID_UPDATESPEED_MEDIUM, &CVizNetGaugeDlg::OnUpdatespeedMedium)
	ON_COMMAND(ID_UPDATESPEED_FAST, &CVizNetGaugeDlg::OnUpdatespeedFast)
	ON_COMMAND(ID_UNITS_BITSPERSEC, &CVizNetGaugeDlg::OnUnitsBitspersec)
	ON_COMMAND(ID_UNITS_KILOBITSPERSEC, &CVizNetGaugeDlg::OnUnitsKilobitspersec)
	ON_COMMAND(ID_UNITS_MEGABITSPERSEC, &CVizNetGaugeDlg::OnUnitsMegabitspersec)
	ON_COMMAND(ID_UNITS_GIGABITSPERSEC, &CVizNetGaugeDlg::OnUnitsGigabitspersec)
	ON_COMMAND(ID_AVERAGING_10SAMPLES, &CVizNetGaugeDlg::OnAveraging10samples)
	ON_COMMAND(ID_AVERAGING_30SAMPLE, &CVizNetGaugeDlg::OnAveraging30sample)
	ON_COMMAND(ID_AVERAGING_60SAMPLES, &CVizNetGaugeDlg::OnAveraging60samples)
	ON_COMMAND(ID_AVERAGING_INSTANTANEOUS, &CVizNetGaugeDlg::OnAveragingInstantaneous)
	ON_COMMAND(ID_THEMES_AQUAPALE, &CVizNetGaugeDlg::OnThemesAquapale)
	ON_COMMAND(ID_THEMES_REDORANGE, &CVizNetGaugeDlg::OnThemesRedorange)
	ON_COMMAND(ID_THEMES_BLUEGREEN, &CVizNetGaugeDlg::OnThemesBluegreen)
	ON_COMMAND(ID_THEMES_GREYWHITE, &CVizNetGaugeDlg::OnThemesGreywhite)
	ON_COMMAND_RANGE(ID_INTERFACES_NIF1, ID_INTERFACES_NIF14, &CVizNetGaugeDlg::OnCommandRangeInterfaces)
	ON_COMMAND(ID_INTERFACES_TOTALOFALL, &CVizNetGaugeDlg::OnInterfacesTotalofall)
	ON_COMMAND(ID_INTERFACES_AUTO, &CVizNetGaugeDlg::OnInterfacesAuto)
	ON_COMMAND(ID_HELP_ONLINEHELP, &CVizNetGaugeDlg::OnHelpOnlinehelp)
	ON_COMMAND(ID_HELP_GETMOREAPPS, &CVizNetGaugeDlg::OnHelpGetmoreapps)
	ON_COMMAND(ID_HELP_CHECKFORUPDATES, &CVizNetGaugeDlg::OnHelpCheckforupdates)
	ON_COMMAND(ID_HELP_ABOUT, &CVizNetGaugeDlg::OnHelpAbout)
	ON_COMMAND(ID_OPTIONS_SETTINGS, &CVizNetGaugeDlg::OnOptionsSettings)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_OPTIONS_CONFIGUREDATAUSAGE, &CVizNetGaugeDlg::OnOptionsConfiguredatausage)
	ON_COMMAND(ID_OPTIONS_RESETDATAUSAGE, &CVizNetGaugeDlg::OnOptionsResetdatausage)
	ON_COMMAND(ID_OPTIONS_RESTOREDATABACKTOHISTORICAL, &CVizNetGaugeDlg::OnOptionsRestoredatabacktohistorical)
END_MESSAGE_MAP()


// CVizNetGaugeDlg message handlers
LRESULT CVizNetGaugeDlg::RestoreHandler(WPARAM wp, LPARAM lp)
{
	MaximizeFromTray();
	return 0;
}

BOOL CVizNetGaugeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_uVMaj = 1;
	m_uVMin = 1;

	m_bTopmostMode = FALSE;
	m_bMinimized = FALSE;

	m_MenuPopup.LoadMenu(IDR_MENU_POPUP);
	SetupMinimizeToTray();


	InitSamples();
	InitDraw();
	InitWMI();
	if (!LoadSettings()) ResetSettings();
	Renew();

	//m_NetHelper.ReportUsage(_T("VizNetGauge"), m_uVMaj * 10 + m_uVMin);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVizNetGaugeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		RePaint();//without refreshing data

		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVizNetGaugeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVizNetGaugeDlg::InitSamples()
{
	for (int i = 0; i < VNG_MAX_INTERFACES; i++)
	{
		for (int j = 0; j < VNG_MAX_SAMPLES; j++)
		{
			m_uDownloadSamples[i][j] = 0;
			m_uUploadSamples[i][j] = 0;
		}

		m_fAverageDownloadSpeed[i] = 0.0f;
		m_fAverageUploadSpeed[i] = 0.0f;
	}

	//m_fAveragingIntervel = 10.0f; //10sec for 1Hz update rate
	//m_uUnit = 1000; //kbps
	//m_sUnitName = _T("Kbps");
	//m_uSelectedInterface = 0; //total of all interfaces
	m_uMaxValue = 0;
	m_uNetInterfaceCount = 0;
	m_fAverageTotalDownloadSpeed = 0.0f;
	m_fAverageTotalUploadSpeed = 0.0f;
	m_IsAutoDetect = FALSE;

//	m_lBytesTotal = 0;
	m_lBytesDown = 0;
	m_lBytesUp = 0;
	m_lBytesRem = 0;
	m_lBytesDownHis = 0;
	m_lBytesUpHis = 0;
	m_lBytesDownHisLast = 0;
	m_lBytesUpHisLast = 0;
	m_lBytesUpInitial = 0;
	m_lBytesDownInitial = 0;

	m_uBackUpInterval = 0;
	m_IsWarnSent = FALSE;
	m_IsRenewed1 = FALSE;
	m_IsRenewed2 = FALSE;
	m_IsInitial = TRUE;


}

void CVizNetGaugeDlg::InitDraw()
{
	m_bIsUpload = FALSE;
	
	m_uBkIntensity = 60;
	m_uGridIntensityMin = 65;
	m_uGridIntensityMaj = 75;
	m_uGridSpacing = 10;

	//m_crBarDn = RGB(20, 200, 200);
	//m_crBarUp = RGB(200, 200, 150);
	//m_crBackground = RGB(m_uBkIntensity, m_uBkIntensity, m_uBkIntensity);
	//m_uSelectedTheme = 0;

	//m_uTimerDelay = 1000; //1 Hz

	m_uBarWidth = 5;
	m_uBarHeight = 50;
	m_uBarSpacing = 2;

	m_uTextIntensityTitle = 170;
	m_uTextIntensityValue = 255;
	m_uValueFontSize = 20;
	m_uTextFontSize = 8;
	m_sFont = _T("Calibri");

	//m_uTimer = SetTimer(VNG_TIMER, m_uTimerDelay, NULL);
	m_crDataBackground = RGB(50, 50, 50);

}

void CVizNetGaugeDlg::Plot()
{
	GetStatsRefresher();
	GetStatsRefresherRaw();
	GetTotalAllInterfaces();
	AutodetectActiveInterface();
	RePaint();
	Warn();
}

void CVizNetGaugeDlg::RePaint()
{
	m_bIsUpload = FALSE;
	PaintGauge();
	m_bIsUpload = TRUE;
	PaintGauge();
}

void CVizNetGaugeDlg::PaintGauge()
{
	CRect rClient;
	GetClientRect(rClient);
	CRect rDClient(rClient);

	int rcw = rClient.Width();
	int rch = rClient.Height();
	//check if minimized
	if ((rcw < 1) || (rch < 1)) return;

	rClient.bottom = rClient.bottom / 2;

	CDC *pDC = this->GetDC();
	CDC * dcMem = new CDC();
	dcMem->CreateCompatibleDC(pDC);

	CBitmap * bitMap = new CBitmap();
	bitMap->CreateCompatibleBitmap(pDC, rClient.Width(), rClient.Height());
	CBitmap * oldMap = dcMem->SelectObject(bitMap);

	rDClient.bottom = (LONG)((float)rClient.bottom *0.9f);
	if (rDClient.Height() < 150)rDClient.bottom = (LONG)((float)rClient.bottom *0.8f);

	//background
	DrawBackground(dcMem, rClient, rDClient);

	//grids
	DrawGrid(dcMem, rDClient, FALSE);
	DrawGrid(dcMem, rDClient, TRUE);

	//bars
	DrawBars(dcMem, rDClient);

	//Text background
	//DrawTextBackground(dcMem, rClient);

	//Text
	DrawInfoText(dcMem, rDClient);
	DrawSpeedText(dcMem, rDClient);
	DrawDataText(dcMem, rDClient);

	//Vignette
	//DrawVignette(dcMem, rClient);

	//render
	int iYpos = (m_bIsUpload) * rClient.bottom;
	pDC->BitBlt(rClient.left, rClient.top + iYpos, rClient.Width(), rClient.Height(), dcMem, 0, 0, SRCCOPY);

	//clean up
	dcMem->SelectObject(oldMap);
	bitMap->DeleteObject();
	delete bitMap;
	dcMem->DeleteDC();
	delete dcMem;
	ReleaseDC(pDC);

}

void CVizNetGaugeDlg::DrawBackground(CDC * pDC, CRect clRect, CRect r)
{

	//COLORREF crBack = RGB(m_uBkIntensity, m_uBkIntensity, m_uBkIntensity);
	pDC->FillSolidRect(clRect, m_crBackground);

	int top = (int)((float)clRect.bottom*0.9f);
	if(r.Height()<150)top = (int)((float)clRect.bottom*0.8f);
	CRect rData(clRect.left, top, clRect.right, clRect.bottom);
	pDC->FillSolidRect(rData, m_crDataBackground);

}

void CVizNetGaugeDlg::DrawGrid(CDC * pDC, CRect clRect, BOOL bMajor)
{
	UINT uIntensity = m_uGridIntensityMin;
	UINT uSpacing = m_uGridSpacing;

	if (bMajor)
	{
		uIntensity = m_uGridIntensityMaj;
		uSpacing = m_uGridSpacing * 10;
	}

	COLORREF crGrid = RGB(uIntensity, uIntensity, uIntensity);

	CPen pnGrid;
	pnGrid.CreatePen(0, 1, crGrid);
	CPen * oldPen = pDC->SelectObject(&pnGrid);

	int iGridHCount = clRect.Height() / uSpacing;
	int iGridVCount = clRect.Width() / uSpacing;

	for (int i = 0; i <= iGridVCount; i++)
	{
		pDC->MoveTo(uSpacing * i, 0);
		pDC->LineTo(uSpacing * i, clRect.bottom);
	}
	//for (int i = 0; i <= iGridHCount; i++)
	//{
	//	pDC->MoveTo(0, uSpacing * i);
	//	pDC->LineTo(clRect.right, uSpacing * i);
	//}
	for (int i = 0; i <= iGridHCount; i++)
	{
		pDC->MoveTo(0,  (clRect.bottom - (uSpacing *i)));
		pDC->LineTo(clRect.right, (clRect.bottom - (uSpacing *i)));
	}

	pDC->SelectObject(oldPen);
	pnGrid.DeleteObject();

}

void CVizNetGaugeDlg::DrawBars(CDC * pDC, CRect clRect)
{
	CRect rBar;
	UINT uMaxBarHeight = (UINT)clRect.Height()/2;
	int iBarVOffset = 1;
	int iXpos = 0;
	UINT uMax = 0;
	m_uBarCount = 0;
	m_uMaxValue = 0;

	//bar count
	m_uBarCount = clRect.Width() / (m_uBarWidth + m_uBarSpacing);

	if (m_bIsUpload)
	{
		//normalize
		for (UINT i = 0; i < m_uBarCount; i++)
		{
			if (m_uUploadSamples[m_uSelectedInterface][i] > uMax)uMax = m_uUploadSamples[m_uSelectedInterface][i];
		}
		if (uMax < 1)return;

		for (UINT i = 0; i < m_uBarCount; i++)
		{
			m_uBarHeight = (uMaxBarHeight * m_uUploadSamples[m_uSelectedInterface][i]) / uMax;
			rBar.SetRect(0 + iXpos, clRect.Height() - m_uBarHeight - iBarVOffset, m_uBarWidth + iXpos, clRect.Height() - iBarVOffset);
			pDC->FillSolidRect(rBar, m_crBarUp);
			iXpos = iXpos + m_uBarSpacing + m_uBarWidth;
		}
	}
	else
	{
		//normalize
		for (UINT i = 0; i < m_uBarCount; i++)
		{
			if (m_uDownloadSamples[m_uSelectedInterface][i] > uMax)uMax = m_uDownloadSamples[m_uSelectedInterface][i];
		}
		if (uMax < 1)return;

		for (UINT i = 0; i < m_uBarCount; i++)
		{
			m_uBarHeight = (uMaxBarHeight * m_uDownloadSamples[m_uSelectedInterface][i]) / uMax;
			rBar.SetRect(0 + iXpos, clRect.Height() - m_uBarHeight - iBarVOffset, m_uBarWidth + iXpos, clRect.Height() - iBarVOffset);
			pDC->FillSolidRect(rBar, m_crBarDn);
			iXpos = iXpos + m_uBarSpacing + m_uBarWidth;
		}
	}

	m_uMaxValue = (8 * uMax) / m_uUnit;
}

void CVizNetGaugeDlg::DrawTextBackground(CDC * pDC, CRect clRect)
{
	float fAtten = 3.0f;
	for (int y = 10; y < 60; y++)
	{
		for (int x = 10; x < 140; x++)
		{
			//if (((x - 30)*(x - 30) + (y - 30)*(y - 30) < 400) || ((x - 30)*(x - 30) + (y - 40)*(y - 40) < 400) ||
			//	((x - 90)*(x - 90) + (y - 30)*(y - 30) < 400) || ((x - 90)*(x - 90) + (y - 40)*(y - 40) < 400) ||
			//	((x>30) && (x<90))
			//	)
			//if(((pow((x-70)/3,4)) + (pow((y-40)/2,4)) < (pow(20,4))))
			
			{
				COLORREF crPix = pDC->GetPixel(x, y);
				pDC->SetPixel(x, y, RGB(GetRValue(crPix) / fAtten, GetGValue(crPix) / fAtten, GetBValue(crPix) / fAtten));
			}
		}
	}

}

void CVizNetGaugeDlg::DrawInfoText(CDC * pDC, CRect clRect)
{
	//too small to draw
	if ((clRect.Width() < 200) || (clRect.Height() < 100))return;

	CRect rText;
	CString sValue;
	COLORREF crText = RGB(m_uTextIntensityTitle, m_uTextIntensityTitle, m_uTextIntensityTitle);

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	CFont fValue;
	fValue.CreatePointFont((int)(m_uTextFontSize * 10 * GetScaleRatio() * 0.8f), m_sFont);

	CFont * oldFont2 = pDC->SelectObject(&fValue);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crText);

	if (!m_bIsUpload)
	{
		rText.SetRect(20, 15, clRect.right / 2, clRect.bottom);
		sValue = m_sNetInterfaces[m_uSelectedInterface];
		pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_LEFT | DT_WORDBREAK, &dtParams);
	}

	sValue = _T("Download | ") + m_sUnitName;
	if (m_bIsUpload) sValue = _T("Upload | ") + m_sUnitName;
	else sValue = _T("Download | ") + m_sUnitName;

	rText.SetRect(clRect.right / 2, 0, clRect.right-20, clRect.bottom/2-2);
	pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_RIGHT| DT_BOTTOM | DT_SINGLELINE, &dtParams);

	//display peak level and mid line
	sValue.Format(_T("Peak %d %s"), m_uMaxValue, m_sUnitName);
	for (int x = 0; x < clRect.right; x = x + 6)
	{
		pDC->SetPixel(x, clRect.bottom / 2, RGB(120, 120, 120));
		pDC->SetPixel(x, clRect.bottom-1, RGB(120, 120, 120));
		//pDC->SetPixel(x, clRect.top+3, RGB(120, 120, 120));
	}

	rText.SetRect(20, 0, clRect.right/2, (clRect.bottom / 2) -2);
	pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_LEFT | DT_BOTTOM | DT_SINGLELINE, &dtParams);


	pDC->SelectObject(oldFont2);
	fValue.DeleteObject();
}

void CVizNetGaugeDlg::DrawSpeedText(CDC * pDC, CRect clRect)
{
	CRect rText;
	rText.left = clRect.left;
	rText.top = clRect.top;
	rText.right = clRect.right-20;
	rText.bottom = clRect.bottom/2;

	COLORREF crValue = RGB(m_uTextIntensityValue, m_uTextIntensityValue, m_uTextIntensityValue);

	CString sValue;
	if(m_bIsUpload) sValue.Format(_T("%.2f"), m_fAverageUploadSpeed[m_uSelectedInterface]);
	else sValue.Format(_T("%.2f"), m_fAverageDownloadSpeed[m_uSelectedInterface]);

	if (m_uSelectedInterface == 0)
	{
		if(m_bIsUpload) sValue.Format(_T("%.2f"), m_fAverageTotalUploadSpeed);
		else sValue.Format(_T("%.2f"), m_fAverageTotalDownloadSpeed);
	}

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	CFont fValue;
	fValue.CreatePointFont((int)(m_uValueFontSize * 10 * GetScaleRatio()), m_sFont);

	CFont * oldFont2 = pDC->SelectObject(&fValue);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crValue);
	pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE, &dtParams);

	pDC->SelectObject(oldFont2);
	fValue.DeleteObject();
}

void CVizNetGaugeDlg::DrawDataText(CDC * pDC, CRect clRect)
{
	if (clRect.Width() < 200) return;

	CRect rText;
	rText.left = clRect.left;
	rText.top = clRect.bottom + (LONG)(clRect.bottom*0.015f) + 1;
	rText.right = clRect.right;
	rText.bottom = clRect.bottom + 400;
	
	COLORREF crValue = RGB(m_uTextIntensityValue, m_uTextIntensityValue, m_uTextIntensityValue);

	CString sValue;
	UINT mb = 1048576;// 1024 * 1024;
	LONGLONG dn = (m_lBytesDownHisLast + m_lBytesDown);
	LONGLONG up = (m_lBytesUpHisLast + m_lBytesUp);
	LONGLONG total = (dn + up) / mb;
	m_lBytesRem = (m_DataUseDlg.m_lMaxData) - total;

	if (m_bIsUpload) sValue.Format(_T("%lld MB Total : : %lld MB Remaining"), total, m_lBytesRem);
	else sValue.Format(_T("%lld MB Down : : %lld MB Up"), dn/mb, up/mb);

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	CFont fValue;
	fValue.CreatePointFont((int)(m_uValueFontSize * 4 * GetScaleRatio()), m_sFont);

	CFont * oldFont2 = pDC->SelectObject(&fValue);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crValue/1.5f);
	pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_CENTER | DT_SINGLELINE, &dtParams);

	pDC->SelectObject(oldFont2);
	fValue.DeleteObject();
}

void CVizNetGaugeDlg::DrawVignette(CDC * pDC, CRect clRect)
{
	int iVigWidth = 30 * clRect.Width()/m_uWinSzMax;
	float fMaxAtten = 0.25f;

	for (int x = 0; x < iVigWidth; x++)
	{
		float fAtten = 1.0f + fMaxAtten * (1.0f - ((float)x/ (float)iVigWidth));
		for (int y = 0; y < clRect.bottom; y++)
		{
			COLORREF crPix = pDC->GetPixel(x, y);
			pDC->SetPixel(x, y, RGB(GetRValue(crPix) / fAtten, GetGValue(crPix) / fAtten, GetBValue(crPix) / fAtten));
		}
	}

	for (int x = clRect.right - iVigWidth; x < clRect.right; x++)
	{
		//float t = (float)x - clRect.right + iVigWidth;
		float fAtten = 1.0f + fMaxAtten *((((float)x - clRect.right + iVigWidth) / (float)iVigWidth));
		for (int y = 0; y < clRect.bottom; y++)
		{
			COLORREF crPix = pDC->GetPixel(x, y);
			pDC->SetPixel(x, y, RGB(GetRValue(crPix) / fAtten, GetGValue(crPix) / fAtten, GetBValue(crPix) / fAtten));
		}
	}

	for (int y = 0; y < iVigWidth; y++)
	{
		float fAtten = 1.0f + fMaxAtten * (1.0f - ((float)y / (float)iVigWidth));
		for (int x = 0; x < clRect.right; x++)
		{
			COLORREF crPix = pDC->GetPixel(x, y);
			pDC->SetPixel(x, y, RGB(GetRValue(crPix) / fAtten, GetGValue(crPix) / fAtten, GetBValue(crPix) / fAtten));
		}
	}

	for (int y = clRect.bottom - iVigWidth; y < clRect.bottom; y++)
	{
		float fAtten = 1.0f + fMaxAtten *((((float)y - clRect.bottom + iVigWidth) / (float)iVigWidth));
		for (int x = 0; x < clRect.right; x++)
		{
			COLORREF crPix = pDC->GetPixel(x, y);
			pDC->SetPixel(x, y, RGB(GetRValue(crPix) / fAtten, GetGValue(crPix) / fAtten, GetBValue(crPix) / fAtten));
		}
	}

}

float CVizNetGaugeDlg::GetScaleRatio()
{
	CRect rClient;
	GetClientRect(&rClient);

	return 1 + (1.5f * (float)rClient.Width() * (float)rClient.Height()) / (float)(m_uWinSzMax * m_uWinSzMax);

}

void CVizNetGaugeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_uTimer)
	{
		Plot();
		SaveHis();
		Renew();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CVizNetGaugeDlg::OnDestroy()
{
	if (!m_bMinimized) //store window position only if not minimized to tray
	{
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		AfxGetApp()->WriteProfileBinary(_T("VizNetGauge"), _T("WP"), (LPBYTE)&wp, sizeof(wp));
	}

	SaveSettings();

	KillTimer(m_uTimer);
	WMICleanup();

	if (m_bMinimized)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_TrayData);
	}

	// Unload the menu resources
	m_MenuTray.DestroyMenu();
	m_MenuPopup.DestroyMenu();

	CDialogEx::OnDestroy();

}


BOOL CVizNetGaugeDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}


void CVizNetGaugeDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_uWinSzMin;
	lpMMI->ptMinTrackSize.y = m_uWinSzMin;
	lpMMI->ptMaxTrackSize.x = m_uWinSzMax;
	lpMMI->ptMaxTrackSize.y = m_uWinSzMax;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CVizNetGaugeDlg::OnEnterSizeMove()
{
	//prevent jerky move/size
	KillTimer(m_uTimer);

	CDialogEx::OnEnterSizeMove();
}


void CVizNetGaugeDlg::OnExitSizeMove()
{
	m_uTimer = SetTimer(VNG_TIMER, m_uTimerDelay, NULL);

	CDialogEx::OnExitSizeMove();
}

//WMI functions follow

//WMI Ref: 
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa394293.aspx
//https://docs.microsoft.com/en-us/windows/desktop/WmiSdk/example-creating-a-wmi-application
//https://docs.microsoft.com/en-us/windows/desktop/WmiSdk/accessing-performance-data-in-c--
//Run %windir%\system32\wbem\wbemtest.exe

BOOL CVizNetGaugeDlg::InitWMI()
{
	pRefresher = NULL;
	pEnum = NULL;
	pConfig = NULL;
	pRefresherRaw = NULL;
	pEnumRaw = NULL;
	pConfigRaw = NULL;

	pNameSpace = NULL;
	pWbemLocator = NULL;
	m_hrResult = S_OK;
	m_bIsInitHandles = TRUE;
	m_bIsInitHandles2 = TRUE;

	BSTR bstrNameSpace = NULL;
	long lID = 0;
	long lIDRaw = 0;

	if (FAILED(m_hrResult = CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		WMICleanup();
		return 1;
	}

	if (FAILED(m_hrResult = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_NONE,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE, 0)))
	{
		WMICleanup();
		return 1;
	}

	if (FAILED(m_hrResult = CoCreateInstance(
		CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(void**)&pWbemLocator)))
	{
		WMICleanup();
		return 1;
	}

	// Connect to the desired namespace.
	bstrNameSpace = SysAllocString(L"\\\\.\\root\\cimv2");
	if (NULL == bstrNameSpace)
	{
		m_hrResult = E_OUTOFMEMORY;
		WMICleanup();
		return 1;
	}
	if (FAILED(m_hrResult = pWbemLocator->ConnectServer(
		bstrNameSpace,
		NULL, // User name
		NULL, // Password
		NULL, // Locale
		0L,   // Security flags
		NULL, // Authority
		NULL, // Wbem context
		&pNameSpace)))
	{
		WMICleanup();
		return 1;
	}
	pWbemLocator->Release();
	pWbemLocator = NULL;
	SysFreeString(bstrNameSpace);
	bstrNameSpace = NULL;

	if (FAILED(m_hrResult = CoCreateInstance(
		CLSID_WbemRefresher,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemRefresher,
		(void**)&pRefresher)))
	{
		WMICleanup();
		return 1;
	}

	if (FAILED(m_hrResult = CoCreateInstance(
		CLSID_WbemRefresher,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemRefresher,
		(void**)&pRefresherRaw)))
	{
		WMICleanup();
		return 1;
	}

	if (FAILED(m_hrResult = pRefresher->QueryInterface(
		IID_IWbemConfigureRefresher,
		(void **)&pConfig)))
	{
		WMICleanup();
		return 1;
	}

	if (FAILED(m_hrResult = pRefresherRaw->QueryInterface(
		IID_IWbemConfigureRefresher,
		(void **)&pConfigRaw)))
	{
		WMICleanup();
		return 1;
	}

	// Add an enumerator to the refresher.
	if (FAILED(m_hrResult = pConfig->AddEnum(
		pNameSpace,
//		L"Win32_PerfRawData_Tcpip_NetworkInterface",
		L"Win32_PerfFormattedData_Tcpip_NetworkInterface",
		0,
		NULL,
		&pEnum,
		&lID)))
	{
		WMICleanup();
		return 1;
	}

	// Add an enumerator to the refresher.
	if (FAILED(m_hrResult = pConfigRaw->AddEnum(
		pNameSpace,
		L"Win32_PerfRawData_Tcpip_NetworkInterface",
//		L"Win32_PerfFormattedData_Tcpip_NetworkInterface",
		0,
		NULL,
		&pEnumRaw,
		&lIDRaw)))
	{
		WMICleanup();
		return 1;
	}


	pConfig->Release();
	pConfig = NULL;
	pConfigRaw->Release();
	pConfigRaw = NULL;

	if (NULL != bstrNameSpace)
	{
		SysFreeString(bstrNameSpace);
	}


	return 0;
}

BOOL CVizNetGaugeDlg::GetStatsRefresher()
{
	HRESULT                 hr = S_OK;
	DWORD                   dwVirtualBytes = 0;
	DWORD                   dwProcessId = 0;
	DWORD                   dwNumObjects = 0;
	DWORD                   dwNumReturned = 0;
	DWORD                   dwIDProcess = 0;
	DWORD                   i = 0;
	int                     x = 0;
	IWbemObjectAccess       **apEnumAccess = NULL;

	dwNumReturned = 0;
	dwIDProcess = 0;
	dwNumObjects = 0;
	
	if (FAILED(hr = pRefresher->Refresh(0L)))
	{
		goto CLEANUP;
	}

	hr = pEnum->GetObjects(0L, dwNumObjects, apEnumAccess, &dwNumReturned);

	// If the buffer was not big enough, allocate a bigger buffer and retry.
	if (hr == WBEM_E_BUFFER_TOO_SMALL && dwNumReturned > dwNumObjects)
	{
		apEnumAccess = new IWbemObjectAccess*[dwNumReturned];
		if (NULL == apEnumAccess)
		{
			hr = E_OUTOFMEMORY;
			goto CLEANUP;
		}
		SecureZeroMemory(apEnumAccess,
			dwNumReturned * sizeof(IWbemObjectAccess*));
		dwNumObjects = dwNumReturned;

		if (FAILED(hr = pEnum->GetObjects(0L,
			dwNumObjects,
			apEnumAccess,
			&dwNumReturned)))
		{
			goto CLEANUP;
		}
	}
	else
	{
		if (hr == WBEM_S_NO_ERROR)
		{
			hr = WBEM_E_NOT_FOUND;
			goto CLEANUP;
		}
	}

	// First time through, get the handles and names of interfaces.
	if (m_bIsInitHandles)
	{
		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
			L"Name",
			&m_lInterfaceNameType,
			&m_lInterfaceNameHandle)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
			L"BytesReceivedPerSec",
			&m_lBytesReceivedPerSecType,
			&m_lBytesReceivedPerSecHandle)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
			L"BytesSentPerSec",
			&m_lBytesSentPerSecType,
			&m_lBytesSentPerSecHandle)))
		{
			goto CLEANUP;
		}

		for (i = 0; i < dwNumReturned; i++)
		{
			long nb = 0;
			byte data[200];

			if (FAILED(hr = apEnumAccess[i]->ReadPropertyValue(m_lInterfaceNameHandle, 200, &nb, data)))
			{
				goto CLEANUP;
			}

			//interface names
			wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
			CString sName(wstr.c_str());

			if ((i + 2) < VNG_MAX_INTERFACES) //0 and 1 are for total and auto modes
			{
				m_sNetInterfaces[i + 2] = sName;
				m_uNetInterfaceCount++;
			}
		}

		UpdateMenuInterfaceNames();
		m_bIsInitHandles = FALSE;
	}

	for (i = 0; i < dwNumReturned; i++)
	{
		long nb = 0;
		//byte data[200];
		DWORD dwBytesRecdPerSec = 0;
		DWORD dwBytesSentPerSec = 0;

		//if (FAILED(hr = apEnumAccess[i]->ReadPropertyValue(m_lInterfaceNameHandle, 200, &nb, data)))
		//{
		//	goto CLEANUP;
		//}
		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(m_lBytesReceivedPerSecHandle, &dwBytesRecdPerSec)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(m_lBytesSentPerSecHandle, &dwBytesSentPerSec)))
		{
			goto CLEANUP;
		}

		//interface names
		//wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
		//CString sName(wstr.c_str());
		//m_sNetInterfaces[i] = sName;

		//sample
		InsertSample(i+2, dwBytesRecdPerSec, dwBytesSentPerSec);

		// Done with the object
		apEnumAccess[i]->Release();
		apEnumAccess[i] = NULL;
	}

	if (NULL != apEnumAccess)
	{
		delete[] apEnumAccess;
		apEnumAccess = NULL;
	}

CLEANUP:

	if (NULL != apEnumAccess)
	{
		for (i = 0; i < dwNumReturned; i++)
		{
			if (apEnumAccess[i] != NULL)
			{
				apEnumAccess[i]->Release();
				apEnumAccess[i] = NULL;
			}
		}
		delete[] apEnumAccess;
	}

	return 0;
}


BOOL CVizNetGaugeDlg::GetStatsRefresherRaw()
{
	HRESULT                 hr = S_OK;
	DWORD                   dwVirtualBytes = 0;
	DWORD                   dwProcessId = 0;
	DWORD                   dwNumObjects = 0;
	DWORD                   dwNumReturned = 0;
	DWORD                   dwIDProcess = 0;
	DWORD                   i = 0;
	int                     x = 0;
	IWbemObjectAccess       **apEnumAccess = NULL;

	dwNumReturned = 0;
	dwIDProcess = 0;
	dwNumObjects = 0;

	if (FAILED(hr = pRefresherRaw->Refresh(0L)))
	{
		goto CLEANUP;
	}

	hr = pEnumRaw->GetObjects(0L, dwNumObjects, apEnumAccess, &dwNumReturned);

	// If the buffer was not big enough, allocate a bigger buffer and retry.
	if (hr == WBEM_E_BUFFER_TOO_SMALL && dwNumReturned > dwNumObjects)
	{
		apEnumAccess = new IWbemObjectAccess*[dwNumReturned];
		if (NULL == apEnumAccess)
		{
			hr = E_OUTOFMEMORY;
			goto CLEANUP;
		}
		SecureZeroMemory(apEnumAccess,
			dwNumReturned * sizeof(IWbemObjectAccess*));
		dwNumObjects = dwNumReturned;

		if (FAILED(hr = pEnumRaw->GetObjects(0L,
			dwNumObjects,
			apEnumAccess,
			&dwNumReturned)))
		{
			goto CLEANUP;
		}
	}
	else
	{
		if (hr == WBEM_S_NO_ERROR)
		{
			hr = WBEM_E_NOT_FOUND;
			goto CLEANUP;
		}
	}

	// First time through, get the handles and names of interfaces.
	if (m_bIsInitHandles2)
	{
		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
			L"BytesReceivedPerSec",
			&m_lBytesReceivedPerSecType2,
			&m_lBytesReceivedPerSecHandle2)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
			L"BytesSentPerSec",
			&m_lBytesSentPerSecType2,
			&m_lBytesSentPerSecHandle2)))
		{
			goto CLEANUP;
		}

		m_bIsInitHandles2 = FALSE;
		m_IsInitial = TRUE;
	}

	m_lBytesDown = 0;
	m_lBytesUp = 0;

	for (i = 0; i < dwNumReturned; i++)
	{
		long nb = 0;
		DWORD dwBytesRecdPerSec = 0;
		DWORD dwBytesSentPerSec = 0;

		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(m_lBytesReceivedPerSecHandle2, &dwBytesRecdPerSec)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(m_lBytesSentPerSecHandle2, &dwBytesSentPerSec)))
		{
			goto CLEANUP;
		}

		//accumulate for all interfaces
		m_lBytesDown += dwBytesRecdPerSec;
		m_lBytesUp += dwBytesSentPerSec;

		// Done with the object
		apEnumAccess[i]->Release();
		apEnumAccess[i] = NULL;
	}

	if (m_IsInitial)
	{
		m_IsInitial = !m_lBytesDown; //ensure non-zero counter
		m_lBytesDownInitial = m_lBytesDown;
		m_lBytesUpInitial = m_lBytesUp;
	}

	m_lBytesDown -= m_lBytesDownInitial;
	m_lBytesUp -= m_lBytesUpInitial;

	//deal with NIC reset
	if (m_lBytesDown < 0 || m_lBytesUp < 0)
	{
		SaveHis();
		m_lBytesDown = 0;
		m_lBytesUp = 0;
		m_lBytesDownInitial = m_lBytesDown;
		m_lBytesUpInitial = m_lBytesUp;
	}

	//m_lBytesDown *= 1000;//test
	//m_lBytesUp *= 1000;//test

	if (NULL != apEnumAccess)
	{
		delete[] apEnumAccess;
		apEnumAccess = NULL;
	}

CLEANUP:

	if (NULL != apEnumAccess)
	{
		for (i = 0; i < dwNumReturned; i++)
		{
			if (apEnumAccess[i] != NULL)
			{
				apEnumAccess[i]->Release();
				apEnumAccess[i] = NULL;
			}
		}
		delete[] apEnumAccess;
	}

	return 0;
}



void CVizNetGaugeDlg::WMICleanup()
{
	if (FAILED(m_hrResult))
	{
		CString sErr;
		sErr.Format(L"Error: WMI failed. Status=%08x", m_hrResult);
		AfxMessageBox(sErr, MB_ICONERROR);
	}

	if (NULL != pWbemLocator)
	{
		pWbemLocator->Release();
	}
	if (NULL != pNameSpace)
	{
		pNameSpace->Release();
	}
	if (NULL != pEnum)
	{
		pEnum->Release();
	}
	if (NULL != pEnumRaw)
	{
		pEnumRaw->Release();
	}
	if (NULL != pConfig)
	{
		pConfig->Release();
	}
	if (NULL != pRefresher)
	{
		pRefresher->Release();
	}
	if (NULL != pConfigRaw)
	{
		pConfigRaw->Release();
	}
	if (NULL != pRefresherRaw)
	{
		pRefresherRaw->Release();
	}

	CoUninitialize();

}

void CVizNetGaugeDlg::InsertSample(int iInterface, DWORD dwDnValue, DWORD dwUpValue)
{
	//push down the values in array
	for (int j = VNG_MAX_SAMPLES - 2; j >=0 ; j--)
	{
		m_uDownloadSamples[iInterface][j+1] = m_uDownloadSamples[iInterface][j];
		m_uUploadSamples[iInterface][j+1] = m_uUploadSamples[iInterface][j];
	}

	m_uDownloadSamples[iInterface][0] = dwDnValue;
	m_uUploadSamples[iInterface][0] = dwUpValue;

	//get averages
	m_fAverageDownloadSpeed[iInterface] = 0.0f;
	m_fAverageUploadSpeed[iInterface] = 0.0f;

	if (m_fAveragingIntervel >= VNG_MAX_SAMPLES) m_fAveragingIntervel = VNG_MAX_SAMPLES;

	for (int j = 0; j < m_fAveragingIntervel; j++)
	{
		m_fAverageDownloadSpeed[iInterface] += m_uDownloadSamples[iInterface][j];
		m_fAverageUploadSpeed[iInterface] += m_uUploadSamples[iInterface][j];
	}

	//bytes to bits per unit bps kbps mbps
	m_fAverageDownloadSpeed[iInterface] = (8 * m_fAverageDownloadSpeed[iInterface]) / (m_fAveragingIntervel * m_uUnit); 
	m_fAverageUploadSpeed[iInterface] = (8 * m_fAverageUploadSpeed[iInterface]) / (m_fAveragingIntervel * m_uUnit);

}

void CVizNetGaugeDlg::GetTotalAllInterfaces()
{
	if (m_uSelectedInterface == 0)
	{
		m_fAverageTotalDownloadSpeed = 0;
		m_fAverageTotalUploadSpeed = 0;

		//push down the values in array
		for (int j = VNG_MAX_SAMPLES - 2; j >= 0; j--)
		{
			m_uDownloadSamples[0][j + 1] = m_uDownloadSamples[0][j];
			m_uUploadSamples[0][j + 1] = m_uUploadSamples[0][j];
		}

		m_uDownloadSamples[0][0] = 0;
		m_uUploadSamples[0][0] = 0;
		for (UINT i = 2; i < m_uNetInterfaceCount; i++)
		{
			//collect sample from all interfaces in iInterface=0
			m_uDownloadSamples[0][0] += m_uDownloadSamples[i][0];
			m_uUploadSamples[0][0] += m_uUploadSamples[i][0];

			//average of all interfaces
			m_fAverageTotalDownloadSpeed += m_fAverageDownloadSpeed[i];
			m_fAverageTotalUploadSpeed += m_fAverageUploadSpeed[i];
		}

	}
}

void CVizNetGaugeDlg::AutodetectActiveInterface()
{
	if (m_uSelectedInterface == 1)
	{
		float fMaxAverage = 0.0f;
		float fSum = 0;
		for (UINT i = 2; i < m_uNetInterfaceCount; i++)
		{
			fSum = m_fAverageDownloadSpeed[i] + m_fAverageUploadSpeed[i];
			if (fSum > fMaxAverage)
			{
				fMaxAverage = fSum;
				m_uSelectedInterface = i;
			}
		}
	}
}

//menu command handlers follow

void CVizNetGaugeDlg::UpdateMenuInterfaceNames()
{
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_ID;
	for (UINT j = 0; j < m_uNetInterfaceCount; j++)
	{
		info.wID = 32808 + j;
		m_MenuPopup.GetSubMenu(0)->GetSubMenu(3)->InsertMenuItem(j+2, &info, TRUE);
		m_MenuPopup.ModifyMenuW(info.wID, MF_BYCOMMAND | MF_STRING, info.wID, m_sNetInterfaces[j+2]);
	}

	m_sNetInterfaces[0] = L"Total traffic from all Interfaces";
	m_sNetInterfaces[1] = L"Interface: Auto";

	ApplySettingsToMenu();
}

void CVizNetGaugeDlg::SetupMinimizeToTray()
{
	m_TrayData.cbSize = sizeof(NOTIFYICONDATA);
	m_TrayData.hWnd = this->m_hWnd;
	m_TrayData.uID = 1;
	m_TrayData.uCallbackMessage = WM_TRAY_MESSAGE;
	m_TrayData.hIcon = this->m_hIcon;

	wcscpy_s(m_TrayData.szTip, _T("Clipboard Plus"));
	m_TrayData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;

	BOOL bSuccess = m_MenuTray.LoadMenu(IDR_MENU_TRAY);
	if (!(bSuccess))
	{
		MessageBox(_T("Unabled to load menu"), _T("Error"));
	}

	m_bMinimized = FALSE;
}

void CVizNetGaugeDlg::MinimizeToTray()
{
	BOOL bSuccess = Shell_NotifyIcon(NIM_ADD, &m_TrayData);
	if (!(bSuccess))
	{
		MessageBox(_T("Unable to set tray icon"), _T("Error"));
	}

	this->ShowWindow(SW_MINIMIZE);
	this->ShowWindow(SW_HIDE);

	m_bMinimized = TRUE;

}


void CVizNetGaugeDlg::MaximizeFromTray()
{
	this->ShowWindow(SW_SHOW);
	this->ShowWindow(SW_RESTORE);

	Shell_NotifyIcon(NIM_DELETE, &m_TrayData);

	m_bMinimized = FALSE;

	SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
	SetForegroundWindow();
	SetActiveWindow();
	SetWindowPos(0, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);
	RedrawWindow(0, 0, RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

}


afx_msg LRESULT CVizNetGaugeDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID;
	UINT uMsg;
	CPoint pt;

	uID = (UINT)wParam;
	uMsg = (UINT)lParam;

	if (uID != 1)
	{
		return 0;
	}

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDown(pt);
		break;

	case WM_RBUTTONDOWN:

	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		OnTrayRButtonDown(pt);
		break;

	}

	return 0;
}

void CVizNetGaugeDlg::OnTrayLButtonDown(CPoint pt)
{
	MaximizeFromTray();
}

void CVizNetGaugeDlg::OnTrayRButtonDown(CPoint pt)
{
	m_MenuTray.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, this);
}


void CVizNetGaugeDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu *pSubMenu = m_MenuPopup.GetSubMenu(0);
	ASSERT(pSubMenu);
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

}


void CVizNetGaugeDlg::OnOptionsAlwaysontop()
{
	m_bTopmostMode = !m_bTopmostMode;
	m_MenuPopup.CheckMenuItem(ID_OPTIONS_ALWAYSONTOP, (MF_CHECKED * (UINT)m_bTopmostMode) | MF_BYCOMMAND);

	if (m_bTopmostMode) SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}


void CVizNetGaugeDlg::OnOptionsMinimizetotray()
{
	MinimizeToTray();
}


void CVizNetGaugeDlg::OnTrayRestore()
{
	MaximizeFromTray();
}


void CVizNetGaugeDlg::OnTrayExit()
{
	CDialogEx::OnOK();
}

void CVizNetGaugeDlg::OnMenuExit()
{
	CDialogEx::OnOK();
}


void CVizNetGaugeDlg::OnUpdatespeedSlow()
{
	KillTimer(m_uTimer);
	m_uTimerDelay = 5000; 
	m_uTimer = SetTimer(VNG_TIMER, m_uTimerDelay, NULL);
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_SLOW,   (MF_CHECKED   | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_MEDIUM, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_FAST,   (MF_UNCHECKED | MF_BYCOMMAND));

}


void CVizNetGaugeDlg::OnUpdatespeedMedium()
{
	KillTimer(m_uTimer);
	m_uTimerDelay = VNG_DEFAULT_UPDATEFREQ;
	m_uTimer = SetTimer(VNG_TIMER, m_uTimerDelay, NULL);
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_SLOW, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_MEDIUM, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_FAST, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnUpdatespeedFast()
{
	KillTimer(m_uTimer);
	m_uTimerDelay = 300;
	m_uTimer = SetTimer(VNG_TIMER, m_uTimerDelay, NULL);
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_SLOW,   (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_MEDIUM, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UPDATESPEED_FAST,   (MF_CHECKED   | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnUnitsBitspersec()
{
	m_uUnit = 1;
	m_sUnitName = _T("Bps");
	m_MenuPopup.CheckMenuItem(ID_UNITS_BITSPERSEC, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_KILOBITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_MEGABITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_GIGABITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnUnitsKilobitspersec()
{
	m_uUnit = VNG_DEFAULT_UNIT;
	m_sUnitName = _T("Kbps");
	m_MenuPopup.CheckMenuItem(ID_UNITS_BITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_KILOBITSPERSEC, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_MEGABITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_GIGABITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnUnitsMegabitspersec()
{
	m_uUnit = 1000000;
	m_sUnitName = _T("Mbps");
	m_MenuPopup.CheckMenuItem(ID_UNITS_BITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_KILOBITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_MEGABITSPERSEC, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_GIGABITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnUnitsGigabitspersec()
{
	m_uUnit = 1000000000;
	m_sUnitName = _T("Gbps");
	m_MenuPopup.CheckMenuItem(ID_UNITS_BITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_KILOBITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_MEGABITSPERSEC, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_UNITS_GIGABITSPERSEC, (MF_CHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnAveraging10samples()
{
	m_fAveragingIntervel = VNG_DEFAULT_AVE;
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_10SAMPLES, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_30SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_60SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_INSTANTANEOUS, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnAveraging30sample()
{
	m_fAveragingIntervel = 30.0f;
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_10SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_30SAMPLES, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_60SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_INSTANTANEOUS, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnAveraging60samples()
{
	m_fAveragingIntervel = 60.0f;
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_10SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_30SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_60SAMPLES, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_INSTANTANEOUS, (MF_UNCHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnAveragingInstantaneous()
{
	m_fAveragingIntervel = 1.0f;
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_10SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_30SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_60SAMPLES, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_AVERAGING_INSTANTANEOUS, (MF_CHECKED | MF_BYCOMMAND));
}


void CVizNetGaugeDlg::OnThemesAquapale()
{
	m_crBarDn = RGB(20, 200, 200);
	m_crBarUp = RGB(200, 200, 150);
	m_crBackground = RGB(m_uBkIntensity, m_uBkIntensity, m_uBkIntensity);
	m_MenuPopup.CheckMenuItem(ID_THEMES_AQUAPALE, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_REDORANGE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_BLUEGREEN, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_GREYWHITE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_uSelectedTheme = VNG_DEFAULT_THEME;

}


void CVizNetGaugeDlg::OnThemesRedorange()
{
	m_crBarDn = RGB(180, 40, 40);
	m_crBarUp = RGB(190, 120, 50);
	m_crBackground = RGB(m_uBkIntensity+5, m_uBkIntensity-2, m_uBkIntensity-2);
	m_MenuPopup.CheckMenuItem(ID_THEMES_AQUAPALE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_REDORANGE, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_BLUEGREEN, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_GREYWHITE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_uSelectedTheme = 1;

}


void CVizNetGaugeDlg::OnThemesBluegreen()
{
	m_crBarDn = RGB(60, 160, 220);
	m_crBarUp = RGB(60, 180, 80);
	m_crBackground = RGB(m_uBkIntensity-5, m_uBkIntensity+2, m_uBkIntensity-5);
	m_MenuPopup.CheckMenuItem(ID_THEMES_AQUAPALE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_REDORANGE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_BLUEGREEN, (MF_CHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_GREYWHITE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_uSelectedTheme = 2;

}


void CVizNetGaugeDlg::OnThemesGreywhite()
{
	m_crBarDn = RGB(230, 230, 230);
	m_crBarUp = RGB(30, 30, 30);
	m_crBackground = RGB(m_uBkIntensity+7, m_uBkIntensity + 7, m_uBkIntensity + 7);
	m_MenuPopup.CheckMenuItem(ID_THEMES_AQUAPALE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_REDORANGE, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_BLUEGREEN, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_THEMES_GREYWHITE, (MF_CHECKED | MF_BYCOMMAND));
	m_uSelectedTheme = 3;

}


void CVizNetGaugeDlg::OnCommandRangeInterfaces(UINT nID)
{
	ClearCheckInterface();
	m_MenuPopup.CheckMenuItem(nID, (MF_CHECKED | MF_BYCOMMAND));
	m_uSelectedInterface = 2 + nID - ID_INTERFACES_NIF1;
}

void CVizNetGaugeDlg::OnInterfacesTotalofall()
{
	ClearCheckInterface();
	m_MenuPopup.CheckMenuItem(ID_INTERFACES_TOTALOFALL, (MF_CHECKED | MF_BYCOMMAND));
	m_uSelectedInterface = VNG_DEFAULT_INTERFACE;
}


void CVizNetGaugeDlg::OnInterfacesAuto()
{
	ClearCheckInterface();
	m_MenuPopup.CheckMenuItem(ID_INTERFACES_AUTO, (MF_CHECKED | MF_BYCOMMAND));
	m_uSelectedInterface = 1; //this will change
	m_IsAutoDetect = TRUE;
}

void CVizNetGaugeDlg::ClearCheckInterface()
{
	if (m_uNetInterfaceCount > VNG_MAX_INTERFACES)m_uNetInterfaceCount = 2;

	for (UINT i = 0; i < m_uNetInterfaceCount; i++)
	{
		m_MenuPopup.CheckMenuItem(ID_INTERFACES_NIF1 + i, (MF_UNCHECKED | MF_BYCOMMAND));
	}
	m_MenuPopup.CheckMenuItem(ID_INTERFACES_TOTALOFALL, (MF_UNCHECKED | MF_BYCOMMAND));
	m_MenuPopup.CheckMenuItem(ID_INTERFACES_AUTO, (MF_UNCHECKED | MF_BYCOMMAND));

	m_IsAutoDetect = FALSE;

}

void CVizNetGaugeDlg::OnHelpOnlinehelp()
{
	ShellExecute(NULL, _T("open"), _T("https://oormi.in/software/vng/help.html"), NULL, NULL, SW_SHOWNORMAL);
}


void CVizNetGaugeDlg::OnHelpGetmoreapps()
{
	ShellExecute(NULL, _T("open"), _T("https://github.com/oormicreations?tab=repositories"), NULL, NULL, SW_SHOWNORMAL);
}


void CVizNetGaugeDlg::OnHelpCheckforupdates()//TODO: change txt file url
{
	m_NetHelper.Checkforupdates(m_uVMaj, m_uVMin,
		_T("https://oormi.in/software/cbp/updatevng.txt"),
		_T(" https://github.com/oormicreations/VizNetGauge/releases"), 
		_T("VizNetGauge"));
}


void CVizNetGaugeDlg::OnHelpAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CVizNetGaugeDlg::OnOptionsSettings()
{
	ResetSettings();
	SaveSettings();
}

void CVizNetGaugeDlg::SaveSettings()
{
	CString profile;
	VNGSET ps;
#ifdef DEBUG
	profile.Format(_T("%d%d_d"), m_uVMaj, m_uVMin);
#else
	profile.Format(_T("%d%d"), m_uVMaj, m_uVMin);
#endif // DEBUG

	ps.m_bSetTopMost = m_bTopmostMode;
	ps.m_uSetUpdateFreq = m_uTimerDelay;
	ps.m_uSetAve = m_fAveragingIntervel;
	ps.m_uSetSelInterface = m_uSelectedInterface;
	if(m_IsAutoDetect) 	ps.m_uSetSelInterface = 1;
	ps.m_uSetTheme = m_uSelectedTheme;
	ps.m_uSetUnit = m_uUnit;
	ps.m_uSetUnused1 = 0;
	ps.m_uSetUnused2 = 0;
	ps.m_uSetUnused3 = 0;
	ps.m_uSetUnused4 = 0;

	ps.m_lMaxData = m_DataUseDlg.m_lMaxData;
	ps.m_lThreshold = m_DataUseDlg.m_lThreshold;
	ps.m_uPeriod1 = m_DataUseDlg.m_uPeriod1;
	ps.m_uPeriod2 = m_DataUseDlg.m_uPeriod2;
	ps.m_bWarn = m_DataUseDlg.m_bWarn;
	ps.m_bWarnPop = m_DataUseDlg.m_bWarnPop;
	ps.m_bWarnDing = m_DataUseDlg.m_bWarnDing;
	ps.m_bWarnVoice = m_DataUseDlg.m_bWarnVoice;
	ps.m_bRenewDay = m_DataUseDlg.m_bRenewDay;
	ps.m_bRenewMonth = m_DataUseDlg.m_bRenewMonth;

	ps.m_lBytesDown = m_lBytesDownHis;
	ps.m_lBytesUp = m_lBytesUpHis;
	ps.m_uToday = m_uToday;
	ps.m_IsRenewed2 = m_IsRenewed2;
	ps.m_uMonth = m_uMonth;

	AfxGetApp()->WriteProfileBinary(_T("VizNetGauge") + profile, _T("Settings"), (LPBYTE)&ps, sizeof(ps));
}


BOOL CVizNetGaugeDlg::LoadSettings()
{
	CString profile;
	VNGSET *pps;
	UINT n;

#ifdef DEBUG
	profile.Format(_T("%d%d_d"), m_uVMaj, m_uVMin);
#else
	profile.Format(_T("%d%d"), m_uVMaj, m_uVMin);
#endif // DEBUG

	if (AfxGetApp()->GetProfileBinary(_T("VizNetGauge") + profile, _T("Settings"), (LPBYTE*)&pps, &n))
	{
		if (n == sizeof(VNGSET))
		{
			 m_bTopmostMode = pps->m_bSetTopMost;
			 m_uTimerDelay = pps->m_uSetUpdateFreq;
			 m_fAveragingIntervel = pps->m_uSetAve;
			 m_uSelectedInterface = pps->m_uSetSelInterface;
			 m_uSelectedTheme = pps->m_uSetTheme;
			 m_uUnit = pps->m_uSetUnit;

			 m_DataUseDlg.m_lMaxData = pps->m_lMaxData;
			 m_DataUseDlg.m_lThreshold = pps->m_lThreshold;
			 m_DataUseDlg.m_uPeriod1 = pps->m_uPeriod1;
			 m_DataUseDlg.m_uPeriod2 = pps->m_uPeriod2;
			 m_DataUseDlg.m_bWarn = pps->m_bWarn;
			 m_DataUseDlg.m_bWarnPop = pps->m_bWarnPop;
			 m_DataUseDlg.m_bWarnDing = pps->m_bWarnDing;
			 m_DataUseDlg.m_bWarnVoice = pps->m_bWarnVoice;
			 m_DataUseDlg.m_bRenewDay = pps->m_bRenewDay;
			 m_DataUseDlg.m_bRenewMonth = pps->m_bRenewMonth;

			 m_lBytesDownHisLast = pps->m_lBytesDown;
			 m_lBytesUpHisLast = pps->m_lBytesUp;
			 m_uToday = pps->m_uToday;
			 m_IsRenewed2 = pps->m_IsRenewed2;
			 m_uMonth = pps->m_uMonth;

		}
		else
		{
			ResetSettings();
		}

		delete[] pps;
		ApplySettingsToMenu();

		return TRUE;
	}
	

	return FALSE;
}

void CVizNetGaugeDlg::ApplySettingsToMenu()
{
	m_bTopmostMode = !m_bTopmostMode;
	OnOptionsAlwaysontop();

	if (m_uTimerDelay == 5000) OnUpdatespeedSlow();
	if (m_uTimerDelay == VNG_DEFAULT_UPDATEFREQ) OnUpdatespeedMedium();
	if (m_uTimerDelay == 300) OnUpdatespeedFast();

	if (m_uUnit == 1) OnUnitsBitspersec();
	if (m_uUnit == VNG_DEFAULT_UNIT) OnUnitsKilobitspersec();
	if (m_uUnit == 1000000) OnUnitsMegabitspersec();
	if (m_uUnit == 1000000000) OnUnitsGigabitspersec();

	if (m_fAveragingIntervel == VNG_DEFAULT_AVE) OnAveraging10samples();
	if (m_fAveragingIntervel == 30.0f) OnAveraging30sample();
	if (m_fAveragingIntervel == 60.0f) OnAveraging60samples();
	if (m_fAveragingIntervel == 1.0f) OnAveragingInstantaneous();

	if (m_uSelectedTheme == VNG_DEFAULT_THEME) OnThemesAquapale();
	if (m_uSelectedTheme == 1) OnThemesRedorange();
	if (m_uSelectedTheme == 2) OnThemesBluegreen();
	if (m_uSelectedTheme == 3) OnThemesGreywhite();

	if (m_uSelectedInterface == VNG_DEFAULT_INTERFACE) OnInterfacesTotalofall();
	else
	{
		if (m_uSelectedInterface == 1) OnInterfacesAuto();
		else
		{
			OnCommandRangeInterfaces(m_uSelectedInterface - 2 + ID_INTERFACES_NIF1);
		}
	}

}

void CVizNetGaugeDlg::ResetSettings()
{
	m_bTopmostMode = !VNG_DEFAULT_TOPMOST;
	OnOptionsAlwaysontop();
	OnUpdatespeedMedium();
	OnUnitsKilobitspersec();
	OnAveraging10samples();
	OnThemesAquapale();
	OnInterfacesTotalofall();

	m_DataUseDlg.m_lMaxData = 20000;
	m_DataUseDlg.m_lThreshold = 1000;
	m_DataUseDlg.m_uPeriod1 = 1;
	m_DataUseDlg.m_uPeriod2 = 1;
	m_DataUseDlg.m_bWarn = 1;
	m_DataUseDlg.m_bWarnPop = 1;
	m_DataUseDlg.m_bWarnDing = 0;
	m_DataUseDlg.m_bWarnVoice = 1;
	m_DataUseDlg.m_bRenewDay = 0;
	m_DataUseDlg.m_bRenewMonth = 1;
	m_uToday = 0;
	m_IsRenewed2 = TRUE;
	m_uMonth = 0;

	SaveSettings();
}

void CVizNetGaugeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	static bool bOnce = true;

	if (bShow && !IsWindowVisible() && bOnce)
	{
		bOnce = false;

		WINDOWPLACEMENT *lwp;
		UINT nl;

		if (AfxGetApp()->GetProfileBinary(_T("VizNetGauge"), _T("WP"), (LPBYTE*)&lwp, &nl))
		{
			SetWindowPlacement(lwp);
			delete[] lwp;
		}
	}
}


void CVizNetGaugeDlg::OnOptionsConfiguredatausage()
{
	if (IDOK == m_DataUseDlg.DoModal()) SaveSettings();
}


void CVizNetGaugeDlg::OnOptionsResetdatausage()
{
	ResetHis(FALSE);
}

void CVizNetGaugeDlg::SaveHis()
{
	if (m_uBackUpInterval > VNG_BACKUP_INTERVAL)
	{
		m_lBytesDownHis = m_lBytesDownHisLast + m_lBytesDown; //last counter + current counter will be saved
		m_lBytesUpHis = m_lBytesUpHisLast + m_lBytesUp;
		SaveSettings();
		m_uBackUpInterval = 0;
	}
	m_uBackUpInterval++;

}

void CVizNetGaugeDlg::ResetHis(BOOL silent)
{
	if (!silent)
	{
		if(IDYES != AfxMessageBox(_T("All data counters will be reset to 0. Are you sure?"), MB_YESNOCANCEL)) return;
	}

	m_lBytesDownHis = 0;
	m_lBytesUpHis = 0;
	m_lBytesDownHisLast = 0;
	m_lBytesUpHisLast = 0;
	m_IsInitial = TRUE;
	SaveSettings();

}

void CVizNetGaugeDlg::Renew()
{
	UINT day = CTime::GetCurrentTime().GetDay();
	UINT month = CTime::GetCurrentTime().GetMonth();
	
	//day = 1;//test
	//month = 6;
	
	//renewed after 1 day
	if ((day != m_uToday) && (m_DataUseDlg.m_bRenewDay))
	{
		m_uToday = day;
		ResetHis(TRUE);
	}
	
	//renewed on a specific date every month
	if ((day >= m_DataUseDlg.m_uPeriod2) && (m_DataUseDlg.m_bRenewMonth) && (!m_IsRenewed2))
	{
		m_IsRenewed2 = TRUE;
		m_uMonth = month;
		ResetHis(TRUE);
	}
	m_IsRenewed2 = month == m_uMonth;

}

void CVizNetGaugeDlg::Warn()
{
	if (!m_DataUseDlg.m_bWarn) return;
	if (m_IsWarnSent) return;

	if (m_lBytesRem < m_DataUseDlg.m_lThreshold)
	{
		m_IsWarnSent = TRUE;

		if (m_DataUseDlg.m_bWarnDing)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE);
		}
		if (m_DataUseDlg.m_bWarnVoice)
		{
			PlaySound(MAKEINTRESOURCE(IDR_WAVE2), NULL, SND_RESOURCE);
		}
		if (m_DataUseDlg.m_bWarnPop)
		{
			CString s;
			s.Format(_T("Warning: You have %ld MB of data left!"), m_lBytesRem);
			MessageBox(s, _T("Data threshold reached"), MB_ICONWARNING);
		}
	}
}

void CVizNetGaugeDlg::OnOptionsRestoredatabacktohistorical()
{
	//m_lBytesDownHisLast -= m_lBytesDown;// current counter will be deducted from historical
	//m_lBytesUpHisLast -= m_lBytesUp;
	//m_lBytesDownHis = m_lBytesDownHisLast;
	//m_lBytesUpHis = m_lBytesUpHisLast;
	//SaveSettings();
}
