#include "SDE_VirtualMachine.h"

static lua_State* s_pMainVM = nullptr;

lua_State* SDE_VirtualMachine::GetMainVM()
{
	return s_pMainVM;
}

SDE_VirtualMachine::SDE_VirtualMachine()
{
	s_pMainVM = luaL_newstate();
	luaL_openlibs(s_pMainVM);
	lua_gc(s_pMainVM, LUA_GCINC, 100);
}

SDE_VirtualMachine::~SDE_VirtualMachine()
{
	lua_close(s_pMainVM);
}

SDE_VirtualMachine& instance = SDE_VirtualMachine::Instance();