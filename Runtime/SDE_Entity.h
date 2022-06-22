#ifndef _SDE_ENTITY_H_
#define _SDE_ENTITY_H_

#define SDE_ENTITY_NAME		"SDE_Entity"
#define SDE_ENTITYDEF_NAME	"SDE_Entity_Def"

#include "SDE_Component.h"

#include <initializer_list>

class SDE_Scene;

class SDE_Entity
{
public:
	struct Def
	{
		std::initializer_list<SDE_Component::Def> listDefComponent;
	};

	lua_Integer GetID();
	SDE_Scene*	GetScene();

	lua_Integer AddComponent(const SDE_Component::Def& defComponent);
	bool		DeleteComponent(lua_Integer nID);

private:
	void		SetID(lua_Integer nID);

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Entity(SDE_Scene* pScene, const Def& defEntity);
	~SDE_Entity();

	friend class SDE_Scene;
};

extern luaL_Reg g_funcEntityMember[];
extern luaL_Reg g_funcEntityDefMember[];

#endif // !_SDE_ENTITY_H_