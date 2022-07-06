#include "SDE_Director.h"
#include "SDE_FileIO.h"
#include "SDE_Data.h"
#include "SDE_LuaUtility.h"

#include "SDE_Debug.h"

#include <iostream>

int main(int argc, char* argv[])
{
	SDE_Data dataScript = SDE_FileIO::Instance().Input("test.lua", false);
	SDE_Director::Instance().RunScript(dataScript);

	return 0;
}