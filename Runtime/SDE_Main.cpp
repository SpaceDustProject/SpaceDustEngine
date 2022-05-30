#include "SDE_Settings.h"
#include "SDE_VirtualMachine.h"
#include "SDE_Blackboard.h"
#include "SDE_Director.h"
#include "SDE_FileIO.h"

int main(int argc, char* argv[])
{
	SDE_Blackboard::Instance().SetValue("SDE_FRAME_RATE", 60.0f);

	SDE_VirtualMachine::Instance();

	if (!SDE_Director::Instance().Run())
	{
		return false;
	}

	return 0;
}