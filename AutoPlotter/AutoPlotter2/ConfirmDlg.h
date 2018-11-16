#pragma once


// CConfirmDlg dialog

class CConfirmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfirmDlg)

public:
	CConfirmDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfirmDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONFIRM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:


	void	SaveSetting();
	virtual BOOL OnInitDialog();
	CString m_strProductId;
	CString m_strNote;
	CString m_strOrderNoDesc;
	CString m_strOrderId;
	CString m_strOrderType;
	CString m_strTypeOld;

};
