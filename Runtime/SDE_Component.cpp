#include "SDE_Component.h"

#include "SDE_Director.h"
#include "SDE_LuaUtility.h"
#include "SDE_Debug.h"

#include "SDE_Scene.h"
#include "SDE_Entity.h"

SDE_Entity* SDE_Component::GetEntity()
{
	return m_pEntity;
}

long long SDE_Component::GetRef()
{
	return m_nRef;
}

void SDE_Component::SetRef(long long nRef)
{
	m_nRef = nRef;
}

SDE_Component::SDE_Component(SDE_Entity* pEntity, const SDE_ComponentDef& defComponent)
	: SDE_LuaLightUserdata(SDE_TYPE_COMPONENT)
{
	m_pEntity = pEntity;
	m_strName = defComponent.strName;

	lua_State* pState = SDE_Director::Instance().GetLuaState();

	SDE_LuaUtility::GetRuntime(pState, m_pEntity->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_COMPONENT);
	lua_rawget(pState, -2);

	lua_newtable(pState);
	m_nRef = luaL_ref(pState, -2);
	lua_pop(pState, 2);
}

SDE_Component::~SDE_Component()
{
	if (m_nRef != LUA_NOREF)
	{
		lua_State* pState = SDE_Director::Instance().GetLuaState();

		SDE_LuaUtility::GetRuntime(pState, m_pEntity->GetScene()->GetName());
		lua_pushstring(pState, SDE_TYPE_COMPONENT);
		lua_rawget(pState, -2);
		luaL_unref(pState, -1, m_nRef);
	}
}

SDE_LUA_FUNC(SDE_ComponentDef_GetName)
{
	SDE_ComponentDef* pDefComponent = (SDE_ComponentDef*)luaL_checkudata(pState, 1, SDE_TYPE_COMPONENTDEF);
	lua_pushstring(pState, pDefComponent->strName.c_str());
	return 1;
}

SDE_LUA_FUNC(SDE_ComponentDef_SetConstructor)
{
	SDE_ComponentDef* pDefComponent = (SDE_ComponentDef*)luaL_checkudata(pState, 1, SDE_TYPE_COMPONENTDEF);

	if (pDefComponent->bIsRegistered)
	{
		SDE_LuaUtility::GetRegistry(pState);
		lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefComponent->strName.c_str());
		lua_rawget(pState, -2);
		lua_pushstring(pState, "constructor");
		lua_pushvalue(pState, 2);
		lua_rawset(pState, -3);
	}
	else
	{
		SDE_LuaUtility::GetTemporary(pState);
		lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefComponent->strName.c_str());
		lua_pushvalue(pState, 2);
		lua_rawset(pState, -3);
	}
	return 0;
}

SDE_LuaMetatable g_metatableComponentDef =
{
	SDE_TYPE_COMPONENTDEF,
	{
		{ "GetName",		SDE_ComponentDef_GetName },
		{ "SetConstructor", SDE_ComponentDef_SetConstructor }
	},

	[](lua_State* pState)->int
	{
		SDE_ComponentDef* pDefComponent = (SDE_ComponentDef*)luaL_checkudata(pState, 1, SDE_TYPE_COMPONENTDEF);

		SDE_LuaUtility::GetTemporary(pState);
		lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
		lua_rawget(pState, -2);
		lua_pushstring(pState, pDefComponent->strName.c_str());
		lua_pushnil(pState);
		lua_rawset(pState, -3);

		pDefComponent->~SDE_ComponentDef();
		return 0;
	}
};

SDE_LUA_FUNC(SDE_Component_GetEntity)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_COMPONENT);
	lua_pushlightuserdata(pState, pComponent->GetEntity());
	return 1;
}

SDE_LUA_FUNC(SDE_Component_GetValue)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_COMPONENT);

	SDE_LuaUtility::GetRuntime(pState, pComponent->GetEntity()->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_COMPONENT);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pComponent->GetRef());

	lua_pushvalue(pState, 2);
	lua_rawget(pState, -2);

	return 1;
}

SDE_LUA_FUNC(SDE_Component_SetValue)
{
	SDE_Component* pComponent = (SDE_Component*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_COMPONENT);

	SDE_LuaUtility::GetRuntime(pState, pComponent->GetEntity()->GetScene()->GetName());
	lua_pushstring(pState, SDE_TYPE_COMPONENT);
	lua_rawget(pState, -2);
	lua_rawgeti(pState, -1, pComponent->GetRef());

	lua_pushvalue(pState, 2);
	lua_pushvalue(pState, 3);
	lua_rawset(pState, -3);

	return 0;
}

SDE_LuaPackage g_packageComponent =
{
	{ "GetEntity",	SDE_Component_GetEntity },

	{ "GetValue",	SDE_Component_GetValue },
	{ "SetValue",	SDE_Component_SetValue }
};