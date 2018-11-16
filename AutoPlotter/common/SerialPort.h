#pragma once



#include <string>
using namespace std;

#include "synctool.h"

class SerialPort
{
public:
	SerialPort(void);
	~SerialPort(void);
public:
	bool	Open( const char *serialPortName,DWORD	dwBaudRate,BYTE	bByteSize,BYTE   bParity,BYTE bStopBits,bool	bTrace );
	void	Close() ;

	unsigned		Read( void* dataBuf, unsigned dataNeedRead ,unsigned timeOut ) ;

	unsigned		Write( void* data, unsigned dataNeedWrite ) ;

	void	Clear() ;

	HANDLE			GetHandle() const { return m_hComm;}
private:
	///���Com�ڴ���
	void	ClearComm() ;
	
	///�����������߳�
	static	unsigned __stdcall	_ReadProc( void *	pParam ) ;

	///�����������߳���ִ����
	void	ReadImpl() ;

	///���������ݶ���������ݻ�����
	void	ReadToDeque() ;

private:
	//		
	string		m_TraceHead;

	bool		m_bTrace;//�Ƿ��¼trace
	///com�ھ��
	HANDLE		m_hComm ;
	
	///�����߳��˳��ź�
	HANDLE		m_hStopEvent ;

	///�첽���¼�֪ͨ���
	HANDLE		m_hReadEvent ;

	///�첽д�¼�֪ͨ���
	HANDLE		m_hWriteEvent;

	///���ڶ�ȡ�����߳̾��
	HANDLE		m_hIOThread ;
	
	///com�������������
	string		m_ReadBuf ;
	
	///ͬ��m_deqReadBuf���ٽ���
	UCriticalSection		m_crsReadBuf ; 
	
	//
	UNotifier	m_ntfReadBuf ;	

};

