

#include "LAnimator.h"

LAnimator::LAnimator()
:Skeleton(nullptr)
{

}

LAnimator::~LAnimator()
{
	if(Skeleton)
	{
		delete Skeleton;
		Skeleton = nullptr;
	}
}