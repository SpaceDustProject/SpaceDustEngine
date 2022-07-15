#include "SDE_Entity.h"

#include "SDE_Director.h"
#include "SDE_MemoryPool.h"
#include "SDE_LuaUtility.h"

#include "SDE_Scene.h"
#include "SDE_Component.h"

#include <unordered_map>

class SDE_Entity::Impl
{
public:
	SDE_Scene* m_pScene;

	std::unordered_map<std::string, SDE_Component*> m_mapComponent;

public:
	Impl(const SDE_EntityDef& defEntity)
	{
		m_pScene = nullptr;
	}
};

SDE_Scene* SDE_Entity::GetScene()
{
	return m_pImpl->m_pScene;
}

SDE_Component* SDE_Entity::CreateComponent(const SDE_ComponentDef& defComponent)
{
	if (m_pImpl->m_mapComponent.find(defComponent.strName) !=
		m_pImpl->m_mapComponent.end())
	{
		return nullptr;
	}
	void* pMem = m_pImpl->m_pScene->GetMemoryPool()->Allocate(sizeof(SDE_Component));
	SDE_Component* pComponent = new (pMem) SDE_Component(this, defComponent);
	m_pImpl->m_mapComponent[defComponent.strName] = pComponent;
	return pComponent;
}

void SDE_Entity::DestroyComponent(SDE_Component* pComponent)
{
	if (!pComponent || m_pImpl->m_mapComponent.find(pComponent->GetName()) ==
		m_pImpl->m_mapComponent.end())
	{
		return;
	}
	pComponent->~SDE_Component();
	m_pImpl->m_pScene->GetMemoryPool()->Free(pComponent, sizeof(SDE_Component));
}

SDE_Component* SDE_Entity::GetComponent(const std::string& strComponentName)
{
	if (m_pImpl->m_mapComponent.find(strComponentName) == m_pImpl->m_mapComponent.end())
		return nullptr;

	return m_pImpl->m_mapComponent[strComponentName];
}

SDE_Entity::SDE_Entity(SDE_Scene* pScene, const SDE_EntityDef& defEntity)
	: SDE_LuaLightUserdata(SDE_TYPE_ENTITY)
{
	void* pMem = pScene->GetMemoryPool()->Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl(defEntity);

	m_pImpl->m_pScene = pScene;
}

SDE_Entity::~SDE_Entity()
{
	lua_State* pState = SDE_Director::Instance().GetLuaState();

	for (std::unordered_map<std::string, SDE_Component*>::iterator iterComponent = m_pImpl->m_mapComponent.begin();
		iterComponent != m_pImpl->m_mapComponent.end(); iterComponent++)
	{
		DestroyComponent((*iterComponent).second);
	}

	m_pImpl->~Impl();
	m_pImpl->m_pScene->GetMemoryPool()->Free(m_pImpl, sizeof(Impl));
}

SDE_LuaMetatable g_metatableEntityDef =
{
	SDE_TYPE_ENTITYDEF,
	{

	},

	[](lua_State* pState)->int
	{
		SDE_EntityDef* pDefEntity = (SDE_EntityDef*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_ENTITYDEF);
		pDefEntity->~SDE_EntityDef();
		return 0;
	}
};

SDE_LUA_FUNC(SDE_Entity_CreateComponent)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_ENTITY);
	SDE_ComponentDef* pDefComponent = (SDE_ComponentDef*)SDE_LuaUtility::GetLightUserdataDef(pState, 2, SDE_TYPE_COMPONENTDEF);

	if (!pDefComponent) return 1;
	SDE_Component* pComponent = pEntity->CreateComponent(*pDefComponent);

	SDE_LuaUtility::GetRuntime(pState, pEntity->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_COMPONENT);
	lua_rawget(pState, -2);
	lua_pushstring(pState, pComponent->GetName().c_str());
	lua_rawget(pState, -2);

	// 若当前没有符合该组件的容器，则新建一个容器
	if (lua_type(pState, -1) == LUA_TNIL)
	{
		lua_pop(pState, 1);

		lua_pushstring(pState, pComponent->GetName().c_str());
		lua_newtable(pState);
		lua_rawset(pState, -3);

		lua_pushstring(pState, pComponent->GetName().c_str());
		lua_rawget(pState, -2);
	}
	lua_newtable(pState);
	pComponent->SetRef(luaL_ref(pState, -2));
	lua_pop(pState, 3);

	lua_pushlightuserdata(pState, pComponent);

	// 根据堆栈对象决定构造函数参数个数
	if (lua_gettop(pState) >= 5)
	{
		lua_pushvalue(pState, 3);
		lua_pcall(pState, 2, 0, 0);
	}
	else lua_pcall(pState, 1, 0, 0);

	lua_pushlightuserdata(pState, pComponent);
	return 1;
}

SDE_LUA_FUNC(SDE_Entity_DestroyComponent)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_ENTITY);
	SDE_Component* pComponent = (SDE_Component*)SDE_LuaUtility::GetLightUserdata(pState, 2, SDE_TYPE_COMPONENT);

	SDE_LuaUtility::GetRuntime(pState, pEntity->GetScene()->GetName().c_str());
	lua_pushstring(pState, SDE_TYPE_COMPONENT);
	lua_rawget(pState, -2);
	lua_pushstring(pState, pComponent->GetName().c_str());
	lua_rawget(pState, -2);
	luaL_unref(pState, -1, pComponent->GetRef());

	pEntity->DestroyComponent(pComponent);
	return 0;
}

SDE_LUA_FUNC(SDE_Entity_GetComponent)
{
	SDE_Entity* pEntity = (SDE_Entity*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_ENTITY);
	std::string strComponentName = luaL_checkstring(pState, 2);
	SDE_Component* pComponent = pEntity->GetComponent(strComponentName);

	if (pComponent)
	{
		lua_pushlightuserdata(pState, pComponent);
	}
	else lua_pushnil(pState);

	return 1;
}

SDE_LuaPackage g_packageEntity =
{
	{ "CreateComponent",	SDE_Entity_CreateComponent },
	{ "DestroyComponent",	SDE_Entity_DestroyComponent },
	{ "GetComponent",		SDE_Entity_GetComponent }
};