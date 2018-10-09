
// VizNetGaugeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VizNetGauge.h"
#include "VizNetGaugeDlg.h"
#include "afxdialogex.h"




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
END_MESSAGE_MAP()


// CVizNetGaugeDlg message handlers

BOOL CVizNetGaugeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_uVMaj = 1;
	m_uVMin = 0;

	InitSamples();
	InitDraw();
	InitWMI();

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
		Plot();
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
	}

	m_fAveragingIntervel = 10.0f; //10sec for 1Hz update rate
	m_uUnit = 1000; //kbps
	m_sUnitName = _T("Kbps");
	m_uSelectedInterface = 1; 
	m_uMaxValue = 0;

}

void CVizNetGaugeDlg::InitDraw()
{
	m_bIsUpload = FALSE;
	m_uDownloadSpeed = 0;
	m_uUploadSpeed = 0;
	
	m_uBkIntensity = 60;
	m_uGridIntensityMin = 65;
	m_uGridIntensityMaj = 75;
	m_uGridSpacing = 10;

	m_crBarDn = RGB(20, 200, 200);
	m_crBarUp = RGB(200, 200, 150);
	
	m_uBarWidth = 5;
	m_uBarHeight = 50;
	m_uBarSpacing = 2;
	m_uBarCount = m_uWinSzMax / (m_uBarWidth + m_uBarSpacing);

	m_uTextIntensityTitle = 150;
	m_uTextIntensityValue = 255;
	m_uValueFontSize = 20;
	m_uTextFontSize = 8;
	m_sFont = _T("Calibri");

	//Plot();
	m_uTimerDelay = 1000; //1 Hz
	m_uTimer = SetTimer(VNG_TIMER, m_uTimerDelay, NULL);

}

void CVizNetGaugeDlg::Plot()
{
	GetStatsRefresher();

	m_bIsUpload = FALSE;
	PaintGauge();
	m_bIsUpload = TRUE;
	PaintGauge();
}

void CVizNetGaugeDlg::PaintGauge()
{
	CRect rClient;
	GetClientRect(rClient);

	//check if minimized
	if ((rClient.Width() < 1) || (rClient.Height() < 1)) return;

	rClient.bottom = rClient.bottom / 2;

	CDC *pDC = this->GetDC();
	CDC * dcMem = new CDC();
	dcMem->CreateCompatibleDC(pDC);

	CBitmap * bitMap = new CBitmap();
	bitMap->CreateCompatibleBitmap(pDC, rClient.Width(), rClient.Height());
	CBitmap * oldMap = dcMem->SelectObject(bitMap);

	//background
	DrawBackground(dcMem, rClient);

	//grids
	DrawGrid(dcMem, rClient, FALSE);
	DrawGrid(dcMem, rClient, TRUE);

	//bars
	DrawBars(dcMem, rClient);

	//Text background
	//DrawTextBackground(dcMem, rClient);

	//Text
	DrawInfoText(dcMem, rClient);
	DrawSpeedText(dcMem, rClient);

	//Vignette
	//DrawVignette(dcMem, rClient);

	//render
	int iYpos = (m_bIsUpload)*rClient.bottom;
	pDC->BitBlt(rClient.left, rClient.top + iYpos, rClient.Width(), rClient.Height(), dcMem, 0, 0, SRCCOPY);

	//clean up
	dcMem->SelectObject(oldMap);
	bitMap->DeleteObject();
	delete bitMap;
	dcMem->DeleteDC();
	delete dcMem;
	ReleaseDC(pDC);

}

void CVizNetGaugeDlg::DrawBackground(CDC * pDC, CRect clRect)
{
	COLORREF crBack = RGB(m_uBkIntensity, m_uBkIntensity, m_uBkIntensity);
	pDC->FillSolidRect(clRect, crBack);
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

	if (m_bIsUpload)
	{
		//normalize
		for (UINT i = 0; i < VNG_MAX_SAMPLES; i++)
		{
			if (m_uUploadSamples[m_uSelectedInterface][i] > uMax)uMax = m_uUploadSamples[m_uSelectedInterface][i];
		}
		if (uMax < 1)return;

		for (UINT i = 0; i < VNG_MAX_SAMPLES; i++)
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
		for (UINT i = 0; i < VNG_MAX_SAMPLES; i++)
		{
			if (m_uDownloadSamples[m_uSelectedInterface][i] > uMax)uMax = m_uDownloadSamples[m_uSelectedInterface][i];
		}
		if (uMax < 1)return;

		for (UINT i = 0; i < VNG_MAX_SAMPLES; i++)
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
	sValue.Format(_T("Peak %d Kbps"), m_uMaxValue);
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
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CVizNetGaugeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(m_uTimer);
	WMICleanup();
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
	pNameSpace = NULL;
	pWbemLocator = NULL;
	m_hrResult = S_OK;
	m_bIsInitHandles = TRUE;

	BSTR bstrNameSpace = NULL;
	long lID = 0;

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

	if (FAILED(m_hrResult = pRefresher->QueryInterface(
		IID_IWbemConfigureRefresher,
		(void **)&pConfig)))
	{
		WMICleanup();
		return 1;
	}

	// Add an enumerator to the refresher.
	if (FAILED(m_hrResult = pConfig->AddEnum(
		pNameSpace,
//		L"Win32_PerfRawData_PerfProc_Process",
		L"Win32_PerfFormattedData_Tcpip_NetworkInterface",
		0,
		NULL,
		&pEnum,
		&lID)))
	{
		WMICleanup();
		return 1;
	}
	pConfig->Release();
	pConfig = NULL;

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

	// First time through, get the handles.
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

		m_bIsInitHandles = FALSE;
	}

	m_uDownloadSpeed = 0;
	m_uUploadSpeed = 0;

	for (i = 0; i < dwNumReturned; i++)
	{
		long nb = 0;
		byte data[200];
		DWORD dwBytesRecdPerSec = 0;
		DWORD dwBytesSentPerSec = 0;

		if (FAILED(hr = apEnumAccess[i]->ReadPropertyValue(m_lInterfaceNameHandle, 200, &nb, data)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(m_lBytesReceivedPerSecHandle, &dwBytesRecdPerSec)))
		{
			goto CLEANUP;
		}
		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(m_lBytesSentPerSecHandle, &dwBytesSentPerSec)))
		{
			goto CLEANUP;
		}

		//interface names
		wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
		CString sName(wstr.c_str());
		m_sNetInterfaces[i] = sName;

		//sample
		InsertSample(i, dwBytesRecdPerSec, dwBytesSentPerSec);

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
	if (NULL != pConfig)
	{
		pConfig->Release();
	}
	if (NULL != pRefresher)
	{
		pRefresher->Release();
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

