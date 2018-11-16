#include "StdAfx.h"
#include "SerialPort.h"
#include "itrace.h"

SerialPort::SerialPort(void):m_ntfReadBuf(NULL)
{
	m_hComm = NULL ;
	m_hStopEvent = NULL ;
	m_hReadEvent = NULL ;
	m_hWriteEvent = NULL ;
	m_hIOThread = NULL ;
	m_bTrace = false;

}

SerialPort::~SerialPort(void)
{
	Close() ;
}
bool	SerialPort::Open ( const char *serialPortName,DWORD	dwBaudRate,BYTE	bByteSize,BYTE   bParity,BYTE bStopBits,bool	bTrace )
{
	m_TraceHead = string(serialPortName)  ;

	m_hReadEvent = CreateEvent( NULL , TRUE , FALSE , NULL ) ;
	m_hWriteEvent = CreateEvent( NULL , TRUE , FALSE , NULL ) ;
	m_hStopEvent = CreateEvent( NULL , TRUE , FALSE , NULL ) ;
	if( m_hReadEvent == NULL || m_hWriteEvent == NULL || m_hStopEvent == NULL )
	{
		TrcWritef( "SerialPort",SY_FATAL , "CreateEvent failed, last error code = %d",::GetLastError() ) ;
		return false ;
	}

	m_bTrace = bTrace ;

	m_hComm = ::CreateFileA( serialPortName ,
		GENERIC_READ | GENERIC_WRITE ,
		0,                // comm devices must be opened w/exclusive-access
		NULL,             // no security attributes
		OPEN_EXISTING,    // comm devices must use OPEN_EXISTING
		FILE_FLAG_OVERLAPPED, // overlapped I/O
		NULL);            // hTemplate must be NULL for comm devices
	if( m_hComm == INVALID_HANDLE_VALUE )
	{
		TrcWritef( SY_ERROR , "CreateFile(%s) failed, last error code = %d", serialPortName, ::GetLastError() ) ;
		return false ;
	}
	
	DCB				dcb = {0};
	::GetCommState( m_hComm, &dcb) ;
	dcb.BaudRate = 9600 ;
	dcb.ByteSize = 8;
	dcb.Parity = 	NOPARITY ;
	dcb.StopBits = 0 ;
	if (!::SetCommState(m_hComm, &dcb)) {
		TrcWritef( "SerialPort",SY_ERROR , "SetCommState failed, last error code = %d", ::GetLastError() ) ;
 		return false ;
	}
	
	COMMTIMEOUTS	Timeouts={0};
	Timeouts.ReadIntervalTimeout = MAXDWORD ;
	if( !::SetCommTimeouts( m_hComm , &Timeouts ) ) //读取超时
	{
		TrcWritef( "SerialPort",SY_ERROR , "SetCommTimeouts failed, last error code = %d", ::GetLastError() ) ;
		return false ;
	}
	
	if( !::SetCommMask( m_hComm , EV_RXCHAR | EV_ERR ) )
	{
		TrcWritef( "SerialPort",SY_ERROR , "SetCommMask failed, last error code = %d", ::GetLastError() ) ;
		return false ;
	}
	::PurgeComm( m_hComm , PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR ) ;

	unsigned	uThreadId = 0 ;
	m_hIOThread = (HANDLE)_beginthreadex( NULL, 0, _ReadProc , this , 0 , & uThreadId );
	if( m_hIOThread == NULL )
	{
		TrcWritef( "SerialPort",SY_ERROR , "_beginthreadex failed, last error code = %d", ::GetLastError() ) ;
		return false ;
	}
	return true ;
}

void	SerialPort::Close()
{	
	if( m_hStopEvent )
	{
		::SetEvent( m_hStopEvent ) ;
		::CloseHandle( m_hStopEvent ) ;
		m_hStopEvent = NULL ;
	}
	if( m_hReadEvent )
	{
		::CloseHandle( m_hReadEvent ) ;
		m_hReadEvent = NULL ;
	}
	if( m_hWriteEvent )
	{
		::CloseHandle( m_hWriteEvent ) ;
		m_hWriteEvent = NULL ;
	}
	if( m_hIOThread )
	{
		if( ::WaitForSingleObject( m_hIOThread, 1000 * 60 ) != WAIT_OBJECT_0 )
		{
			::TerminateThread( m_hIOThread , 0 ) ;
			TrcWritef( "SerialPort",SY_WARN , "SerialPort::Close -- TerminateThread" ) ;
		}
		::CloseHandle( m_hIOThread ) ;
		m_hIOThread = NULL ;
	}
	if( m_hComm )
	{
		::PurgeComm( m_hComm , PURGE_TXABORT|PURGE_RXABORT ) ;
		::CloseHandle( m_hComm ) ;
		m_hComm = NULL ;
	}
}


