#ifndef _SDE_SCENE_H_
#define _SDE_SCENE_H_

#define SDE_TYPE_SCENE		"SDE_Scene"
#define SDE_TYPE_SCENEDEF	"SDE_SceneDef"

#include "SDE_LuaLightUserdata.h"

class SDE_MemoryPool;
class SDE_Director;

class SDE_Entity;
struct SDE_EntityDef;

class SDE_Component;
struct SDE_ComponentDef;

class SDE_System;
struct SDE_SystemDef;

class SDE_LuaPackage;
class SDE_LuaMetatable;

struct SDE_SceneDef : SDE_LuaLightUserdataDef
{
	SDE_SceneDef(const std::string strNameIn)
		: SDE_LuaLightUserdataDef(strNameIn) {}
};

class SDE_Scene : public SDE_LuaLightUserdata
{
public:
	void				Step();

	SDE_Entity*			CreateEntity(const SDE_EntityDef& defEntity);
	void				DestroyEntity(SDE_Entity* pEntity);

	SDE_System*			CreateSystem(const SDE_SystemDef& defSystem);
	void				DestroySystem(SDE_System* pSystem);

	void				AddComponent(SDE_Component* pComponent);
	void				DeleteComponent(SDE_Component* pComponent);

	SDE_MemoryPool*		GetMemoryPool();

	// ��ĳϵͳ�Ӷ���ϵͳ�б����Ƴ���������ĳ��ϵͳ����
	void				RemoveSystemFromList(SDE_System* pSystem);

	// ����ָ�����͵������������Ϊ�������ö�ջ��ָ���������ĺ���
	void				ForeachComponent(const std::string& strName, void(*funcCalled)(SDE_Component*));

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Scene(const SDE_SceneDef& defScene);
	~SDE_Scene();

	friend class SDE_Director;
};

extern SDE_LuaMetatable g_metatableSceneDef;
extern SDE_LuaPackage g_packageScene;

#endif // !_SDE_SCENE_H_