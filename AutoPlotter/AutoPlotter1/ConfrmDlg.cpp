// ConfrmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPlotter1.h"
#include "ConfrmDlg.h"
#include "afxdialogex.h"


// CConfrmDlg dialog

IMPLEMENT_DYNAMIC(CConfrmDlg, CDialogEx)

CConfrmDlg::CConfrmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfrmDlg::IDD, pParent)
{

	m_strNO = _T("");
	m_strTime = _T("");

	m_strProductId = _T("");
	m_strNote = _T("");
	m_strOrderNoDesc = _T("");
	m_strOrderId = _T("");
	m_strOrderType = _T("");

}

CConfrmDlg::~CConfrmDlg()
{
}

void CConfrmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PNO, m_strNO);
	DDX_Text(pDX, IDC_EDIT_TIME, m_strTime);
	DDX_Text(pDX, IDC_EDIT_PID, m_strProductId);
	DDX_Text(pDX, IDC_EDIT_NOTE, m_strNote);
	DDX_Text(pDX, IDC_EDIT_ORDER_NO, m_strOrderNoDesc);
	DDX_Text(pDX, IDC_EDIT_ORDERID, m_strOrderId);
	DDX_Text(pDX, IDC_EDIT_TYPE, m_strOrderType);
}


BEGIN_MESSAGE_MAP(CConfrmDlg, CDialogEx)
END_MESSAGE_MAP()


// CConfrmDlg message handlers


BOOL CConfrmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
