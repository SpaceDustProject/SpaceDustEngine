#ifndef _SDE_ENTITY_H_
#define _SDE_ENTITY_H_

#include "SDE_LuaLightUserdata.h"

#define SDE_TYPE_ENTITY		"SDE_Entity"
#define SDE_TYPE_ENTITYDEF	"SDE_EntityDef"

class SDE_Scene;
struct SDE_ComponentDef;
class SDE_Component;

class SDE_LuaMetatable;
class SDE_LuaPackage;

struct SDE_EntityDef : SDE_LuaLightUserdataDef
{
	SDE_EntityDef(const std::string strNameIn)
		: SDE_LuaLightUserdataDef(strNameIn) {}
};

class SDE_Entity : public SDE_LuaLightUserdata
{
public:
	SDE_Scene*		GetScene();

	SDE_Component*	CreateComponent(const SDE_ComponentDef& defComponent);
	void			DestroyComponent(SDE_Component* pComponent);
	SDE_Component*	GetComponent(const std::string& strComponentName);

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Entity(SDE_Scene* pScene, const SDE_EntityDef& defEntity);
	~SDE_Entity();

	friend class SDE_Scene;
};

extern SDE_LuaMetatable g_metatableEntityDef;
extern SDE_LuaPackage g_packageEntity;

#endif // !_SDE_ENTITY_H_