
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
	m_uTimerDelay = 1000;
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
	DrawVignette(dcMem, rClient);

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
	COLORREF crBar = m_crBarDn;
	int iMaxBarHeight = clRect.Height()/2;
	int iBarVOffset = 1;
	int iXpos = 0;

	if (m_bIsUpload)
	{
		crBar = m_crBarUp;
	}

	for (UINT i = 0; i < m_uBarCount; i++)
	{
		m_uBarHeight = rand() % iMaxBarHeight;
		rBar.SetRect(0 + iXpos, clRect.Height() - m_uBarHeight - iBarVOffset, m_uBarWidth + iXpos, clRect.Height() - iBarVOffset);
		pDC->FillSolidRect(rBar, crBar);
		iXpos = iXpos + m_uBarSpacing + m_uBarWidth;
	}

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
	rText.left = clRect.right /2;
	rText.top = clRect.bottom/6;
	rText.right = clRect.right - 30;
	rText.bottom = clRect.bottom;

	int iFontSzScale = 1 + ((clRect.Width() * clRect.Height()) / (400*400));
	COLORREF crText = RGB(m_uTextIntensityTitle, m_uTextIntensityTitle, m_uTextIntensityTitle);

	CString sValue;
	sValue = _T("RealTek Ethernet\r\nDownload\r\nKbps");
	if (m_bIsUpload) sValue.Replace(_T("Download"), _T("Upload"));

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	CFont fValue;
	fValue.CreatePointFont((int)(m_uTextFontSize * 10 * iFontSzScale), m_sFont);

	CFont * oldFont2 = pDC->SelectObject(&fValue);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crText);
	pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_RIGHT|DT_WORDBREAK, &dtParams);

	pDC->SelectObject(oldFont2);
	fValue.DeleteObject();
}

void CVizNetGaugeDlg::DrawSpeedText(CDC * pDC, CRect clRect)
{
	CRect rText;
	rText.left = clRect.left;
	rText.top = clRect.top;
	rText.right = clRect.right/2;
	rText.bottom = clRect.bottom/2;

	int iFontSzScale = 1 + ((clRect.Width() * clRect.Height()) / (300 * 300));
	COLORREF crValue = RGB(m_uTextIntensityValue, m_uTextIntensityValue, m_uTextIntensityValue);

	CString sValue;
	//m_uDownloadSpeed = rand() % 15000;
	sValue.Format(_T("%d"), m_uDownloadSpeed);

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	CFont fValue;
	fValue.CreatePointFont((int)(m_uValueFontSize * 10 * iFontSzScale), m_sFont);

	CFont * oldFont2 = pDC->SelectObject(&fValue);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crValue);
	pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_CENTER | DT_VCENTER | DT_SINGLELINE, &dtParams);

	pDC->SelectObject(oldFont2);
	fValue.DeleteObject();
}

