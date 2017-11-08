#include <File/TPackageMan.h>
#include <File/TPackage.h>
#include "File/TFileDir.h"
#include <Common/TLog.h>
#include <Common/TTime.h>
#include <Core/TMemory.h>
#include <stdio.h>

int TestPackage()
{
	if (!TLog::Init("game.log", LOG_DEBUG, false)) {
		printf("init log file failed\n");
	}


	if (!TFileDir::GetInstance()->Init("E:\\Program Files (x86)\\÷Ôœ…3°§“ªƒÓ«¨¿§\\element", "", "", "")) {
		printf("fail to init the directory\n");
	}

/*
	if (!TFileDir::GetInstance()->Init("F:\\PGP\\÷Ôœ…3\\element", "", "", "")) {
		printf("fail to init the directory\n");
	}
*/


	char *pck_list[] = { "building.pck", "configs.pck", "facedata.pck", "gfx.pck", "grasses.pck", "models.pck",
		"interfaces.pck", "litmodels.pck", "sfx.pck", "shaders.pck", "surfaces.pck", "textures.pck" };

	TTime tt;
	tt.Start();
	int64 totalSize = 0;
	for (int i = 0; i < sizeof(pck_list) / sizeof(char*); ++i) {
		TPackage * pPackage = TPackageMan::GetInstance()->OpenFilePackage(pck_list[i], 0);
		if (pPackage == nullptr) {
			printf("open failed, package_file=%s\n", pck_list[i]);
			continue;
		}

		int64 packageSize = pPackage->GetPackageFileSize();
		totalSize += packageSize;
		printf("open successfully, package_file=%s, package_size=%lld\n", pck_list[i], packageSize);
	}
	tt.Stop();
	printf("Total cost time=%d\n", tt.GetCostTime());

	int alloc_size = TMemory::GetAllocSize();
	int raw_size = TMemory::GetAllocRawSize();
	printf("Alloc size=%d, raw size=%d\n", alloc_size, raw_size);

	TMemory::ExportMemoryInfo("memory.log");
	return 0;
}

