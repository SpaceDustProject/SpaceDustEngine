#include "SDE_Component.h"

#include <functional>

SDE_Component::SDE_Component(Def* defComponent)
{

}

SDE_Component::~SDE_Component()
{

}

SDE_Entity* SDE_Component::GetEntity()
{
	return m_pEntity;
}

const std::string& SDE_Component::GetType()
{
	return m_strType;
}

int SDE_Component::GetRef()
{
	return m_nRef;
}

luaL_Reg g_funcComponentMember[] =
{
	{ nullptr, nullptr }
};

luaL_Reg g_funcComponentDefMember[] =
{
	{ nullptr, nullptr }
};