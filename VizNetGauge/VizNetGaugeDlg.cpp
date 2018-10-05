
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
	m_uDownloadSpeed = m_uUploadSpeed = 0;

	m_uValueFontSize = 20;
	m_uTextFontSize = 10;
	m_sFont = _T("Calibri");

	m_Timer = SetTimer(WM_USER + 200, 100, NULL);

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


void CVizNetGaugeDlg::PaintGauge()
{
	CRect rClient, rBar, rText;
	GetClientRect(rClient);
	rBar.SetRect(0, 0, 5, 50);

	CDC *pDC = this->GetDC();
	CDC * dcMem = new CDC();
	dcMem->CreateCompatibleDC(pDC);

	CBitmap * bitMap = new CBitmap();
	bitMap->CreateCompatibleBitmap(pDC, rClient.Width(), rClient.Height());
	CBitmap * oldMap = dcMem->SelectObject(bitMap);

	//background
	COLORREF crBack = RGB(60, 60, 60);
	dcMem->FillSolidRect(rClient, crBack);

	//grid
	COLORREF crGrid = RGB(80, 80, 80);
	CPen pnGrid;
	pnGrid.CreatePen(0, 1, crGrid);
	CPen * oldPen = dcMem->SelectObject(&pnGrid);

	int iGridSpacing = 10;
	int iGridHCount = rClient.Height()/ iGridSpacing;
	int iGridVCount = rClient.Width() / iGridSpacing;

	for (int i = 0; i < iGridVCount; i++)
	{
		dcMem->MoveTo(iGridSpacing * i, 0);
		dcMem->LineTo(iGridSpacing * i, rClient.bottom);
	}
	for (int i = 0; i < iGridHCount; i++)
	{
		dcMem->MoveTo(0, iGridSpacing * i);
		dcMem->LineTo(rClient.right, iGridSpacing * i);
	}

	dcMem->SelectObject(oldPen);
	pnGrid.DeleteObject();

	


	//bars
	COLORREF crBar = RGB(20, 200, 200);
	int iBarHeight = 40;
	int iBarWidth = 5;
	int iBarCount = 200;
	int iBarSpacing = 2;
	int iBarVOffset = rClient.Height() / 2;
	int iXpos = 0;

	for (int i = 0; i < iBarCount; i++)
	{
		iBarHeight = rand() % 100;
		rBar.SetRect(0 + iXpos, rClient.Height() - iBarHeight - iBarVOffset, iBarWidth + iXpos, rClient.Height() - iBarVOffset);
		dcMem->FillSolidRect(rBar, crBar);
		iXpos = iXpos + iBarSpacing + iBarWidth;
	}

	//Text background
	//rText.SetRect(10, 10, 120, 60);
	//CBrush br;
	//br.CreateSolidBrush(RGB(90, 90, 90));
	//dcMem->SelectObject(&br);
	//dcMem->RoundRect(rText, CPoint(15, 15));






	//Text
	COLORREF crText = RGB(160, 160, 160);
	COLORREF crValue = RGB(255, 255, 255);
	CString sValue;
	m_uDownloadSpeed = rand() % 5000;

	DRAWTEXTPARAMS dtParams;
	dtParams.cbSize = sizeof(DRAWTEXTPARAMS);
	dtParams.iLeftMargin = 0;
	dtParams.iRightMargin = 0;
	dtParams.iTabLength = 1;

	sValue = _T("Download: Kbps");
	rText.SetRect(10, 0, 200, 20);

	CFont fTitle;
	fTitle.CreatePointFont((int)(m_uTextFontSize * 10), m_sFont);

	CFont * oldFont1 = dcMem->SelectObject(&fTitle);
	dcMem->SetBkMode(TRANSPARENT);
	dcMem->SetTextColor(crText);
	int ht2 = dcMem->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_LEFT, &dtParams);

	dcMem->SelectObject(oldFont1);
	fTitle.DeleteObject();

	sValue.Format(_T("%d"), m_uDownloadSpeed);
	rText.SetRect(10, ht2, 200, ht2+60);

	CFont fValue;
	fValue.CreatePointFont((int)(m_uValueFontSize * 10), m_sFont);

	CFont * oldFont2 = dcMem->SelectObject(&fValue);
	//dcMem->SetBkMode(TRANSPARENT);
	dcMem->SetTextColor(crValue);
	ht2 = dcMem->DrawTextEx(sValue.GetBuffer(), -1, rText, DT_LEFT, &dtParams);

	dcMem->SelectObject(oldFont2);
	fValue.DeleteObject();


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

	// TODO: Add your message handler code here
	KillTimer(m_Timer);
}
