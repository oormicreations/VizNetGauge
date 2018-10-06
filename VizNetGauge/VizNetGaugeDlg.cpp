
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
	m_uWinSzMin = 200;
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

	m_Timer = SetTimer(WM_USER + 200, 500, NULL);

}

void CVizNetGaugeDlg::PaintGauge()
{
	CRect rClient;
	GetClientRect(rClient);

	CDC *pDC = this->GetDC();
	CDC * dcMem = new CDC();
	dcMem->CreateCompatibleDC(pDC);

	CBitmap * bitMap = new CBitmap();
	bitMap->CreateCompatibleBitmap(pDC, rClient.Width(), rClient.Height());
	CBitmap * oldMap = dcMem->SelectObject(bitMap);

	//background
	DrawBackground(dcMem, rClient);

	//grid
	DrawGrid(dcMem, rClient, FALSE);
	DrawGrid(dcMem, rClient, TRUE);

	//bars
	DrawBars(dcMem, rClient);

	//Text background
	DrawTextBackground(dcMem, rClient);

	//Text
	DrawText(dcMem, rClient);

	//render
	pDC->BitBlt(rClient.left, rClient.top, rClient.Width(), rClient.Height(), dcMem, 0, 0, SRCCOPY);

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
	for (int i = 0; i <= iGridHCount; i++)
	{
		pDC->MoveTo(0, uSpacing * i);
		pDC->LineTo(clRect.right, uSpacing * i);
	}

	pDC->SelectObject(oldPen);
	pnGrid.DeleteObject();

}

void CVizNetGaugeDlg::DrawBars(CDC * pDC, CRect clRect)
{
	CRect rBar;
	int iBarVOffset = clRect.Height() / 2;
	COLORREF crBar = m_crBarDn;
	//if (bUpload)
	//{
	//	iBarVOffset = clRect.Height() / 40;
	//	crBar = m_crBarUp;
	//}
	int iXpos = 0;

	for (UINT i = 0; i < m_uBarCount; i++)
	{
		m_uBarHeight = rand() % 100;
		rBar.SetRect(0 + iXpos, clRect.Height() - m_uBarHeight - iBarVOffset, m_uBarWidth + iXpos, clRect.Height() - iBarVOffset);
		pDC->FillSolidRect(rBar, crBar);
		iXpos = iXpos + m_uBarSpacing + m_uBarWidth;
	}

}

void CVizNetGaugeDlg::DrawTextBackground(CDC * pDC, CRect clRect)
{
	float fAtten = 1.33f;
	for (int y = 11; y < 60; y++)
	{
		for (int x = 10; x < 120; x++)
		{
			if (((x - 30)*(x - 30) + (y - 30)*(y - 30) < 400) || ((x - 30)*(x - 30) + (y - 40)*(y - 40) < 400) ||
				((x - 90)*(x - 90) + (y - 30)*(y - 30) < 400) || ((x - 90)*(x - 90) + (y - 40)*(y - 40) < 400) ||
				((x>30) && (x<90))
				)
			{
				COLORREF crPix = pDC->GetPixel(x, y);
				pDC->SetPixel(x, y, RGB(GetRValue(crPix) / fAtten, GetGValue(crPix) / fAtten, GetBValue(crPix) / fAtten));
			}
		}
	}

}

void CVizNetGaugeDlg::DrawText(CDC * pDC, CRect clRect)
{
	CRect rText;

	COLORREF crText = RGB(m_uTextIntensityTitle, m_uTextIntensityTitle, m_uTextIntensityTitle);
	COLORREF crValue = RGB(m_uTextIntensityValue, m_uTextIntensityValue, m_uTextIntensityValue);

	CString sValue;
	m_uDownloadSpeed = rand() % 15000;

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	sValue = _T("Download: Kbps");
	rText.SetRect(20, 15, 200, 30);

	CFont fTitle;
	fTitle.CreatePointFont((int)(m_uTextFontSize * 10), m_sFont);

	CFont * oldFont1 = pDC->SelectObject(&fTitle);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crText);
	int ht2 = pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_LEFT, &dtParams);

	pDC->SelectObject(oldFont1);
	fTitle.DeleteObject();

	sValue.Format(_T("%d"), m_uDownloadSpeed);
	rText.SetRect(20, 25, 100, 60);

	CFont fValue;
	fValue.CreatePointFont((int)(m_uValueFontSize * 10), m_sFont);

	CFont * oldFont2 = pDC->SelectObject(&fValue);
	//pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(crValue);
	ht2 = pDC->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_RIGHT, &dtParams);

	pDC->SelectObject(oldFont2);
	fValue.DeleteObject();
}

void CVizNetGaugeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_Timer)
	{
		PaintGauge();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CVizNetGaugeDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	KillTimer(m_Timer);
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
