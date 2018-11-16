
// AutoPlotter2Dlg.h : ͷ�ļ�
//

#pragma once

#include "../common/Plotter.h"
#include "../common/common.h"
#include <fstream>
using namespace std;

enum HostMode
{
	MODE_ALL = -1,
	MODE_IBMHID = 0,
	MODE_HIDKB = 1,
	MODE_USB_OPOS = 2,
	MODE_SNAPI_IMG = 3,
	MODE_SNAPI_NO_IMG = 4,
	MODE_USB_CDC = 5,
	MODE_USB_SSI_CDC = 6,
	MODE_IBMTT = 7,
	MODE_SSI = 8,
	MODE_SSI_BT =9
};

//This structure is the basic unit of a Scanner List which contains Information for all
//discovered scanners
typedef struct _SCANNER
{
	CString Type;
	int ID;
	CString Model;
	CString Serial;
	CString GUID;
	CString VID;
	CString PID;
	CString DoM;
	CString Firmware;
	int HostMode;

} SCANNER, *PSCANNER;

// CAutoPlotter2Dlg �Ի���
class CAutoPlotter2Dlg : public CDialogEx
{
// ����
public:
	CAutoPlotter2Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTOPLOTTER2_DIALOG };

	void OnScanData(short reserved, BSTR scanData);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��



// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	class CScannerCommands *m_ScannerCommands;
	CStringArray	m_logs;
	CPlotter		m_plotter;
	bool	m_bNeedWait;
	bool	m_bScanning;
public:
	CEdit m_logEdit;


	vector<stOrder>	m_vctOrders;
	void UpdateLogView(void);
	void	CreateLog(CTime ct,int ordno, CString orderid,CString type,CString id,CString fail);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnCancel();
	CEdit m_editStatus;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	CComboBox m_ctrlOrders;
	CString m_strOrder;
	afx_msg void OnSelchangeComboOrder();
	CString m_strNote;
	int m_nOrderTotalNum;
	CString m_strOrderType;
	int m_nOrderCompNum;
	virtual void OnOK();
private:
	void UpdateByOrder(CString order);
public:
	afx_msg void OnBnClickedButtonTest();
};
