#include "SDE_Scene.h"

#include "SDE_Debug.h"
#include "SDE_MemoryPool.h"
#include "SDE_LuaUtility.h"

#include "SDE_System.h"
#include "SDE_Entity.h"

#include <unordered_set>

class SDE_Scene::Impl
{
public:
	SDE_MemoryPool	m_memoryPool;

public:
	Impl(const SDE_SceneDef& defScene)
	{

	}
};

SDE_MemoryPool* SDE_Scene::GetMemoryPool()
{
	return &m_pImpl->m_memoryPool;
}

SDE_Entity* SDE_Scene::CreateEntity(const SDE_EntityDef& defEntity)
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_Entity));
	SDE_Entity* pEntity = new (pMem) SDE_Entity(this, defEntity);
	return pEntity;
}

void SDE_Scene::DestroyEntity(SDE_Entity* pEntity)
{
	pEntity->~SDE_Entity();
	m_pImpl->m_memoryPool.Free(pEntity, sizeof(SDE_Entity));
}

SDE_System* SDE_Scene::CreateSystem(const SDE_SystemDef& defSystem)
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_System));
	SDE_System* pSystem = new (pMem) SDE_System(this, defSystem);
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

	this->SetName(defScene.strName);
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

SDE_LUA_FUNC(SDE_Scene_CreateEntity)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_EntityDef* pDefEntity = (SDE_EntityDef*)SDE_LuaUtility::GetLightUserdataDef(pState, 2, SDE_TYPE_ENTITYDEF);

	if (!pDefEntity) return 1;
	SDE_Entity* pEntity = pScene->CreateEntity(*pDefEntity);

	lua_pushlightuserdata(pState, pEntity);

	// 根据堆栈对象决定构造函数参数个数
	if (lua_gettop(pState) >= 5)
	{
		lua_pushvalue(pState, 3);
		lua_pcall(pState, 2, 0, 0);
	}
	else lua_pcall(pState, 1, 0, 0);

	lua_pushlightuserdata(pState, pScene);
	return 1;
}

SDE_LUA_FUNC(SDE_Scene_DestroyEntity)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_Entity* pEntity = (SDE_Entity*)SDE_LuaUtility::GetLightUserdata(pState, 2, SDE_TYPE_ENTITY);
	pScene->DestroyEntity(pEntity);
	return 0;
}

SDE_LUA_FUNC(SDE_Scene_CreateSystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_SystemDef* pDefSystem = (SDE_SystemDef*)SDE_LuaUtility::GetLightUserdataDef(pState, 2, SDE_TYPE_SYSTEMDEF);

	if (!pDefSystem) return 1;
	SDE_System* pSystem = pScene->CreateSystem(*pDefSystem);

	lua_pushlightuserdata(pState, pSystem);

	// 根据堆栈对象决定构造函数参数个数
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
	pScene->DestroySystem(pSystem);
	return 0;
}

SDE_LuaPackage g_packageScene =
{
	{ "CreateEntity",	SDE_Scene_CreateEntity },
	{ "DestroyEntity",	SDE_Scene_DestroyEntity },

	{ "CreateSystem",	SDE_Scene_CreateSystem },
	{ "DestroySystem",	SDE_Scene_DestroySystem }
};