#pragma once

#define  SY_WARN							("Warning") 
#define  SY_ERROR							("Error")
#define  SY_FATAL							("Fatal") 

#define  SY_INFO							("Info") 


#define			DRV_DISCARD			"DRV_DISCARD"


#define			DRV_INPUT			"DRV_WRITE"


#define			DRV_RAW				"DRV_RAW"


#define			DRV_OUTPUT			"DRV_READ"


char*	Hex2Ascii( const unsigned char* data, int dataLen ) ;
void	TrcWritef(const char* module , const char* level, const char* format, ... );