unsigned	SerialPort::Read ( void* dataBuf, unsigned dataNeedRead ,unsigned timeOut )
{
	unsigned		hasRead = 0 ;//实际读取的数据
	if( dataBuf == NULL || dataNeedRead == 0 )
	{
		TrcWritef("SerialPort", SY_WARN , "SerialPort::Read -- param invalid." ) ;
		return	hasRead ;
	}

	DWORD	dwBeginTick = ::GetTickCount() ;
	do
	{
		m_crsReadBuf.Lock() ;
		if( dataNeedRead > m_ReadBuf.size() && 
			::GetTickCount() - dwBeginTick < timeOut &&
			::WaitForSingleObject( m_hStopEvent , 0 ) == WAIT_TIMEOUT ) 
		{
			//没达到指定长度、没有超时、且整个对象没有退出
			//暂时不做任何事情，继续等待
		}
		else
		{
			hasRead = min( dataNeedRead , m_ReadBuf.size() ) ;
			if( hasRead > 0 )
			{
				memcpy( dataBuf , m_ReadBuf.data() , hasRead ) ;
				m_ReadBuf.erase( m_ReadBuf.begin() , m_ReadBuf.begin() + hasRead ) ;
				if(m_bTrace)												   
				{
					char *asciiData = Hex2Ascii((const unsigned char*) dataBuf , hasRead);
					TrcWritef( "SerialPort",DRV_OUTPUT ,"%s:%s(%s)" , m_TraceHead.c_str(),dataBuf,asciiData ) ;
					delete []asciiData;

				}
			}
			m_crsReadBuf.Unlock() ;
			break ;
		}
		m_crsReadBuf.Unlock() ;
		m_ntfReadBuf.WaitNotify(10) ;//等待
	}
	while( true ) ;

	return hasRead ;
}

unsigned	SerialPort::Write( void* data, unsigned dataNeedWrite )
{
	DWORD		hasWrite = 0 ;//实际读取的数据

	if( data == NULL || dataNeedWrite == 0 )
	{
		TrcWritef( "SerialPort",SY_WARN , "SerialPort::Write -- param invalid." ) ;
		return	hasWrite ;
	}
	ClearComm() ;															 //写串口之前清空COM
	OVERLAPPED olWrite = {0};
	ResetEvent( m_hWriteEvent ) ;
	olWrite.hEvent = m_hWriteEvent ;

	if( !::WriteFile( m_hComm , data , dataNeedWrite , &hasWrite , &olWrite ) )
	{
		DWORD	dwRet = GetLastError();
		if( dwRet == ERROR_IO_PENDING )
		{

			if( !::GetOverlappedResult( m_hComm , &olWrite , &hasWrite , TRUE ) )
			{
				TrcWritef( "SerialPort",SY_ERROR , "SerialPort::Write -- GetOverlappedResult failed , ErrCode = %d" , ::GetLastError() ) ;
				ClearComm() ;
				return FALSE ;
			}
		}
		else
		{
			TrcWritef( "SerialPort",SY_ERROR , "SerialPort::Write -- WriteFile failed , ErrCode = %d" , dwRet ) ;
			ClearComm() ;
			return FALSE ;
		}
	}
	if (m_bTrace)													   
	{
		char *asciiData = Hex2Ascii((const unsigned char*) data , hasWrite);
		TrcWritef( "SerialPort",DRV_INPUT ,"%s:%s(%s)" , m_TraceHead.c_str(),data,asciiData ) ;
		delete []asciiData;
	}


	return hasWrite;
}

void  SerialPort::ClearComm()
{
	DWORD dwErr = 0 ;
	if( !::ClearCommError( m_hComm , &dwErr , NULL ) )
	{
		TrcWritef( "SerialPort",SY_ERROR , "SerialPort::ClearComm -- api ClearCommError failed , ErrCode = %d" , ::GetLastError() ) ;
		return ;
	}
	string	strErr ;

	if( dwErr & CE_BREAK )
		strErr += " CE_BREAK |" ;

	if( dwErr & CE_DNS )
		strErr += " CE_DNS |" ;

	if( dwErr & CE_FRAME )
		strErr += " CE_FRAME |" ;

	if( dwErr & CE_IOE )
		strErr += " CE_IOE |" ;

	if( dwErr & CE_MODE )
		strErr += " CE_MODE |" ;

	if( dwErr & CE_OOP )
		strErr += " CE_OOP |" ;

	if( dwErr & CE_OVERRUN )
		strErr += " CE_OVERRUN |" ;

	if( dwErr & CE_PTO )
		strErr += " CE_PTO |" ;

	if( dwErr & CE_RXOVER )
		strErr += " CE_RXOVER |" ;

	if( dwErr & CE_RXPARITY )
		strErr += " CE_RXPARITY |" ;

	if( dwErr & CE_TXFULL )
		strErr += " CE_TXFULL |" ;

	if( !strErr.empty() )
	{
		strErr.erase( strErr.end() ) ;
		TrcWritef( SY_ERROR , "::ClearCommError -- Result = %s " , strErr.c_str() ) ;
	}
}


