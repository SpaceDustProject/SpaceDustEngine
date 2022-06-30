#ifndef _SDE_DIRECTOR_H_
#define _SDE_DIRECTOR_H_

#include "SDE_Scene.h"

class SDE_MemoryPool;

class SDE_Director
{
public:
	bool				Run();											// ����

	lua_State*			GetLuaVM();										// ��ȡ�ڲ������
	SDE_MemoryPool*		GetMemoryPool();								// ��ȡ�ڲ��ڴ��

	SDE_Scene*			GetScene();										// ��ȡ��ǰ���г���
	SDE_Scene*			SwitchScene(SDE_Scene::Def* pDefScene);			// �л���ǰ����

	SDE_Entity::Def*	GetEntityDef(const std::string& strType);
	SDE_Entity::Def*	CreateEntityDef(const std::string& strType);
	bool				DestroyEntityDef(const std::string& strType);

	SDE_Component::Def*	GetComponentDef(const std::string& strType);
	SDE_Component::Def*	CreateComponentDef(const std::string& strType);
	bool				DestroyComponentDef(const std::string& strType);

	SDE_System::Def*	GetSystemDef(const std::string& strType);
	SDE_System::Def*	CreateSystemDef(const std::string& strType);
	bool				DestroySystemDef(const std::string& strType);

	SDE_Scene::Def*		GetSceneDef(const std::string& strType);
	SDE_Scene::Def*		CreateSceneDef(const std::string& strType);
	bool				DestroySceneDef(const std::string& strType);

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_Director();
	~SDE_Director();
};

extern luaL_Reg g_funcDirectorMember[];
extern SDE_Director g_director;

#endif // !_SDE_DIRECTOR_H_