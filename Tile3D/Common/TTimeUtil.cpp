#include "TTimeUtil.h"

#if PLATFORM_WIN
	#include <Windows.h>
#elif PLATFORM_IOS
#include	
#elif PLATFORM_ANDROID
#include <unistd.h>
#endif

void TTimeUtil::Sleep(int time)
{
#if PLATFORM_WIN
	::Sleep(time);
#elif PLATFORM_IOS

#elif PLATFORM_ANDROID

#endif
}