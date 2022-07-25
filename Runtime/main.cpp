#include "SDE_Director.h"

#include "SDE_Data.h"
#include "SDE_FileIO.h"
#include "SDE_Debug.h"
#include "SDE_Blackboard.h"

#include <iostream>

int main(int argc, char* argv[])
{
	SDE_Data dataScript = SDE_FileIO::Instance().Input("test.lua", false);
	SDE_Director::Instance().RunScript(dataScript);

	SDE_Director::Instance().GetBlackboard()->SetValue("SDE_FRAME_RATE", 60.0f);
	SDE_Director::Instance().Run();

	std::cin.get();

	return 0;
}