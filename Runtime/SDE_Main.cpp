#include "SDE_Settings.h"
#include "SDE_VirtualMachine.h"
#include "SDE_Blackboard.h"
#include "SDE_Director.h"
#include "SDE_FileIO.h"

#include <iostream>

int main(int argc, char* argv[])
{
	SDE_Blackboard::Instance().SetValue("SDE_FRAME_RATE", 60.0f);

	SDE_Data dataScript = SDE_FileIO::Instance().Input("test.lua");
	SDE_VirtualMachine::Instance().RunScript(g_director.GetLuaVM(), dataScript);
	g_director.Run();

	return 0;
}