unsigned __stdcall	SerialPort::_ReadProc( void *	pParam ) 
{
	try
	{
		SerialPort	*pSerialPort = ( SerialPort * ) pParam ;
		pSerialPort->ReadImpl() ;
	}
	catch( ... )
	{
		TrcWritef( "SerialPort",SY_FATAL , "SerialPort::_ReadProc -- Unknown exception." ) ;
	}
	_endthreadex( 0 ) ;
	return 0 ;
}
void	SerialPort::ReadToDeque()
{
	while( true )
	{
		const DWORD dwBufLen = 512 ;
		char	buf[ dwBufLen ] = {0} ;
		DWORD	dwRead = 0 ;
		OVERLAPPED	olRead = {0} ;
		if( ::ReadFile( m_hComm , buf , dwBufLen ,&dwRead , &olRead ) )
		{
			if( dwRead != 0 )
			{
				USingleLock	locker( &m_crsReadBuf ) ;
				m_ReadBuf += string(buf,dwRead);
				m_ntfReadBuf.Notify() ;//通知buff有更新
			}
			if( dwRead != 0 )
			{
				char *asciiData = Hex2Ascii((const unsigned char*) buf , dwRead);
				TrcWritef( "SerialPort",DRV_RAW ,"%s:%s(%s)" , m_TraceHead.c_str(),buf,asciiData ) ;
				delete []asciiData;
			}
			if( dwRead != dwBufLen )
				break;
		}
		else
		{
			///之前的串口超时设置，已经保证异步io能马上返回实际的结果,而不会有io_pending的返回值
			TrcWritef( "SerialPort",SY_ERROR , "SerialPort::ReadToDeque -- ReadFile failed , ErrCode = %d" , ::GetLastError() ) ;
			ClearComm() ;
			break;
		}
	}
}

void	SerialPort::ReadImpl()
{
	HANDLE		hEvents[2] = {0};
	OVERLAPPED	ol =	 {0};
	ol.hEvent = m_hReadEvent ;

	hEvents[0] = m_hStopEvent ;
	hEvents[1] = m_hReadEvent ;

	DWORD	dwEvtMask = 0 ;
	DWORD	dwRet = 0 ;

	while (TRUE)  
	{
		::ResetEvent( m_hStopEvent ) ;
		::ResetEvent( m_hReadEvent ) ;

		if( !WaitCommEvent( m_hComm , &dwEvtMask , &ol ) )
		{
			dwRet = ::GetLastError() ;
			if( ERROR_IO_PENDING == dwRet )
			{
				dwRet =	::WaitForMultipleObjects( 2, hEvents, FALSE, INFINITE );
				if( dwRet == WAIT_OBJECT_0 )
				{
					TrcWritef( "SerialPort",SY_INFO , "SerialPort::ReadImpl -- StopEvent has signal,process will quit" ) ;
					break;
				}
				else if( dwRet != WAIT_OBJECT_0 + 1 )
				{
					///此种情况做任何处理都不合适。但它出现的概率应该是极低，故可忽略。
					TrcWritef( "SerialPort",SY_ERROR , "SerialPort::ReadImpl -- WaitForMultipleObjects failed , ErrCode = %d" ,::GetLastError() ) ;
				}
				if( !GetOverlappedResult( m_hComm , &ol , &dwRet , FALSE ) )
				{
					TrcWritef( "SerialPort",SY_ERROR , "SerialPort::ReadImpl -- GetOverlappedResult failed , ErrCode = %d" ,::GetLastError() ) ;
					ClearComm() ;
					Sleep(10) ;
					continue ;
				}
			}
			else
			{
				TrcWritef( "SerialPort",SY_ERROR , "SerialPort::ReadImpl -- WaitCommEvent failed , ErrCode = %d" ,dwRet ) ;
				ClearComm() ;
				Sleep(10) ;
				continue ;
			}
		}
		if( dwEvtMask & EV_RXCHAR )
		{
			ReadToDeque() ;
		}
		if( dwEvtMask & EV_ERR )
		{
			TrcWritef( "SerialPort",SY_ERROR , "SerialPort::ReadImpl -- Comm Event EV_ERR" ) ;
			ClearComm() ;
			Sleep(10) ;
		}
	}
}

void  SerialPort::Clear()
{
	if( ! PurgeComm( m_hComm , PURGE_TXCLEAR | PURGE_TXABORT ) )
	{
		TrcWritef( "SerialPort",SY_ERROR , "SerialPort::Clear -- PurgeComm failed , ErrCode = %d" ,::GetLastError() ) ;
	}
	USingleLock	locker( &m_crsReadBuf ) ;
	if ( !m_ReadBuf.empty() )
	{
		if(m_bTrace)												   
		{
			char *asciiData = Hex2Ascii((const unsigned char*) m_ReadBuf.data() , m_ReadBuf.size());
			TrcWritef( "SerialPort",DRV_DISCARD ,"%s:%s(%s)" , m_TraceHead.c_str(),m_ReadBuf.c_str(),asciiData ) ;
			delete []asciiData;
		}
		m_ReadBuf.clear() ;
	}	
}


