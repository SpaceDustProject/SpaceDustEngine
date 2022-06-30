#include "SDE_Settings.h"
#include "SDE_VirtualMachine.h"
#include "SDE_Blackboard.h"
#include "SDE_Director.h"
#include "SDE_FileIO.h"
#include "SDE_Debug.h"

#include <stdio.h>
#include <iostream>

int main(int argc, char* argv[])
{
	SDE_Blackboard::Instance().SetValue("SDE_FRAME_RATE", 60.0f);

	try
	{
		if (luaL_dofile(g_director.GetLuaVM(), "test.lua"))
			SDE_Debug::Instance().OutputLog("%s\n",
				lua_tostring(g_director.GetLuaVM(), -1)
			);
	}
	catch (const std::exception& err)
	{
		SDE_Debug::Instance().OutputLog("%s\n",
			std::string("Underlying code exception: ").append(err.what()).c_str()
		);
	}
	g_director.Run();

	std::cin.get();
	return 0;
}