void CVizNetGaugeDlg::DrawVignette(CDC * pDC, CRect clRect)
{
	int iVigWidth = 20 * clRect.Width()/m_uWinSzMax;
	float fMaxAtten = 1.0f;

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

//WMI Ref: 
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa394293.aspx
//https://docs.microsoft.com/en-us/windows/desktop/WmiSdk/example-creating-a-wmi-application
//https://docs.microsoft.com/en-us/windows/desktop/WmiSdk/accessing-performance-data-in-c--

BOOL CVizNetGaugeDlg::InitWMI()
{
	// To add error checking,
	// check returned HRESULT below where collected.
	HRESULT                 hr = S_OK;
	/*IWbemRefresher          **/pRefresher = NULL;
	IWbemConfigureRefresher *pConfig = NULL;
	/*IWbemHiPerfEnum         **/pEnum = NULL;
	IWbemServices           *pNameSpace = NULL;
	IWbemLocator            *pWbemLocator = NULL;
	//IWbemObjectAccess       **apEnumAccess = NULL;
	BSTR                    bstrNameSpace = NULL;
	long                    lID = 0;
	//long                    lVirtualBytesHandle = 0;
	//long                    lIDProcessHandle = 0;
	//DWORD                   dwVirtualBytes = 0;
	//DWORD                   dwProcessId = 0;
	//DWORD                   dwNumObjects = 0;
	//DWORD                   dwNumReturned = 0;
	//DWORD                   dwIDProcess = 0;
	//DWORD                   i = 0;
	//int                     x = 0;

	if (FAILED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
		goto CLEANUP;
	}

	if (FAILED(hr = CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_NONE,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE, 0)))
	{
		goto CLEANUP;
	}

	if (FAILED(hr = CoCreateInstance(
		CLSID_WbemLocator,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(void**)&pWbemLocator)))
	{
		goto CLEANUP;
	}

	// Connect to the desired namespace.
	bstrNameSpace = SysAllocString(L"\\\\.\\root\\cimv2");
	if (NULL == bstrNameSpace)
	{
		hr = E_OUTOFMEMORY;
		goto CLEANUP;
	}
	if (FAILED(hr = pWbemLocator->ConnectServer(
		bstrNameSpace,
		NULL, // User name
		NULL, // Password
		NULL, // Locale
		0L,   // Security flags
		NULL, // Authority
		NULL, // Wbem context
		&pNameSpace)))
	{
		goto CLEANUP;
	}
	pWbemLocator->Release();
	pWbemLocator = NULL;
	SysFreeString(bstrNameSpace);
	bstrNameSpace = NULL;

	if (FAILED(hr = CoCreateInstance(
		CLSID_WbemRefresher,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWbemRefresher,
		(void**)&pRefresher)))
	{
		goto CLEANUP;
	}

	if (FAILED(hr = pRefresher->QueryInterface(
		IID_IWbemConfigureRefresher,
		(void **)&pConfig)))
	{
		goto CLEANUP;
	}

	// Add an enumerator to the refresher.
	if (FAILED(hr = pConfig->AddEnum(
		pNameSpace,
//		L"Win32_PerfRawData_PerfProc_Process",
		L"Win32_PerfFormattedData_Tcpip_NetworkInterface",
		0,
		NULL,
		&pEnum,
		&lID)))
	{
		goto CLEANUP;
	}
	pConfig->Release();
	pConfig = NULL;

	//// Get a property handle for the VirtualBytes property.

	//// Refresh the object ten times and retrieve the value.
	//for (x = 0; x < 10; x++)
	//{
	//	dwNumReturned = 0;
	//	dwIDProcess = 0;
	//	dwNumObjects = 0;

	//	if (FAILED(hr = pRefresher->Refresh(0L)))
	//	{
	//		goto CLEANUP;
	//	}

	//	hr = pEnum->GetObjects(0L,
	//		dwNumObjects,
	//		apEnumAccess,
	//		&dwNumReturned);
	//	// If the buffer was not big enough,
	//	// allocate a bigger buffer and retry.
	//	if (hr == WBEM_E_BUFFER_TOO_SMALL
	//		&& dwNumReturned > dwNumObjects)
	//	{
	//		apEnumAccess = new IWbemObjectAccess*[dwNumReturned];
	//		if (NULL == apEnumAccess)
	//		{
	//			hr = E_OUTOFMEMORY;
	//			goto CLEANUP;
	//		}
	//		SecureZeroMemory(apEnumAccess,
	//			dwNumReturned * sizeof(IWbemObjectAccess*));
	//		dwNumObjects = dwNumReturned;

	//		if (FAILED(hr = pEnum->GetObjects(0L,
	//			dwNumObjects,
	//			apEnumAccess,
	//			&dwNumReturned)))
	//		{
	//			goto CLEANUP;
	//		}
	//	}
	//	else
	//	{
	//		if (hr == WBEM_S_NO_ERROR)
	//		{
	//			hr = WBEM_E_NOT_FOUND;
	//			goto CLEANUP;
	//		}
	//	}

	//	// First time through, get the handles.
	//	if (0 == x)
	//	{
	//		CIMTYPE VirtualBytesType;
	//		CIMTYPE ProcessHandleType;
	//		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
	//			L"Name",
	//			&VirtualBytesType,
	//			&lVirtualBytesHandle)))
	//		{
	//			goto CLEANUP;
	//		}
	//		if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
	//			L"BytesReceivedPerSec",
	//			&ProcessHandleType,
	//			&lIDProcessHandle)))
	//		{
	//			goto CLEANUP;
	//		}

	//		int t = 0;
	//	}

	//	m_uDownloadSpeed = 0;
	//	for (i = 0; i < dwNumReturned; i++)
	//	{
	//		long nb = 0;
	//		byte data[200];
	//		DWORD dwBytesRecPerSec = 0;

	//		//hr = apEnumAccess[i]->ReadPropertyValue(lVirtualBytesHandle, 200, &nb, data);
	//		//wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
	//		//CString d(wstr.c_str());

	//		if (FAILED(hr = apEnumAccess[i]->ReadPropertyValue(lVirtualBytesHandle, 200, &nb, data)))
	//		{
	//			goto CLEANUP;
	//		}
	//		if (FAILED(hr = apEnumAccess[i]->ReadDWORD(lIDProcessHandle,&dwBytesRecPerSec)))
	//		{
	//			goto CLEANUP;
	//		}

	//		wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
	//		CString sName(wstr.c_str());

	//		m_uDownloadSpeed += dwBytesRecPerSec; //total from all network interfaces

	//		//if (FAILED(hr = apEnumAccess[i]->ReadDWORD(
	//		//	lVirtualBytesHandle,
	//		//	&dwVirtualBytes)))
	//		//{
	//		//	goto CLEANUP;
	//		//}
	//		//if (FAILED(hr = apEnumAccess[i]->ReadDWORD(
	//		//	lIDProcessHandle,
	//		//	&dwIDProcess)))
	//		//{
	//		//	goto CLEANUP;
	//		//}
	//		//TRACE(L"Process ID %lu is using %lu bytes\n",
	//		//	dwIDProcess, dwVirtualBytes);
	//		//wprintf(L"Process ID %lu is using %lu bytes\n",
	//		//	dwIDProcess, dwVirtualBytes);

	//		// Done with the object
	//		apEnumAccess[i]->Release();
	//		apEnumAccess[i] = NULL;
	//	}

	//	if (NULL != apEnumAccess)
	//	{
	//		delete[] apEnumAccess;
	//		apEnumAccess = NULL;
	//	}

	//	// Sleep for a second.
	//	Sleep(1000);
	//}
	//// exit loop here
CLEANUP:

	if (NULL != bstrNameSpace)
	{
		SysFreeString(bstrNameSpace);
	}

	//if (NULL != apEnumAccess)
	//{
	//	for (i = 0; i < dwNumReturned; i++)
	//	{
	//		if (apEnumAccess[i] != NULL)
	//		{
	//			apEnumAccess[i]->Release();
	//			apEnumAccess[i] = NULL;
	//		}
	//	}
	//	delete[] apEnumAccess;
	//}


	//if (NULL != pWbemLocator)
	//{
	//	pWbemLocator->Release();
	//}
	//if (NULL != pNameSpace)
	//{
	//	pNameSpace->Release();
	//}
	//if (NULL != pEnum)
	//{
	//	pEnum->Release();
	//}
	//if (NULL != pConfig)
	//{
	//	pConfig->Release();
	//}
	//if (NULL != pRefresher)
	//{
	//	pRefresher->Release();
	//}

	//CoUninitialize();

	if (FAILED(hr))
	{
		wprintf(L"Error status=%08x\n", hr);
	}

	return 1;
}

