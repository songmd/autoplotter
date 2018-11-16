#include "StdAfx.h"
#include "itrace.h"
#include <fstream>

using namespace std;

#define			TRC_BUF_LEN					1024*4 


char*	Hex2Ascii( const unsigned char* data, int dataLen ) 
{
	char *ret = 0;
	const char *hexString = "0123456789ABCDEF" ;
	if( data != NULL && dataLen >= 0 )
	{
		ret = new char[dataLen*2+1] ;
		ret[dataLen*2] = 0;
		for( int i=0; i<dataLen  ; i++ )
		{
			ret[2*i] = hexString[ (data[i] >> 4) & 0x0F ]  ;
			ret[2*i+1] =  hexString[ data[i] & 0x0F ] ;
		}
	}
	return ret;
}

void	TrcWritef( const char* module , const char* level, const char* format, ... )
{
	char path_buffer[_MAX_PATH] = {0};
	char drive[_MAX_DRIVE] = {0};
	char dir[_MAX_DIR] = {0};
	char fname[_MAX_FNAME] = {0};
	char ext[_MAX_EXT] = {0};

	char	szLogFilePath[_MAX_PATH] = {0} ;

	char	szDate[16] = {0} ;
	char	szTime[16] = {0} ;

	char	szContent[TRC_BUF_LEN] = {0};
	char	szBuf[TRC_BUF_LEN+256] = {0} ;
	SYSTEMTIME	time = {0} ;
	::GetLocalTime(&time) ;
	_snprintf_s( szDate , sizeof(szDate) , _TRUNCATE , "%04d-%02d-%02d" , time.wYear , time.wMonth ,time.wDay ) ;
	_snprintf_s( szTime , sizeof(szTime) , _TRUNCATE , "%02d:%02d:%02d.%03d" , time.wHour , time.wMinute , time.wSecond , time.wMilliseconds ) ; 

	///获取当前进程路径
	::GetModuleFileNameA(NULL,path_buffer,_MAX_PATH); 
	_splitpath_s(path_buffer,drive,dir,fname,ext);	
	_snprintf_s( szLogFilePath , sizeof(szLogFilePath) , _TRUNCATE , "%s%slogs\\%s.txt" , drive , dir ,szDate ) ;

	va_list argList;
	va_start(argList, format);
	char content[TRC_BUF_LEN] = {0};
	_vsnprintf_s ( szContent, TRC_BUF_LEN,  _TRUNCATE , format ,argList ) ;
	va_end(argList);

	_snprintf_s( szBuf , sizeof(szBuf) , _TRUNCATE , "%.10s  %.8s    %s     %08d     %s" , module, level, szTime, ::GetCurrentThreadId(),szContent ) ; 
	ofstream log(szLogFilePath , ios_base::out | ios_base::app ) ;
	log << szBuf << endl;
}
