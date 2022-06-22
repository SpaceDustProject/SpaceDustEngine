#ifndef _SDE_SCENE_H_
#define _SDE_SCENE_H_

#define SDE_SCENE_NAME		"SDE_Scene"
#define SDE_SCENEDEF_NAME	"SDE_Scene_Def"

#include "SDE_Entity.h"
#include "SDE_System.h"
#include "SDE_Component.h"

#include <initializer_list>

class SDE_Director;

class SDE_Scene
{
public:
	struct Def
	{
		std::initializer_list<SDE_Entity::Def> listDefEntity;	// 初始化实体列表
		std::initializer_list<SDE_System::Def> listDefSystem;	// 初始化系统列表
	};

	// 场景更新函数
	bool Step();

	// 获取场景所属的导演
	SDE_Director*	GetDirector();

	// 用定义创建一个实体，并返回其指针
	SDE_Entity*		CreateEntity(const SDE_Entity::Def& defEntity);
	// 用定义创建一个系统，并返回其指针
	SDE_System*		CreateSystem(const SDE_System::Def& defSystem);
	// 为指定实体创建一个组件
	SDE_Component*	CreateComponent(SDE_Entity* pEntity, const SDE_Component::Def& pComponent);

	// 删除指定实体，并返回是否成功
	bool			DestroyEntity(SDE_Entity* pEntity);
	// 删除指定系统，并返回是否成功
	bool			DestroySystem(SDE_System* pSystem);
	// 为指定实体删除组件，并返回是否成功
	bool			DestroyComponent(SDE_Component* pComponent);

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Scene(SDE_Director* pDirector, const Def& defScene);
	~SDE_Scene();

	friend class SDE_Director;
};

extern luaL_Reg g_funcSceneMember[];
extern luaL_Reg g_funcSceneDefMember[];

#endif // !_SDE_SCENE_H_