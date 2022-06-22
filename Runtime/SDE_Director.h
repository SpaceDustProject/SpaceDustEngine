#ifndef _SDE_DIRECTOR_H_
#define _SDE_DIRECTOR_H_

#include "SDE_Scene.h"

class SDE_MemoryPool;

class SDE_Director
{
public:
	bool				Run();											// ����

	SDE_Scene*			GetScene();										// ��ȡ��ǰ���г���
	SDE_Scene*			SwitchScene(const SDE_Scene::Def& defScene);	// �л���ǰ����

	lua_State*			GetLuaVM();										// ��ȡ�ڲ������
	SDE_MemoryPool*		GetMemoryPool();								// ��ȡ�ڲ��ڴ��

	SDE_Entity::Def*	GetEntityDef(const std::string& strName);
	void				RegisterEntityDef(const std::string& strName, SDE_Entity::Def* pDefEntity);
	bool				UnregisterEntityDef(const std::string& strName);

	SDE_Component::Def*	GetComponentDef(const std::string& strName);
	void				RegisterComponentDef(const std::string& strName, const SDE_Component::Def& defComponent);
	bool				UnregisterComponentDef(const std::string& strName);

	SDE_System::Def*	GetSystemDef(const std::string& strName);
	void				RegisterSystemDef(const std::string& strName, const SDE_System::Def& defSystem);
	bool				UnregisterSystemDef(const std::string& strName);

	SDE_Scene::Def*		GetSceneDef(const std::string& strName);
	void				RegisterSceneDef(const std::string& strName, SDE_Scene::Def* pDefScene);
	bool				UnregisterSceneDef(const std::string& strName);

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_Director();
	~SDE_Director();
};

extern SDE_Director g_director;
extern luaL_Reg g_funcDirectorMember[];

#endif // !_SDE_DIRECTOR_H_