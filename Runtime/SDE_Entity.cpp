#include "SDE_Entity.h"

#include <functional>

SDE_Entity::SDE_Entity()
{

}

SDE_Entity::~SDE_Entity()
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

const luaL_Reg* SDE_Entity::GetMemberFunc()
{
	return s_regMember;
}

lua_CFunction SDE_Entity::GetGCFunc()
{
	return s_funcGC;
}