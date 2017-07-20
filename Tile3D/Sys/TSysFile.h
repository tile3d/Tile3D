#pragma once

#if PLATFORM_WIN
	#include "Win/TSysFileWin.h"
#elif PLATFORM_IOS
	#include "IOS/TSysFileIOS.h"
#elif PLATFORM_ANDROID
	#include "Android/TSysFileAndroid.h"
#endif