BOOL CVizNetGaugeDlg::GetStatsRefresher()
{
	HRESULT                 hr = S_OK;
	long                    lVirtualBytesHandle = 0;
	long                    lIDProcessHandle = 0;
	DWORD                   dwVirtualBytes = 0;
	DWORD                   dwProcessId = 0;
	DWORD                   dwNumObjects = 0;
	DWORD                   dwNumReturned = 0;
	DWORD                   dwIDProcess = 0;
	DWORD                   i = 0;
	int                     x = 0;
	IWbemObjectAccess       **apEnumAccess = NULL;



	// Get a property handle for the VirtualBytes property.

	// Refresh the object ten times and retrieve the value.
	//for (x = 0; x < 10; x++)
	//{
		dwNumReturned = 0;
		dwIDProcess = 0;
		dwNumObjects = 0;

		if (FAILED(hr = pRefresher->Refresh(0L)))
		{
			goto CLEANUP;
		}

		hr = pEnum->GetObjects(0L,
			dwNumObjects,
			apEnumAccess,
			&dwNumReturned);
		// If the buffer was not big enough,
		// allocate a bigger buffer and retry.
		if (hr == WBEM_E_BUFFER_TOO_SMALL
			&& dwNumReturned > dwNumObjects)
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
		if (0 == x)
		{
			CIMTYPE VirtualBytesType;
			CIMTYPE ProcessHandleType;
			if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
				L"Name",
				&VirtualBytesType,
				&lVirtualBytesHandle)))
			{
				goto CLEANUP;
			}
			if (FAILED(hr = apEnumAccess[0]->GetPropertyHandle(
				L"BytesReceivedPerSec",
				&ProcessHandleType,
				&lIDProcessHandle)))
			{
				goto CLEANUP;
			}

			int t = 0;
		}

		m_uDownloadSpeed = 0;
		for (i = 0; i < dwNumReturned; i++)
		{
			long nb = 0;
			byte data[200];
			DWORD dwBytesRecPerSec = 0;

			//hr = apEnumAccess[i]->ReadPropertyValue(lVirtualBytesHandle, 200, &nb, data);
			//wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
			//CString d(wstr.c_str());

			if (FAILED(hr = apEnumAccess[i]->ReadPropertyValue(lVirtualBytesHandle, 200, &nb, data)))
			{
				goto CLEANUP;
			}
			if (FAILED(hr = apEnumAccess[i]->ReadDWORD(lIDProcessHandle, &dwBytesRecPerSec)))
			{
				goto CLEANUP;
			}

			wstring wstr(reinterpret_cast<wchar_t*>(data), nb / sizeof(wchar_t));
			CString sName(wstr.c_str());

			m_uDownloadSpeed += dwBytesRecPerSec; //total from all network interfaces

			//if (FAILED(hr = apEnumAccess[i]->ReadDWORD(
			//	lVirtualBytesHandle,
			//	&dwVirtualBytes)))
			//{
			//	goto CLEANUP;
			//}
			//if (FAILED(hr = apEnumAccess[i]->ReadDWORD(
			//	lIDProcessHandle,
			//	&dwIDProcess)))
			//{
			//	goto CLEANUP;
			//}
			//TRACE(L"Process ID %lu is using %lu bytes\n",
			//	dwIDProcess, dwVirtualBytes);
			//wprintf(L"Process ID %lu is using %lu bytes\n",
			//	dwIDProcess, dwVirtualBytes);

			// Done with the object
			apEnumAccess[i]->Release();
			apEnumAccess[i] = NULL;
		}

		if (NULL != apEnumAccess)
		{
			delete[] apEnumAccess;
			apEnumAccess = NULL;
		}

		// Sleep for a second.
	//	Sleep(1000);
	//}
	// exit loop here

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


