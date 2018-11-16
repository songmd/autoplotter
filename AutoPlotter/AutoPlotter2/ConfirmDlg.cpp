// ConfirmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPlotter2.h"
#include "ConfirmDlg.h"
#include "afxdialogex.h"


// CConfirmDlg dialog

IMPLEMENT_DYNAMIC(CConfirmDlg, CDialogEx)

CConfirmDlg::CConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConfirmDlg::IDD, pParent)
{

	m_strProductId = _T("");
	m_strNote = _T("");
	m_strOrderNoDesc = _T("");
	m_strOrderId = _T("");
	m_strOrderType = _T("");
	m_strTypeOld = _T("");
}

CConfirmDlg::~CConfirmDlg()
{
}

void CConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ID, m_strProductId);
	DDX_Text(pDX, IDC_EDIT_NOTE, m_strNote);
	DDX_Text(pDX, IDC_EDIT_ORDER_NO, m_strOrderNoDesc);
	DDX_Text(pDX, IDC_EDIT_ORDERID, m_strOrderId);
	DDX_Text(pDX, IDC_EDIT_TYPE, m_strOrderType);
	DDX_Text(pDX, IDC_EDIT_TYPE_OLD, m_strTypeOld);
}


BEGIN_MESSAGE_MAP(CConfirmDlg, CDialogEx)

END_MESSAGE_MAP()


// CConfirmDlg message handlers





BOOL CConfirmDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


