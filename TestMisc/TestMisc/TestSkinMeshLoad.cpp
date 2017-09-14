#include <Render/SkinModel/TSkinModelMan.h>
#include <Render/SkinModel/TSkinModel.h>
#include <stdio.h>



int main()
{
	const char* filename = "D:\\engine\\Tile3D\\TestMisc\\TestMisc\\SkinModel\\2016¿ñ±©º£µÁ\\2016¿ñ±©º£µÁ.smd";
	TSkinModel * pSkinModel = TSkinModelMan::GetInstance()->LoadSkinModel(filename, 0);
	if (pSkinModel == nullptr) {
		printf("failed to load the skin model");
		return -1;
	}
	TSkinModelMan::GetInstance()->ReleaseSkinModel(pSkinModel);
	return 0;
}
