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
	SDE_Scene*	GetScene();

	long long	GetStartRef();
	long long	GetUpdateRef();
	long long	GetStopRef();
	SDE_System* GetNextSystem();

	void		SetStartRef(long long nRef);
	void		SetUpdateRef(long long nRef);
	void		SetStopRef(long long nRef);
	void		SetNextSystem(SDE_System* pSystem);

	bool		IsRunning();
	void		SetIsRunning(bool bIsRunning);

private:
	SDE_Scene*	m_pScene;

	long long	m_nRefStart;
	long long	m_nRefUpdate;
	long long	m_nRefStop;

	bool		m_bIsRunning;
	SDE_System* m_pSystemNext;

private:
	SDE_System(SDE_Scene* pScene, const SDE_SystemDef& defSystem);
	~SDE_System();

	friend class SDE_Scene;
};

extern SDE_LuaMetatable g_metatableSystemDef;
extern SDE_LuaPackage g_packageSystem;

#endif // !_SDE_SYSTEM_H_