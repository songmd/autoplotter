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
	///清除Com口错误
	void	ClearComm() ;
	
	///读串口数据线程
	static	unsigned __stdcall	_ReadProc( void *	pParam ) ;

	///读串口数据线程主执行体
	void	ReadImpl() ;

	///将串口数据读到输出数据缓存区
	void	ReadToDeque() ;

private:
	//		
	string		m_TraceHead;

	bool		m_bTrace;//是否记录trace
	///com口句柄
	HANDLE		m_hComm ;
	
	///输入线程退出信号
	HANDLE		m_hStopEvent ;

	///异步读事件通知句柄
	HANDLE		m_hReadEvent ;

	///异步写事件通知句柄
	HANDLE		m_hWriteEvent;

	///串口读取输入线程句柄
	HANDLE		m_hIOThread ;
	
	///com口数据输出缓存
	string		m_ReadBuf ;
	
	///同步m_deqReadBuf的临界量
	UCriticalSection		m_crsReadBuf ; 
	
	//
	UNotifier	m_ntfReadBuf ;	

};

