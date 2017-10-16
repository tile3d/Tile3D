#pragma once

#include <Math/TMatrix4.h>
#include <Common/TTypes.h>

class TStream;
class TDevice
{
public:
	virtual bool Init() { return true; }
	virtual void Close() {}

	virtual void Clear() {}
	virtual void BeginRender(){}
	virtual void EndRender() {}
	virtual void Present() {}

	virtual TStream* CreateStream() { return nullptr; }

	virtual void SetTransform(TRANSFORMATION_STATE state, const TMatrix4 & mat) {}


	virtual bool OnLostDevice() { return true; }
	virtual bool OnResetDevice() { return true; }
	virtual bool IsLostDevice() { return false; }

};
