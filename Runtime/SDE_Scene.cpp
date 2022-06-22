#include "SDE_Scene.h"

#include "SDE_MemoryPool.h"
#include "SDE_VirtualMachine.h"
#include "SDE_Director.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>

class SDE_Scene::Impl
{
public:
	SDE_Director* m_pDirector;								// 场景所属导演

	std::unordered_set<SDE_Entity*>		m_setEntity;		// 场景中的实体
	std::unordered_set<SDE_Component*>	m_setComponent;		// 场景中的组件
	std::unordered_set<SDE_System*>		m_setSystem;		// 场景中的系统

public:
	Impl(SDE_Director* pDirector, const SDE_Scene::Def& defScene)
	{
		m_pDirector = pDirector;

		lua_State* pState = m_pDirector->GetLuaVM();

		SDE_VirtualMachine::Instance().GetGlobal(pState);
		lua_getfield(pState, -1, "Runtime");
		lua_getfield(pState, -1, "System");
		lua_getfield(pState, -1, "OnInit");
		lua_pushnil(pState);
		while (lua_next(pState, -2))
		{
			lua_pcall(pState, 0, 0, 0);
			lua_pop(pState, 1);
		}
		lua_pop(pState, 4);
	}
	~Impl()
	{
		lua_State* pState = m_pDirector->GetLuaVM();
		
		SDE_VirtualMachine::Instance().GetGlobal(pState);
		lua_getfield(pState, -1, "Runtime");
		lua_getfield(pState, -1, "System");
		lua_getfield(pState, -1, "OnUpdate");
		lua_pushnil(pState);
		while (lua_next(pState, -2))
		{
			lua_pcall(pState, 0, 0, 0);
			lua_pop(pState, 1);
		}
		lua_pop(pState, 4);
	}
};

bool SDE_Scene::Step()
{
	lua_State* pState = m_pImpl->m_pDirector->GetLuaVM();

	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Runtime");
	lua_getfield(pState, -1, "System");
	lua_getfield(pState, -1, "OnQuit");
	lua_pushnil(pState);
	while (lua_next(pState, -2))
	{
		lua_pcall(pState, 0, 0, 0);
		lua_pop(pState, 1);
	}
	lua_pop(pState, 3);

	return true;
}

SDE_Director* SDE_Scene::GetDirector()
{
	return m_pImpl->m_pDirector;
}

SDE_Entity* SDE_Scene::CreateEntity(const SDE_Entity::Def& defEntity)
{
	void* pMem = m_pImpl->m_pDirector->GetMemoryPool()->Allocate(sizeof(SDE_Entity));
	SDE_Entity* pEntity = pEntity = new (pMem) SDE_Entity(this, defEntity);

	m_pImpl->m_setEntity.insert(pEntity);

	return pEntity;
}

SDE_System* SDE_Scene::CreateSystem(const SDE_System::Def& defSystem)
{
	void* pMem = m_pImpl->m_pDirector->GetMemoryPool()->Allocate(sizeof(SDE_System));
	SDE_System* pSystem = new (pMem) SDE_System(this, defSystem);

	m_pImpl->m_setSystem.insert(pSystem);

	return pSystem;
}

SDE_Component* SDE_Scene::CreateComponent(SDE_Entity* pEntity, const SDE_Component::Def& defComponent)
{
	if (m_pImpl->m_setEntity.find(pEntity) == m_pImpl->m_setEntity.end())
		return false;

	void* pMem = m_pImpl->m_pDirector->GetMemoryPool()->Allocate(sizeof(SDE_Component));
	SDE_Component* pComponent = new (pMem) SDE_Component(pEntity, defComponent);

	m_pImpl->m_setComponent.insert(pComponent);

	return pComponent;
}

bool SDE_Scene::DestroyEntity(SDE_Entity* pEntity)
{
	if (m_pImpl->m_setEntity.find(pEntity) == m_pImpl->m_setEntity.end())
		return false;

	pEntity->~SDE_Entity();
	m_pImpl->m_pDirector->GetMemoryPool()->Free(pEntity, sizeof(SDE_Entity));

	return true;
}

