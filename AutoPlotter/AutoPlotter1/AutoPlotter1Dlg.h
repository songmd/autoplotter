
// AutoPlotter1Dlg.h : ͷ�ļ�
//

#pragma once

#include "../common/Plotter.h"
#include "../common/common.h"
#include <fstream>
using namespace std;

// CAutoPlotter1Dlg �Ի���
class CAutoPlotter1Dlg : public CDialogEx
{
// ����
public:
	CAutoPlotter1Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AUTOPLOTTER1_DIALOG };

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
	virtual void OnCancel();
	virtual void OnOK();
public:
	CEdit m_logEdit;
	afx_msg void OnBnClickedButtonRun();

private:

	CStringArray	m_logs;
	CPlotter		m_plotter;
	bool			m_bNeedWait;
	CTime			m_today;
	vector<stOrder>	m_vctOrders;
public:
	void SaveSetting(void);
	void UpdateLogView(void);
	void	CreateLog(CTime ct,int ordno, CString orderid,int nNo,CString type,CString id,CString fail);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_btnRun;
	int m_productNo;
	CString CreateID(CTime ct,int nPNo);
	CString	CreateCode1(int no);
	CString	CreateCode2(int no);

	CComboBox m_ctrlOrders;
	CString m_strOrder;
	afx_msg void OnSelchangeComboOrder();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	CString m_strNote;
	int m_nOrderTotalNum;
	CString m_strOrderType;
	int m_nOrderCompNum;

private:
	void UpdateByOrder(CString order);
};
