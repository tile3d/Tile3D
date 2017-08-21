#include <File/TPackageMan.h>
#include "File/TFileDir.h"
#include <Util/TLog.h>
#include <stdio.h>

int main()
{
	if (!TLog::Init("game.log", LOG_DEBUG, false)) {
		printf("init log file failed\n");
	}

	/*
	if (!TFileDir::GetInstance()->Init("D:\\engine\\Tile3D\\TestMisc\\TestMisc", "", "", "")) {

	}
	*/

	if (!TFileDir::GetInstance()->Init("E:\\Program Files (x86)\\ÖïÏÉ3¡¤Ò»ÄîÇ¬À¤\\element", "", "", "")) {

	}


	if (!TPackageMan::GetInstance()->OpenFilePackage("gfx.pck", 0)) {
		printf("open failed\n");
	}
	return 0;
}
