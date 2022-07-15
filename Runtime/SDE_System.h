#ifndef _SDE_SYSTEM_H_
#define _SDE_SYSTEM_H_

#define SDE_TYPE_SYSTEM				"SDE_System"
#define SDE_TYPE_SYSTEMDEF			"SDE_SystemDef"

#define SDE_NAME_SYSTEM_ONINIT		"SDE_System_OnInit"
#define SDE_NAME_SYSTEM_ONUPDATE	"SDE_System_OnUpdate"
#define SDE_NAME_SYSTEM_ONQUIT		"SDE_System_OnQuit"

#include "SDE_LuaLightUserdata.h"

class SDE_LuaMetatable;
class SDE_LuaPackage;

class SDE_Scene;

struct SDE_SystemDef : SDE_LuaLightUserdataDef
{
	SDE_SystemDef(const std::string strNameIn)
		: SDE_LuaLightUserdataDef(strNameIn) {}
};

class SDE_System : public SDE_LuaLightUserdata
{
public:
	SDE_Scene*			GetScene();

	long long			GetInitRef();
	long long			GetUpdateRef();
	long long			GetQuitRef();

	void				SetInitRef(long long nRef);
	void				SetUpdateRef(long long nRef);
	void				SetQuitRef(long long nRef);

private:
	SDE_Scene*	m_pScene;

	long long	m_nRefInit;
	long long	m_nRefUpdate;
	long long	m_nRefQuit;

private:
	SDE_System(SDE_Scene* pScene, const SDE_SystemDef& defSystem);
	~SDE_System();

	friend class SDE_Scene;
};

extern SDE_LuaMetatable g_metatableSystemDef;
extern SDE_LuaPackage g_packageSystem;

#endif // !_SDE_SYSTEM_H_