#include "SDE_System.h"
#include "SDE_Director.h"
#include "SDE_VirtualMachine.h"
#include "SDE_Debug.h"

#include <functional>
#include <iostream>

const std::string& SDE_System::GetType()
{
	return m_strType;
}

SDE_Scene* SDE_System::GetScene()
{
	return m_pScene;
}

int SDE_System::GetInitRef()
{
	return m_nRefInit;
}

int SDE_System::GetUpdateRef()
{
	return m_nRefUpdate;
}

int SDE_System::GetQuitRef()
{
	return m_nRefQuit;
}

SDE_System::SDE_System(SDE_Scene* pScene, Def* defSystem)
{
	m_pScene = pScene;
	m_strType = defSystem->strType;

	std::cout << defSystem->strType << std::endl;

	lua_State* pState = pScene->GetDirector()->GetLuaVM();
	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, 1, "Runtime");
	lua_getfield(pState, -1, "System");

	// 将该系统的 Init 函数推入 Runtime
	lua_getfield(pState, -1, "OnInit");

	lua_getfield(pState, 1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, m_strType.c_str());
	lua_getfield(pState, -1, "OnInit");

	m_nRefInit = luaL_ref(pState, -5);
	lua_pop(pState, 4);

	// 将该系统的 Update 函数推入 Runtime
	lua_getfield(pState, -1, "OnUpdate");

	lua_getfield(pState, 1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, m_strType.c_str());
	lua_getfield(pState, -1, "OnUpdate");

	m_nRefUpdate = luaL_ref(pState, -5);
	lua_pop(pState, 4);

	// 将该系统的 Quit 函数推入 Runtime
	lua_getfield(pState, -1, "OnQuit");

	lua_getfield(pState, 1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, m_strType.c_str());
	lua_getfield(pState, -1, "OnQuit");

	m_nRefQuit = luaL_ref(pState, -5);
	lua_pop(pState, 4);

	// 完成堆栈清理工作
	lua_pop(pState, 3);
}

SDE_System::~SDE_System()
{
	lua_State* pState = m_pScene->GetDirector()->GetLuaVM();

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Runtime");
	lua_getfield(pState, -1, "System");

	// 将 Init 函数移出 Runtime
	lua_getfield(pState, -1, "OnInit");
	luaL_unref(pState, -1, m_nRefInit);
	lua_pop(pState, 1);

	// 将 Update 函数移出 Runtime
	lua_getfield(pState, -1, "OnUpdate");
	luaL_unref(pState, -1, m_nRefUpdate);
	lua_pop(pState, 1);

	// 将 Quit 函数移出 Runtime
	lua_getfield(pState, -1, "OnInit");
	luaL_unref(pState, -1, m_nRefQuit);
	lua_pop(pState, 1);
}

luaL_Reg g_funcSystemMember[] =
{
	
	{ nullptr, nullptr }
};

SDE_API SDE_SystemDef_GetType(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);
	lua_pushstring(pState, pDefSystem->strType.c_str());
	return 1;
}

SDE_API SDE_SystemDef_GetInitFunc(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, pDefSystem->strType.c_str());
	lua_getfield(pState, -1, "OnInit");

	return 1;
}

SDE_API SDE_SystemDef_GetUpdateFunc(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, pDefSystem->strType.c_str());
	lua_getfield(pState, -1, "OnUpdate");

	return 1;
}

SDE_API SDE_SystemDef_GetQuitFunc(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, pDefSystem->strType.c_str());

	lua_getfield(pState, -1, "OnQuit");
	return 1;
}

SDE_API SDE_SystemDef_SetInitFunc(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");

	lua_getfield(pState, -1, pDefSystem->strType.c_str());
	lua_pushvalue(pState, 2);
	lua_setfield(pState, -2, "OnInit");

	return 0;
}

SDE_API SDE_SystemDef_SetUpdateFunc(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");

	lua_getfield(pState, -1, pDefSystem->strType.c_str());
	lua_pushvalue(pState, 2);
	lua_setfield(pState, -2, "OnUpdate");

	return 0;
}

SDE_API SDE_SystemDef_SetQuitFunc(lua_State* pState)
{
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 1, SDE_SYSTEMDEF_NAME);

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");

	lua_getfield(pState, -1, pDefSystem->strType.c_str());
	lua_pushvalue(pState, 2);
	lua_setfield(pState, -2, "OnQuit");

	return 0;
}

luaL_Reg g_funcSystemDefMember[] =
{
	{ "GetType",		SDE_SystemDef_GetType},

	{ "GetInitFunc",	SDE_SystemDef_GetInitFunc},
	{ "GetUpdateFunc",	SDE_SystemDef_GetUpdateFunc},
	{ "GetQuitFunc",	SDE_SystemDef_GetQuitFunc},

	{ "SetInitFunc",	SDE_SystemDef_SetInitFunc},
	{ "SetUpdateFunc",	SDE_SystemDef_SetUpdateFunc},
	{ "SetQuitFunc",	SDE_SystemDef_SetQuitFunc},

	{ nullptr,			nullptr }
};