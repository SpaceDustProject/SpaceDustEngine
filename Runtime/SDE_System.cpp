#include "SDE_System.h"

#include <functional>

SDE_System::SDE_System()
{

}

SDE_System::~SDE_System()
{

}

static const luaL_Reg s_regMember[] =
{
	{nullptr, nullptr}
};

static const lua_CFunction s_funcGC = [](lua_State* pState)->int
{
	return 0;
};

const luaL_Reg* SDE_System::GetMemberFunc()
{
	return s_regMember;
}

lua_CFunction SDE_System::GetGCFunc()
{
	return s_funcGC;
}