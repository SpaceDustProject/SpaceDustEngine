#include "SDE_Component.h"

#include "SDE_Director.h"
#include "SDE_LuaUtility.h"

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
	m_nRef = LUA_NOREF;
}

SDE_Component::~SDE_Component()
{
	if (m_nRef != LUA_NOREF)
	{
		lua_State* pState = SDE_Director::Instance().GetLuaState();

		SDE_LuaUtility::GetRuntime(pState);
		lua_pushstring(pState, SDE_TYPE_COMPONENT);
		lua_rawget(pState, -2);
		lua_pushstring(pState, GetName().c_str());
		lua_rawget(pState, -2);
		luaL_unref(pState, -1, m_nRef);
	}
}

SDE_LuaMetatable g_metatableComponentDef =
{
	SDE_TYPE_COMPONENTDEF,
	{

	},

	[](lua_State* pState)->int
	{
		SDE_ComponentDef* pComponent = (SDE_ComponentDef*)luaL_checkudata(pState, 1, SDE_TYPE_COMPONENTDEF);
		pComponent->~SDE_ComponentDef();
		return 0;
	}
};

SDE_LuaPackage g_packageComponent =
{

};