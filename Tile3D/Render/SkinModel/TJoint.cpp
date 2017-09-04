#include "TJoint.h"
#include "TAnimJoint.h"
#include "TSprintJoint.h"
#include "TAxisSprintJoint.h"

TJoint::TJoint() : TObject(CLASSID_JOINT)
{

}


TJoint::~TJoint()
{

}

TJoint* CreateJointByClassID(int classID)
{
	TJoint * pJoint = nullptr;

	switch (classID) {
		case 
	}
}

TJoint* CreateJointByType(int type)
{

}
