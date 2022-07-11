#include "SDE_Scene.h"

#include "SDE_Debug.h"
#include "SDE_MemoryPool.h"
#include "SDE_LuaUtility.h"

#include "SDE_System.h"

#include <unordered_set>

class SDE_Scene::Impl
{
public:
	std::string		m_strName;

	SDE_MemoryPool	m_memoryPool;

public:
	Impl(const SDE_SceneDef& defScene)
	{
		m_strName = defScene.strName;
	}
};

const std::string& SDE_Scene::GetName()
{
	return m_pImpl->m_strName;
}

SDE_MemoryPool* SDE_Scene::GetMemoryPool()
{
	return &m_pImpl->m_memoryPool;
}

SDE_Entity* SDE_Scene::CreateEntity(const SDE_EntityDef& defEntity)
{

}

bool SDE_Scene::DestroyEntity(SDE_Entity* pEntity)
{

}

SDE_System* SDE_Scene::CreateSystem(const SDE_SystemDef& pDefSystem)
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_System));
	SDE_System* pSystem = new (pMem) SDE_System(this, pDefSystem);
	return pSystem;
}

void SDE_Scene::DestroySystem(SDE_System* pSystem)
{
	pSystem->~SDE_System();
	m_pImpl->m_memoryPool.Free(pSystem, sizeof(SDE_System));
}

SDE_Scene::SDE_Scene(const SDE_SceneDef& defScene)
	: SDE_LuaLightUserdata(SDE_TYPE_SCENE)
{
	m_pImpl = new Impl(defScene);
}

SDE_Scene::~SDE_Scene()
{
	delete m_pImpl;
}

SDE_LUA_FUNC(SDE_SceneDef_GetName)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);
	lua_pushstring(pState, pDefScene->strName.c_str());
	return 1;
}

SDE_LUA_FUNC(SDE_SceneDef_SetConstructor)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);

	if (pDefScene->bIsRegistered)
	{
		SDE_LuaUtility::GetRegistry(pState);
		lua_pushstring(pState, SDE_TYPE_SCENEDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefScene->strName.c_str());
		lua_rawget(pState, -2);
		lua_pushstring(pState, "constructor");
		lua_pushvalue(pState, 2);
		lua_rawset(pState, -3);
	}
	else
	{
		SDE_LuaUtility::GetTemporary(pState);
		lua_pushstring(pState, SDE_TYPE_SCENEDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefScene->strName.c_str());
		lua_pushvalue(pState, 2);
		lua_rawset(pState, -3);
	}
	return 0;
}

SDE_LuaMetatable g_metatableSceneDef =
{
	SDE_TYPE_SCENEDEF,
	{
		{ "GetName",		SDE_SceneDef_GetName },
		{ "SetConstructor", SDE_SceneDef_SetConstructor }
	},

	[](lua_State* pState)->int
	{
		SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);
		pDefScene->~SDE_SceneDef();
		return 0;
	}
};

SDE_LUA_FUNC(SDE_Scene_GetName)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	lua_pushstring(pState, pScene->GetName().c_str());
	return 1;
}

SDE_LUA_FUNC(SDE_Scene_CreateSystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_SystemDef* pDefSystem = nullptr;

	// 获取系统定义对象，并将构造函数留在栈顶
	if (lua_type(pState, 2) == LUA_TSTRING)
	{
		std::string strType = lua_tostring(pState, 2);

		SDE_LuaUtility::GetRegistry(pState);

		lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
		lua_rawget(pState, -2);
		lua_remove(pState, -2);

		lua_pushstring(pState, strType.c_str());
		lua_rawget(pState, -2);
		lua_remove(pState, -2);

		// 如果注册表内不存在该定义
		if (lua_type(pState, -1) == LUA_TNIL)
			return 1;

		lua_pushstring(pState, "userdata");
		lua_rawget(pState, -2);
		pDefSystem = (SDE_SystemDef*)lua_touserdata(pState, -1);
		lua_pop(pState, 1);

		lua_pushstring(pState, "constructor");
		lua_rawget(pState, -2);
		lua_remove(pState, -2);
	}
	else
	{
		pDefSystem = (SDE_SystemDef*)luaL_checkudata(pState, 2, SDE_TYPE_SYSTEMDEF);

		SDE_LuaUtility::GetTemporary(pState);

		lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
		lua_rawget(pState, -2);
		lua_remove(pState, -2);

		lua_pushstring(pState, pDefSystem->strName.c_str());
		lua_rawget(pState, -2);
		lua_remove(pState, -2);
	}

	SDE_System* pSystem = pScene->CreateSystem(*pDefSystem);

	// 根据堆栈对象决定构造函数参数个数
	lua_pushlightuserdata(pState, pSystem);

	if (lua_gettop(pState) >= 5)
	{
		lua_pushvalue(pState, 3);
		lua_pcall(pState, 2, 0, 0);
	}
	else lua_pcall(pState, 1, 0, 0);

	lua_pushlightuserdata(pState, pSystem);
	return 1;
}

SDE_LUA_FUNC(SDE_Scene_DestroySystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 2, SDE_TYPE_SYSTEM);

	SDE_LuaUtility::GetRuntime(pState, pScene->GetName());
	lua_pushstring(pState, SDE_TYPE_SYSTEM);
	lua_rawget(pState, -2);

	if (pSystem->GetInitRef() != LUA_NOREF)
	{
		lua_pushstring(pState, SDE_NAME_SYSTEM_ONINIT);
		lua_rawget(pState, -2);
		lua_pushnil(pState);
		lua_rawseti(pState, -2, pSystem->GetInitRef());
		lua_pop(pState, 1);
	}

	if (pSystem->GetUpdateRef() != LUA_NOREF)
	{
		lua_pushstring(pState, SDE_NAME_SYSTEM_ONUPDATE);
		lua_rawget(pState, -2);
		lua_pushnil(pState);
		lua_rawseti(pState, -2, pSystem->GetUpdateRef());
		lua_pop(pState, 1);
	}

	if (pSystem->GetQuitRef() != LUA_NOREF)
	{
		lua_pushstring(pState, SDE_NAME_SYSTEM_ONQUIT);
		lua_rawget(pState, -2);
		lua_pushnil(pState);
		lua_rawseti(pState, -2, pSystem->GetQuitRef());
		lua_pop(pState, 1);
	}

	pScene->DestroySystem(pSystem);
	return 0;
}

SDE_LuaPackage g_packageScene =
{
	{ "GetSceneName",	SDE_Scene_GetName },
	{ "CreateSystem",	SDE_Scene_CreateSystem },
	{ "DestroySystem",	SDE_Scene_DestroySystem }
};