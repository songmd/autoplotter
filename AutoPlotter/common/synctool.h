#pragma once


class	USyncTool
{
public:
	virtual	void Lock()=0;
	virtual	void Unlock()=0;
};
////////////////////////////////////////////////////////////////////////////////////////////
//							UCriticalSection
///////////////////////////////////////////////////////////////////////////////////////////
class	UCriticalSection:public USyncTool
{
public:
	UCriticalSection() ;

	~UCriticalSection() ;

	void	Lock() ;

	void	Unlock() ;

private:
	CRITICAL_SECTION	m_sect ; /// recursion supported; /// recursion supported
};

////////////////////////////////////////////////////////////////////////////////////////////
//							USecurityAttributes
///////////////////////////////////////////////////////////////////////////////////////////
class	USecurityAttributes
{
public:
	USecurityAttributes() ;
	~USecurityAttributes() ;

	operator LPSECURITY_ATTRIBUTES () ;

private:
	SECURITY_ATTRIBUTES SecurityAttributes;
	BYTE                SecurityDescriptorBase[sizeof(SECURITY_DESCRIPTOR)+7];
	bool				m_bInit ;
	// use Byte-Array to store security descriptor because "SECURITY_DESCRIPTOR SecurityDescriptor"
	// doesn't work if this structure isn't allocated at 8 byte boundaries
	// usage of SecurityAttributes fails with error (998)
};

////////////////////////////////////////////////////////////////////////////////////////////
//							UMutex
///////////////////////////////////////////////////////////////////////////////////////////
class UMutex :public USyncTool
{ 
public: 
	UMutex( const char* name );
	~UMutex();
		void	Lock() ;

	void	Unlock() ;
private: 
	HANDLE m_mutex; 
};

////////////////////////////////////////////////////////////////////////////////////////////
//							USingleLock
///////////////////////////////////////////////////////////////////////////////////////////
class	USingleLock
{
private:
	USingleLock() ;
public:
	explicit USingleLock( USyncTool	 *pSect) ;
	~USingleLock() ;
		void	Lock() ;

	void	Unlock() ;
private:
	USyncTool		*m_pSect ;
};

////////////////////////////////////////////////////////////////////////////////////////////
//							UNotifier
///////////////////////////////////////////////////////////////////////////////////////////
class	UNotifier
{
public:
	UNotifier(const char*name) ;
	~UNotifier() ;
	void	Notify() ;
	bool	WaitNotify( unsigned	milliseconds ) ;
private:
	HANDLE	m_hNotifier;
};