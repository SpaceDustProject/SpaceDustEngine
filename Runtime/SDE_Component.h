#ifndef _SDE_COMPONENT_H_
#define _SDE_COMPONENT_H_

#define SDE_COMPONENT_NAME		"SDE_Component"
#define SDE_COMPONENTDEF_NAME	"SDE_Component_Def"

#include <lua.hpp>
#include <string>

class SDE_Entity;

class SDE_Component
{
public:
	struct Def
	{
		std::string strName;
	};

	SDE_Entity* GetEntity();

private:
	lua_Integer m_nID;
	std::string m_strName;

private:
	SDE_Component(SDE_Entity* pEntity, const Def& defComponent);
	~SDE_Component();

	friend class SDE_Scene;
};

extern luaL_Reg g_funcComponentMember[];
extern luaL_Reg g_funcComponentDefMember[];

#endif // !_SDE_COMPONENT_H_