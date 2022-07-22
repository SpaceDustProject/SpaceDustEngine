#include "SDE_Scene.h"

#include "SDE_Debug.h"
#include "SDE_MemoryPool.h"
#include "SDE_LuaUtility.h"

#include "SDE_Director.h"
#include "SDE_Entity.h"
#include "SDE_System.h"
#include "SDE_Component.h"

#include <list>
#include <unordered_set>
#include <unordered_map>

class SDE_Scene::Impl
{
public:
	SDE_MemoryPool m_memoryPool;

	std::list<SDE_System*> m_listSystemIndependent;

	// 储存该场景运行的系统
	std::unordered_map<
		std::string, SDE_System*
	> m_mapSystem;

	// 储存该场景的所有组件
	std::unordered_map<
		std::string, std::unordered_set<SDE_Component*>
	> m_mapComponent;

public:
	Impl(const SDE_SceneDef& defScene)
	{
		lua_State* pState = SDE_Director::Instance().GetLuaState();

		// 在 Lua 中为该场景创建环境
		SDE_LuaUtility::GetRuntime(pState);
		lua_pushstring(pState, defScene.strName.c_str());
		lua_newtable(pState);
		{
			// 用于储存所有 System 的表
			lua_pushstring(pState, SDE_TYPE_SYSTEM);
			lua_newtable(pState);
			lua_rawset(pState, -3);

			// 用于储存所有 Component 的表
			lua_pushstring(pState, SDE_TYPE_COMPONENT);
			lua_newtable(pState);
			lua_rawset(pState, -3);
		}
		lua_rawset(pState, -3);
		lua_pop(pState, 1);
	}
};

void SDE_Scene::Step()
{
	lua_State* pState = SDE_Director::Instance().GetLuaState();

	for (std::list<SDE_System*>::iterator iterSystem = m_pImpl->m_listSystemIndependent.begin();
		iterSystem != m_pImpl->m_listSystemIndependent.end(); iterSystem++)
	{
		SDE_System* pSystem = *iterSystem;

		while (pSystem)
		{
			if (pSystem->IsRunning() && pSystem->GetUpdateRef() != LUA_NOREF)
			{
				lua_rawgeti(pState, -1, pSystem->GetUpdateRef());
				lua_pushlightuserdata(pState, pSystem);
				
				if (lua_pcall(pState, 1, 0, 0))
				{
					SDE_Debug::Instance().OutputError(
						"%s\n", lua_tostring(pState, -1)
					);
				}
			}
			pSystem = pSystem->GetNextSystem();
		}
	}
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

	m_pImpl->m_listSystemIndependent.push_back(pSystem);
	m_pImpl->m_mapSystem[pSystem->GetName()] = pSystem;

	return pSystem;
}

void SDE_Scene::DestroySystem(SDE_System* pSystem)
{
	if (m_pImpl->m_mapSystem.find(pSystem->GetName()) == m_pImpl->m_mapSystem.end())
	{
		SDE_Debug::Instance().OutputInfo(
			"There're no system named %s.\n",
			pSystem->GetName()
		);
		return;
	}
	m_pImpl->m_mapSystem.erase(pSystem->GetName());

	for (std::list<SDE_System*>::iterator iterSystem = m_pImpl->m_listSystemIndependent.begin();
		iterSystem != m_pImpl->m_listSystemIndependent.end(); iterSystem++)
	{
		SDE_System* pSystemFound = *iterSystem;
		SDE_System* pSystemPrev = nullptr;

		while (pSystemFound)
		{
			if (pSystemFound == pSystem)
			{
				if (!pSystemPrev)
				{
					m_pImpl->m_listSystemIndependent.erase(iterSystem);
					m_pImpl->m_listSystemIndependent.push_back(pSystemFound->GetNextSystem());
				}
				else pSystemPrev->SetNextSystem(pSystemFound->GetNextSystem());

				break;
			}
			pSystemPrev = pSystemFound;
			pSystemFound = pSystemFound->GetNextSystem();
		}
	}

	pSystem->~SDE_System();
	m_pImpl->m_memoryPool.Free(pSystem, sizeof(SDE_System));
}

void SDE_Scene::AddComponent(SDE_Component* pComponent)
{
	m_pImpl->m_mapComponent[pComponent->GetName()].insert(pComponent);
}

void SDE_Scene::DeleteComponent(SDE_Component* pComponent)
{
	if (m_pImpl->m_mapComponent.find(pComponent->GetName()) == m_pImpl->m_mapComponent.end() ||
		m_pImpl->m_mapComponent[pComponent->GetName()].find(pComponent) == m_pImpl->m_mapComponent[pComponent->GetName()].end())
	{
		return;
	}
	m_pImpl->m_mapComponent[pComponent->GetName()].erase(pComponent);
}

void SDE_Scene::RemoveSystemFromList(SDE_System* pSystem)
{
	for (std::list<SDE_System*>::iterator iterSystem = m_pImpl->m_listSystemIndependent.begin();
		iterSystem != m_pImpl->m_listSystemIndependent.end(); iterSystem++)
	{
		if (*iterSystem == pSystem)
		{
			m_pImpl->m_listSystemIndependent.erase(iterSystem);
			break;
		}
	}
}

void SDE_Scene::ForeachComponent(const std::string& strName, void(*funcCalled)(SDE_Component*))
{
	if (m_pImpl->m_mapComponent.find(strName) == m_pImpl->m_mapComponent.end())
		return;

	for (std::unordered_set<SDE_Component*>::iterator iterComponent = m_pImpl->m_mapComponent[strName].begin();
		iterComponent != m_pImpl->m_mapComponent[strName].end(); iterComponent++)
	{
		funcCalled(*iterComponent);
	}
}

SDE_MemoryPool* SDE_Scene::GetMemoryPool()
{
	return &m_pImpl->m_memoryPool;
}

SDE_Scene::SDE_Scene(const SDE_SceneDef& defScene)
	: SDE_LuaLightUserdata(SDE_TYPE_SCENE)
{
	m_strName = defScene.strName;
	
	void* pMem = SDE_Director::Instance().GetMemoryPool()->Allocate(sizeof(SDE_Scene));
	m_pImpl = new (pMem) Impl(defScene);
}

SDE_Scene::~SDE_Scene()
{
	lua_State* pState = SDE_Director::Instance().GetLuaState();

	// 将整个场景 Lua 环境连根拔起
	SDE_LuaUtility::GetRuntime(pState);
	lua_pushstring(pState, m_strName.c_str());
	lua_pushnil(pState);
	lua_rawset(pState, -3);

	m_pImpl->~Impl();
	SDE_Director::Instance().GetMemoryPool()->Free(m_pImpl, sizeof(SDE_Scene));
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

	lua_pushlightuserdata(pState, pEntity);
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