#include "SDE_System.h"

#include "SDE_Director.h"
#include "SDE_LuaUtility.h"
#include "SDE_MemoryPool.h"
#include "SDE_Debug.h"

#include "SDE_Scene.h"
#include "SDE_Component.h"

SDE_Scene* SDE_System::GetScene()
{
	return m_pScene;
}

long long SDE_System::GetStartRef()
{
	return m_nRefStart;
}

long long SDE_System::GetUpdateRef()
{
	return m_nRefUpdate;
}

long long SDE_System::GetStopRef()
{
	return m_nRefStop;
}

SDE_System* SDE_System::GetNextSystem()
{
	return m_pSystemNext;
}

void SDE_System::SetStartRef(long long nRef)
{
	m_nRefStart = nRef;
}

void SDE_System::SetUpdateRef(long long nRef)
{
	m_nRefUpdate = nRef;
}

void SDE_System::SetStopRef(long long nRef)
{
	m_nRefStop = nRef;
}

void SDE_System::SetNextSystem(SDE_System* pSystem)
{
	if (m_pSystemNext)
		return;

	// 被设置的系统拥有前置依赖后，便不再属于独立系统
	m_pSystemNext = pSystem;
	m_pScene->RemoveSystemFromList(pSystem);
}

bool SDE_System::IsRunning()
{
	return m_bIsRunning;
}

SDE_System::SDE_System(SDE_Scene* pScene, const SDE_SystemDef& defSystem)
	: SDE_LuaLightUserdata(SDE_TYPE_SYSTEM)
{
	m_strName = defSystem.strName;
	m_pScene = pScene;

	m_nRefStart = LUA_NOREF;
	m_nRefUpdate = LUA_NOREF;
	m_nRefStop = LUA_NOREF;

	m_bIsRunning = true;
	m_pSystemNext = nullptr;
}

SDE_System::~SDE_System()
{
	lua_State* pState = SDE_Director::Instance().GetLuaState();

	SDE_LuaUtility::GetRuntime(pState, m_pScene->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);

	if (m_nRefStart != LUA_NOREF)
		luaL_unref(pState, -1, m_nRefStart);

	if (m_nRefStart != LUA_NOREF)
		luaL_unref(pState, -1, m_nRefUpdate);
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

		SDE_LuaUtility::GetTemporary(pState);
		lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefSystem->strName.c_str());
		lua_pushnil(pState);
		lua_rawset(pState, -3);

		pDefSystem->~SDE_SystemDef();
		return 0;
	}
};

SDE_LUA_FUNC(SDE_System_ForeachComponent)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	std::string strComponentName = luaL_checkstring(pState, 2);

	pSystem->GetScene()->ForeachComponent(
		strComponentName,
		[](SDE_Component* pComponent)
		{
			lua_State* pState = SDE_Director::Instance().GetLuaState();
			
			lua_pushvalue(pState, 3);
			lua_pushlightuserdata(pState, pComponent);
			
			if (lua_pcall(pState, 1, 0, 0))
			{
				SDE_Debug::Instance().OutputInfo(
					"%s\n", lua_tostring(pState, -1)
				);
				return;
			}
		}
	);

	return 0;
}

SDE_LUA_FUNC(SDE_System_GetStartFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	std::string strName = pSystem->GetName();

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pSystem->GetStartRef());

	return 1;
}

SDE_LUA_FUNC(SDE_System_GetUpdateFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	std::string strName = pSystem->GetName();

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pSystem->GetUpdateRef());

	return 1;
}

SDE_LUA_FUNC(SDE_System_GetStopFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	std::string strName = pSystem->GetName();

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pSystem->GetStopRef());

	return 1;
}

SDE_LUA_FUNC(SDE_System_GetNextSystem)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	if (pSystem)
	{
		lua_pushlightuserdata(pState, pSystem->GetNextSystem());
	}
	else lua_pushnil(pState);

	return 1;
}

SDE_LUA_FUNC(SDE_System_SetStartFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushvalue(pState, 2);

	if (pSystem->GetStartRef() == LUA_NOREF)
	{
		pSystem->SetStartRef(luaL_ref(pState, -2));
	}
	else lua_rawseti(pState, -2, pSystem->GetStartRef());

	return 0;
}

SDE_LUA_FUNC(SDE_System_SetUpdateFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushvalue(pState, 2);

	if (pSystem->GetUpdateRef() == LUA_NOREF)
	{
		pSystem->SetUpdateRef(luaL_ref(pState, -2));
	}
	else lua_rawseti(pState, -2, pSystem->GetUpdateRef());

	return 0;
}

SDE_LUA_FUNC(SDE_System_SetStopFunc)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pSystem->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);
	lua_pushvalue(pState, 2);

	if (pSystem->GetStopRef() == LUA_NOREF)
	{
		pSystem->SetStopRef(luaL_ref(pState, -2));
	}
	else lua_rawseti(pState, -2, pSystem->GetStopRef());

	return 0;
}

SDE_LUA_FUNC(SDE_System_SetNextSystem)
{
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SYSTEM);
	SDE_System* pSystemNext = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 2, SDE_TYPE_SYSTEM);
	pSystem->SetNextSystem(pSystemNext);
	return 0;
}

SDE_LuaPackage g_packageSystem =
{
	{ "ForeachComponent",	SDE_System_ForeachComponent },

	{ "GetStartFunc",		SDE_System_GetStartFunc },
	{ "GetUpdateFunc",		SDE_System_GetUpdateFunc },
	{ "GetStopFunc",		SDE_System_GetStopFunc },
	{ "GetNextSystem",		SDE_System_GetNextSystem },

	{ "SetStartFunc",		SDE_System_SetStartFunc },
	{ "SetUpdateFunc",		SDE_System_SetUpdateFunc },
	{ "SetStopFunc",		SDE_System_SetStopFunc },
	{ "SetNextSystem",		SDE_System_SetNextSystem }
};