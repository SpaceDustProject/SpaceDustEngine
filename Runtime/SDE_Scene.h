#ifndef _SDE_SCENE_H_
#define _SDE_SCENE_H_

#define SDE_TYPE_SCENE		"SDE_Scene"
#define SDE_TYPE_SCENEDEF	"SDE_SceneDef"

#include "SDE_LuaLightUserdata.h"

class SDE_MemoryPool;

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

};

class SDE_Scene : public SDE_LuaLightUserdata
{
public:
	const std::string&	GetName();

	SDE_Entity*			CreateEntity(const SDE_EntityDef& pDefEntity);
	bool				DestroyEntity(SDE_Entity* pEntity);

	SDE_System*			CreateSystem(const SDE_SystemDef& pDefSystem);
	void				DestroySystem(SDE_System* pSystem);

	SDE_MemoryPool*		GetMemoryPool();

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