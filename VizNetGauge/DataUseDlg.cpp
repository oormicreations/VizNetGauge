// DataUseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VizNetGauge.h"
#include "DataUseDlg.h"
#include "afxdialogex.h"


// CDataUseDlg dialog

IMPLEMENT_DYNAMIC(CDataUseDlg, CDialogEx)

CDataUseDlg::CDataUseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DATAUSE, pParent)
{

}

CDataUseDlg::~CDataUseDlg()
{
}

void CDataUseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDataUseDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDataUseDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_WARN, &CDataUseDlg::OnBnClickedCheckWarn)
	ON_BN_CLICKED(IDC_CHECK_REN1, &CDataUseDlg::OnBnClickedCheckRen1)
	ON_BN_CLICKED(IDC_CHECK_REN2, &CDataUseDlg::OnBnClickedCheckRen2)
END_MESSAGE_MAP()


// CDataUseDlg message handlers


BOOL CDataUseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	SetDlgItemInt(IDC_EDIT_MAXDATA, m_lMaxData);
	SetDlgItemInt(IDC_EDIT_RENDATA, m_uPeriod1);
	SetDlgItemInt(IDC_EDIT_RENDATA2, m_uPeriod2);
	SetDlgItemInt(IDC_EDIT_THRES, m_lThreshold);

	((CButton*) GetDlgItem(IDC_CHECK_WARN))->SetCheck(m_bWarn);
	((CButton*) GetDlgItem(IDC_CHECK_POP))->SetCheck(m_bWarnPop);
	((CButton*) GetDlgItem(IDC_CHECK_DING))->SetCheck(m_bWarnDing);
	((CButton*) GetDlgItem(IDC_CHECK_VOICE))->SetCheck(m_bWarnVoice);
	((CButton*) GetDlgItem(IDC_CHECK_REN1))->SetCheck(m_bRenewDay);
	((CButton*) GetDlgItem(IDC_CHECK_REN2))->SetCheck(m_bRenewMonth);

	((CButton*)GetDlgItem(IDC_EDIT_RENDATA))->EnableWindow(m_bRenewDay);
	((CButton*)GetDlgItem(IDC_EDIT_RENDATA2))->EnableWindow(m_bRenewMonth);
	((CButton*)GetDlgItem(IDC_CHECK_POP))->EnableWindow(m_bWarn);
	((CButton*)GetDlgItem(IDC_CHECK_DING))->EnableWindow(m_bWarn);
	((CButton*)GetDlgItem(IDC_CHECK_VOICE))->EnableWindow(m_bWarn);
	((CButton*)GetDlgItem(IDC_EDIT_THRES))->EnableWindow(m_bWarn);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDataUseDlg::OnBnClickedOk()
{
	m_lMaxData = GetDlgItemInt(IDC_EDIT_MAXDATA);
	m_uPeriod1 = GetDlgItemInt(IDC_EDIT_RENDATA);
	m_uPeriod2 = GetDlgItemInt(IDC_EDIT_RENDATA2);
	m_lThreshold = GetDlgItemInt(IDC_EDIT_THRES);

	m_bWarn = ((CButton*)GetDlgItem(IDC_CHECK_WARN))->GetCheck();
	m_bWarnPop = ((CButton*)GetDlgItem(IDC_CHECK_POP))->GetCheck();
	m_bWarnDing = ((CButton*)GetDlgItem(IDC_CHECK_DING))->GetCheck();
	m_bWarnVoice = ((CButton*)GetDlgItem(IDC_CHECK_VOICE))->GetCheck();
	m_bRenewDay = ((CButton*)GetDlgItem(IDC_CHECK_REN1))->GetCheck();
	m_bRenewMonth = ((CButton*)GetDlgItem(IDC_CHECK_REN2))->GetCheck();

	CDialogEx::OnOK();
}


void CDataUseDlg::OnBnClickedCheckWarn()
{
	m_bWarn = ((CButton*)GetDlgItem(IDC_CHECK_WARN))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_POP))->EnableWindow(m_bWarn);
	((CButton*)GetDlgItem(IDC_CHECK_DING))->EnableWindow(m_bWarn);
	((CButton*)GetDlgItem(IDC_CHECK_VOICE))->EnableWindow(m_bWarn);
	((CButton*)GetDlgItem(IDC_EDIT_THRES))->EnableWindow(m_bWarn);
}


void CDataUseDlg::OnBnClickedCheckRen1()
{
	m_bRenewDay = ((CButton*)GetDlgItem(IDC_CHECK_REN1))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_REN2))->SetCheck(!m_bRenewDay);
	((CButton*)GetDlgItem(IDC_EDIT_RENDATA))->EnableWindow(m_bRenewDay);
	((CButton*)GetDlgItem(IDC_EDIT_RENDATA2))->EnableWindow(!m_bRenewDay);
}


void CDataUseDlg::OnBnClickedCheckRen2()
{
	m_bRenewMonth = ((CButton*)GetDlgItem(IDC_CHECK_REN2))->GetCheck();
	((CButton*)GetDlgItem(IDC_CHECK_REN1))->SetCheck(!m_bRenewMonth);
	((CButton*)GetDlgItem(IDC_EDIT_RENDATA))->EnableWindow(!m_bRenewMonth);
	((CButton*)GetDlgItem(IDC_EDIT_RENDATA2))->EnableWindow(m_bRenewMonth);
}
