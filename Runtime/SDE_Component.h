#ifndef _SDE_COMPONENT_H_
#define _SDE_COMPONENT_H_

#include "SDE_LuaLightUserdata.h"

#define SDE_TYPE_COMPONENT		"SDE_Component"
#define SDE_TYPE_COMPONENTDEF	"SDE_ComponentDef"

class SDE_Entity;

class SDE_LuaMetatable;
class SDE_LuaPackage;

struct SDE_ComponentDef : SDE_LuaLightUserdataDef
{
	SDE_ComponentDef(const std::string& strName)
		: SDE_LuaLightUserdataDef(strName) {}
};

class SDE_Component : public SDE_LuaLightUserdata
{
public:
	SDE_Entity*		GetEntity();
	long long		GetRef();

	void			SetRef(long long nRef);

private:
	SDE_Entity*		m_pEntity;
	long long		m_nRef;

private:
	SDE_Component(SDE_Entity* pEntity, const SDE_ComponentDef& defComponent);
	~SDE_Component();

	friend class SDE_Entity;
};

extern SDE_LuaMetatable g_metatableComponentDef;
extern SDE_LuaPackage g_packageComponent;

#endif // !_SDE_COMPONENT_H_