bool SDE_Scene::DestroySystem(SDE_System* pSystem)
{
	if (m_pImpl->m_setSystem.find(pSystem) == m_pImpl->m_setSystem.end())
		return false;

	pSystem->~SDE_System();
	m_pImpl->m_pDirector->GetMemoryPool()->Free(pSystem, sizeof(SDE_System));

	return true;
}

bool SDE_Scene::DestroyComponent(SDE_Component* pComponent)
{
	if (m_pImpl->m_setComponent.find(pComponent) == m_pImpl->m_setComponent.end())
		return false;
	
	pComponent->~SDE_Component();
	m_pImpl->m_pDirector->GetMemoryPool()->Free(pComponent, sizeof(SDE_Component));

	return true;
}

SDE_Scene::SDE_Scene(SDE_Director* pDirector, const SDE_Scene::Def& defScene)
{
	void* pMem = pDirector->GetMemoryPool()->Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl(pDirector, defScene);
}

SDE_Scene::~SDE_Scene()
{
	m_pImpl->~Impl();
	m_pImpl->m_pDirector->GetMemoryPool()->Free(m_pImpl, sizeof(Impl));
}

SDE_API SDE_Scene_CreateEntity(lua_State* pState)
{
	SDE_Scene* pScene = (SDE_Scene*)luaL_checkudata(pState, 1, SDE_SCENE_NAME);
	SDE_Entity::Def* pDefEntity = (SDE_Entity::Def*)luaL_checkudata(pState, 2, SDE_ENTITYDEF_NAME);
	lua_pushlightuserdata(pState, pScene->CreateEntity(*pDefEntity));
	luaL_setmetatable(pState, SDE_ENTITY_NAME);
	return 1;
}

SDE_API SDE_Scene_CreateSystem(lua_State* pState)
{
	SDE_Scene* pScene = (SDE_Scene*)luaL_checkudata(pState, 1, SDE_SCENE_NAME);
	SDE_System::Def* pDefSystem = (SDE_System::Def*)luaL_checkudata(pState, 2, SDE_SYSTEMDEF_NAME);
	lua_pushlightuserdata(pState, pScene->CreateSystem(*pDefSystem));
	luaL_setmetatable(pState, SDE_SYSTEM_NAME);
	return 1;
}

SDE_API SDE_Scene_DestroyEntity(lua_State* pState)
{
	SDE_Scene* pScene = (SDE_Scene*)luaL_checkudata(pState, 1, SDE_SCENE_NAME);
	SDE_Entity* pEntity = (SDE_Entity*)luaL_checkudata(pState, 2, SDE_ENTITY_NAME);
	lua_pushboolean(pState, pScene->DestroyEntity(pEntity));
	return 1;
}

SDE_API SDE_Scene_DestroySystem(lua_State* pState)
{
	SDE_Scene* pScene = (SDE_Scene*)luaL_checkudata(pState, 1, SDE_SCENE_NAME);
	SDE_System* pSystem = (SDE_System*)luaL_checkudata(pState, 2, SDE_SYSTEM_NAME);
	lua_pushboolean(pState, pScene->DestroySystem(pSystem));
	return 1;
}

luaL_Reg g_funcSceneMember[] =
{
	{ "AddEntity",		SDE_Scene_CreateEntity },
	{ "AddSystem",		SDE_Scene_CreateSystem },

	{ "DeleteEntity",   SDE_Scene_CreateEntity },
	{ "DeleteSystem",	SDE_Scene_CreateSystem },

	{ nullptr,			nullptr }
};

SDE_API SDE_SceneDef_AddEntityDef(lua_State* pState)
{
	return 1;
}

SDE_API SDE_SceneDef_DeleteEntityDef(lua_State* pState)
{
	return 1;
}

SDE_API SDE_SceneDef_AddSystemDef(lua_State* pState)
{
	return 1;
}

SDE_API SDE_SceneDef_DeleteSystemDef(lua_State* pState)
{
	return 1;
}

luaL_Reg g_funcSceneDefMember[] =
{
	{ "AddEntityDef",		SDE_SceneDef_AddEntityDef },
	{ "DeleteEntityDef",	SDE_SceneDef_DeleteEntityDef },

	{ "AddSystemDef",		SDE_SceneDef_AddSystemDef },
	{ "DeleteSystemDef",	SDE_SceneDef_DeleteSystemDef },

	{ nullptr,				nullptr}
};