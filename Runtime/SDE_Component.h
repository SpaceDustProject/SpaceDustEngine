#ifndef _SDE_COMPONENT_H_
#define _SDE_COMPONENT_H_

#define SDE_COMPONENT_NAME		"SDE_Component"
#define SDE_COMPONENTDEF_NAME	"SDE_Component_Def"

#include <lua.hpp>
#include <string>

class SDE_Entity;
class SDE_Scene;

class SDE_Component
{
public:
	struct Def
	{
		std::string strType;
	};

	SDE_Entity*			GetEntity();
	const std::string&	GetType();
	int					GetRef();

private:
	SDE_Entity* m_pEntity;
	std::string m_strType;
	int			m_nRef;

private:
	SDE_Component(Def* defComponent);
	~SDE_Component();

	friend class SDE_Entity;
	friend class SDE_Scene;
};

extern luaL_Reg g_funcComponentMember[];
extern luaL_Reg g_funcComponentDefMember[];

#endif // !_SDE_COMPONENT_H_