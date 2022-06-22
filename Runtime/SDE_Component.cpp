#include "SDE_Component.h"

#include <functional>

SDE_Component::SDE_Component(SDE_Entity* pEntity, const Def& defComponent)
{

}

SDE_Component::~SDE_Component()
{

}

luaL_Reg g_funcComponentMember[] =
{
	{ nullptr, nullptr }
};

luaL_Reg g_funcComponentDefMember[] =
{
	{ nullptr, nullptr }
};