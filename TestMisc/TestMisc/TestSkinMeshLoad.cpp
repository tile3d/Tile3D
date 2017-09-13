#include <Render/SkinModel/TSkinModelMan.h>
#include <stdio.h>

bool LoadSkinModel(const char* fileName)
{
	if (TSkinModelMan::GetInstance()->LoadSkinModel(fileName, 0) == nullptr) {
		return false;
	}
	return true;
}

int main()
{
	const char* filename = ".\\2016�񱩺���\\2016�񱩺���.smd";
	if (!LoadSkinModel(filename)) {
		printf("failed to load the skin model");
		return -1;
	}

	return 0;
}