BOOL CVizNetGaugeDlg::GetStats()
{
	HRESULT hres;
	IWbemServices *pSvc = NULL;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		//cout << "Failed to initialize COM library. Error code = 0x"
		//	<< hex << hres << endl;
		return 1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres))
	{
		//cout << "Failed to initialize security. Error code = 0x"
		//	<< hex << hres << endl;
		CoUninitialize();
		return 1;                    // Program has failed.
	}



	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		//cout << "Failed to create IWbemLocator object."
		//	<< " Err code = 0x"
		//	<< hex << hres << endl;
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		//cout << "Could not connect. Error code = 0x"
		//	<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return 1;                // Program has failed.
	}

	//cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		//cout << "Could not set proxy blanket. Error code = 0x"
		//	<< hex << hres << endl;
		pSvc->Release();
		//pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		//bstr_t("SELECT * FROM Win32_OperatingSystem"),
		bstr_t("SELECT * FROM Win32_PerfFormattedData_Tcpip_NetworkInterface"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		//cout << "Query for operating system name failed."
		//	<< " Error code = 0x"
		//	<< hex << hres << endl;
		pSvc->Release();
		//pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	m_uDownloadSpeed = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		//wcout << "Name : " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"BytesTotalPerSec", 0, &vtProp, 0, 0);
		//wcout << "BytesTotalPerSec : " << vtProp.bstrVal << endl;

		BSTR b = vtProp.bstrVal;
		m_uDownloadSpeed += _ttoi(b);

		VariantClear(&vtProp);

		pclsObj->Release();
	}

	// Cleanup
	// ========

	//pSvc->Release();
	//pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return 0;
}