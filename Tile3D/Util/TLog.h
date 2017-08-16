#pragma once

#include <stdio.h>

enum LOG_LEVEL
{
	LOG_NONE,			//not used
	LOG_DEBUG,			//debug info for development
	LOG_INFO,			//log info (Log user information, for server end normally use this)
	LOG_WARNING,		//warning info
	LOG_ERR,			//log error info
	LOG_FATAL,			//log fatal message
};

//TBD
//1) need add lock
//2) support remote log(send over network)
//3) support debug window
//4) support multiple log files?
//5) need add log directory
class TLog
{
public:
	//init
	static bool Init(const char * logFile, LOG_LEVEL logLevel, bool append);
	//release the log file
	static void Release();

	static void SetLogLevel(LOG_LEVEL logLevel) {
		m_logLevel = logLevel;
	}

	static const char * GetLogLevelStr(LOG_LEVEL logLevel);

	static void Log(LOG_LEVEL log_level, const char* component, const char * fmt, ...);
	
private:
	static FILE * m_logFile;
	static LOG_LEVEL m_logLevel;
};

