#ifndef _SDE_SYSTEM_H_
#define _SDE_SYSTEM_H_

#define SDE_TYPE_SYSTEM		"SDE_System"
#define SDE_TYPE_SYSTEMDEF	"SDE_SystemDef"

#include "SDE_LuaLightUserdata.h"

class SDE_LuaMetatable;

class SDE_Scene;

struct SDE_SystemDef
{

};

class SDE_System : public SDE_LuaLightUserdata
{
public:


private:


public:
	SDE_System(SDE_Scene* pScene, const SDE_SystemDef& defSystem);
	~SDE_System();
};

extern SDE_LuaMetatable g_metatableSystemDef;
extern SDE_LuaLightMetatable g_metatableSystem;

#endif // !_SDE_SYSTEM_H_