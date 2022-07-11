#include "SDE_System.h"

#include "SDE_LuaUtility.h"
#include "SDE_Debug.h"

#include "SDE_Scene.h"

SDE_Scene* SDE_System::GetScene()
{
	return m_pScene;
}

const std::string& SDE_System::GetName()
{
	return m_strType;
}

long long SDE_System::GetInitRef()
{
	return m_nRefInit;
}

long long SDE_System::GetUpdateRef()
{
	return m_nRefUpdate;
}

long long SDE_System::GetQuitRef()
{
	return m_nRefQuit;
}

void SDE_System::SetInitRef(long long nRef)
{
	m_nRefInit = nRef;
}

void SDE_System::SetUpdateRef(long long nRef)
{
	m_nRefUpdate = nRef;
}

void SDE_System::SetQuitRef(long long nRef)
{
	m_nRefQuit = nRef;
}

SDE_System::SDE_System(SDE_Scene* pScene, const SDE_SystemDef& defSystem)
	: SDE_LuaLightUserdata(SDE_TYPE_SYSTEM)
{
	m_pScene = pScene;
	m_strType = defSystem.strName;

	m_nRefInit = LUA_NOREF;
	m_nRefUpdate = LUA_NOREF;
	m_nRefQuit = LUA_NOREF;
}

SDE_System::~SDE_System()
{

}

SDE_LUA_FUNC(SDE_SystemDef_GetName)
{
	SDE_SystemDef* pDefSystem = (SDE_SystemDef*)luaL_checkudata(pState, 1, SDE_TYPE_SYSTEMDEF);
	lua_pushstring(pState, pDefSystem->strName.c_str());
	return 1;
}

SDE_LUA_FUNC(SDE_SystemDef_SetConstructor)
{
	SDE_SystemDef* pDefSystem = (SDE_SystemDef*)luaL_checkudata(pState, 1, SDE_TYPE_SYSTEMDEF);

	if (pDefSystem->bIsRegistered)
	{
		SDE_LuaUtility::GetRegistry(pState);
		lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefSystem->strName.c_str());
		lua_rawget(pState, -2);
		lua_pushstring(pState, "constructor");
		lua_pushvalue(pState, 2);
		lua_rawset(pState, -3);
	}
	else
	{
		SDE_LuaUtility::GetTemporary(pState);
		lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefSystem->strName.c_str());
		lua_pushvalue(pState, 2);
		lua_rawset(pState, -3);
	}
	return 0;
}

SDE_LuaMetatable g_metatableSystemDef =
{
	SDE_TYPE_SYSTEMDEF,
	{
		{ "GetName",		SDE_SystemDef_GetName },
		{ "SetConstructor", SDE_SystemDef_SetConstructor }
	},

	[](lua_State* pState)->int
	{
		SDE_SystemDef* pDefSystem = (SDE_SystemDef*)luaL_checkudata(pState, 1, SDE_TYPE_SYSTEMDEF);
		pDefSystem->~SDE_SystemDef();
		return 0;
	}
};

SDE_LUA_FUNC(SDE_System_GetName)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	lua_pushstring(pState, pSystem->GetName().c_str());
	return 1;
}

SDE_LUA_FUNC(SDE_System_GetInitFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	std::string strType = pSystem->GetName();

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushstring(pState, SDE_NAME_SYSTEM_ONINIT);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pSystem->GetInitRef());

	return 1;
}

SDE_LUA_FUNC(SDE_System_GetUpdateFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	std::string strType = pSystem->GetName();

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushstring(pState, SDE_NAME_SYSTEM_ONUPDATE);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pSystem->GetUpdateRef());

	return 1;
}

SDE_LUA_FUNC(SDE_System_GetQuitFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushstring(pState, SDE_NAME_SYSTEM_ONQUIT);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pSystem->GetQuitRef());

	return 1;
}

SDE_LUA_FUNC(SDE_System_SetInitFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushstring(pState, SDE_NAME_SYSTEM_ONINIT);
	lua_rawget(pState, -2);
	lua_pushvalue(pState, 2);
	lua_rawseti(pState, -2, pSystem->GetInitRef());

	return 0;
}

SDE_LUA_FUNC(SDE_System_SetUpdateFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushstring(pState, SDE_NAME_SYSTEM_ONUPDATE);
	lua_rawget(pState, -2);
	lua_pushvalue(pState, 2);
	lua_rawseti(pState, -2, pSystem->GetUpdateRef());

	return 0;
}

SDE_LUA_FUNC(SDE_System_SetQuitFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushstring(pState, SDE_NAME_SYSTEM_ONQUIT);
	lua_rawget(pState, -2);
	lua_pushvalue(pState, 2);
	lua_rawseti(pState, -2, pSystem->GetQuitRef());

	return 0;
}

SDE_LuaPackage g_packageSystem =
{
	{ "GetSystemName",	SDE_System_GetName },

	{ "GetInitFunc",	SDE_System_GetInitFunc },
	{ "GetUpdateFunc",	SDE_System_GetUpdateFunc },
	{ "GetQuitFunc",	SDE_System_GetQuitFunc },

	{ "SetInitFunc",	SDE_System_SetInitFunc },
	{ "SetUpdateFunc",	SDE_System_SetUpdateFunc },
	{ "SetQuitFunc",	SDE_System_SetQuitFunc }
};