#include "TLog.h"
#include <stdarg.h>
#include <time.h>

#pragma warning (disable: 4996)

FILE * TLog::m_logFile = nullptr;
LOG_LEVEL TLog::m_logLevel = LOG_NONE;

bool TLog::Init(const char * logFile, LOG_LEVEL logLevel, bool append)
{
	if (append) {
		m_logFile = fopen(logFile, "at");
	}
	else {
		m_logFile = fopen(logFile, "wt");
	}

	if (m_logFile == nullptr) {
		return false;
	}

	m_logLevel = logLevel;
	return true;
}

void TLog::Release()
{
	if (m_logFile != nullptr) {
		fclose(m_logFile);
	}
}


const char * TLog::GetLogLevelStr(LOG_LEVEL logLevel)
{
	switch (logLevel) {
		case LOG_NONE:
			return "NORMAL";
		case LOG_INFO:
			return "INFO";
		case LOG_WARNING:
			return "WARNING";
		case LOG_ERR:
			return "ERROR";
		case LOG_FATAL:
			return "FATAL";
		default:
			return "NORMAL";
	}
}

//TBD: 
// 1) move the time function to the TTime
// 2) how to reduce the time function call    
void TLog::Log(LOG_LEVEL logLevel, const char* component, const char * fmt, ...)
{
	if (logLevel < m_logLevel) return;
	if (m_logFile == nullptr) return;

	//construct the log message
	char logMsg[2048];
	va_list args_list;
	va_start(args_list, fmt);
	vsnprintf(logMsg, sizeof(logMsg), fmt, args_list);
	va_end(args_list);

	//construct the log header
	time_t t = ::time(NULL);
	tm* ptm = ::localtime(&t);

	fprintf(m_logFile, "[%02d:%02d:%02d][%s][%s] %s\n", ptm->tm_hour, ptm->tm_min, ptm->tm_sec, component, GetLogLevelStr(logLevel), logMsg);
	fflush(m_logFile);
}


