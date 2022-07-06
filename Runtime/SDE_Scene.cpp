#include "SDE_Scene.h"

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

SDE_LUA_FUNC(SDE_SceneDef_AddSystemDef)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);
	std::string strSystemType = luaL_checkstring(pState, 2);
	pDefScene->listDefSystem.push_back(strSystemType);
	return 0;
}

SDE_LUA_FUNC(SDE_SceneDef_DeleteSystemDef)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);
	std::string strSystemType = luaL_checkstring(pState, 2);

	std::list<std::string>::iterator iterFound = pDefScene->listDefSystem.begin();
	while (iterFound != pDefScene->listDefSystem.end())
	{
		if ((*iterFound) == strSystemType)
		{
			pDefScene->listDefSystem.erase(iterFound);
			return 0;
		}
		iterFound++;
	}

	return 0;
}

SDE_LuaMetatable g_metatableSceneDef =
{
	SDE_TYPE_SCENEDEF,
	{
		{ "AddSystemDef",		SDE_SceneDef_AddSystemDef },
		{ "DeleteSystemDef",	SDE_SceneDef_DeleteSystemDef }
	},

	[](lua_State* pState)->int
	{
		SDE_SystemDef* pSystemDef = (SDE_SystemDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);
		pSystemDef->~SDE_SystemDef();
		return 0;
	}
};

SDE_LUA_FUNC(SDE_Scene_CreateSystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_SystemDef* pSystemDef = (SDE_SystemDef*)luaL_checkudata(pState, 2, SDE_TYPE_SCENEDEF);
	lua_pushlightuserdata(pState, pScene->CreateSystem(*pSystemDef));
	return 1;
}

SDE_LUA_FUNC(SDE_Scene_DestroySystem)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_System* pSystem = (SDE_System*)SDE_LuaUtility::GetLightUserdata(pState, 2, SDE_TYPE_SYSTEM);
	pScene->DestroySystem(pSystem);
	return 0;
}

SDE_LuaLightMetatable g_metatableScene =
{
	SDE_TYPE_SCENE,
	{
		{ "CreateSystem",	SDE_Scene_CreateSystem },
		{ "DestroySystem",	SDE_Scene_DestroySystem }
	}
};