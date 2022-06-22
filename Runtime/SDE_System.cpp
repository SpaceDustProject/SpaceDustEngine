#include "SDE_System.h"
#include "SDE_Director.h"
#include "SDE_VirtualMachine.h"

#include <functional>

const std::string& SDE_System::GetName()
{
	return m_strName;
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

SDE_System::SDE_System(SDE_Scene* pScene, const SDE_System::Def& defSystem)
{
	m_pScene = pScene;

	lua_State* pState = pScene->GetDirector()->GetLuaVM();

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, 1, "Runtime");
	lua_getfield(pState, -1, "System");

	// 将该系统的 Init 函数推入 Runtime
	lua_getfield(pState, -1, "OnInit");

	lua_getfield(pState, 1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, m_strName.c_str());
	lua_getfield(pState, -1, "OnInit");

	m_nRefInit = luaL_ref(pState, -5);
	lua_pop(pState, 4);

	// 将该系统的 Update 函数推入 Runtime
	lua_getfield(pState, -1, "OnUpdate");

	lua_getfield(pState, 1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, m_strName.c_str());
	lua_getfield(pState, -1, "OnUpdate");

	m_nRefUpdate = luaL_ref(pState, -5);
	lua_pop(pState, 4);

	// 将该系统的 Quit 函数推入 Runtime
	lua_getfield(pState, -1, "OnQuit");

	lua_getfield(pState, 1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, m_strName.c_str());
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
	
}

luaL_Reg g_funcSystemMember[] =
{
	

	{ nullptr, nullptr }
};

luaL_Reg g_funcSystemDefMember[] =
{
	{ nullptr, nullptr }
};