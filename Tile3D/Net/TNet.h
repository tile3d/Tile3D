#pragma once

class TNet
{
public:
	TNet* GetInstance() {
		static TNet tn;
		return &tn;
	}

	bool Init();
	bool Cleanup();

private:
	
};

