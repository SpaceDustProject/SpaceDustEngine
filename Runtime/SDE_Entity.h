#ifndef _SDE_ENTITY_H_
#define _SDE_ENTITY_H_

#define SDE_ENTITY_NAME		"SDE_Entity"
#define SDE_ENTITYDEF_NAME	"SDE_Entity_Def"

#include "SDE_Component.h"

#include <list>

class SDE_Scene;

class SDE_Entity
{
public:
	struct Def
	{
		std::string						strType;
		std::list<SDE_Component::Def*>	listDefComponent;
	};

	SDE_Scene*		GetScene();

	SDE_Component*	AddComponent(SDE_Component::Def* defComponent);
	bool			DeleteComponent(SDE_Component* pComponent);

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Entity(SDE_Scene* pScene, Def* defEntity);
	~SDE_Entity();

	friend class SDE_Scene;
};

extern luaL_Reg g_funcEntityMember[];
extern luaL_Reg g_funcEntityDefMember[];

#endif // !_SDE_ENTITY_H_