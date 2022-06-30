#ifndef _SDE_SCENE_H_
#define _SDE_SCENE_H_

#define SDE_SCENE_NAME		"SDE_Scene"
#define SDE_SCENEDEF_NAME	"SDE_Scene_Def"

#include "SDE_Entity.h"
#include "SDE_System.h"
#include "SDE_Component.h"

#include <functional>

class SDE_Director;

class SDE_Scene
{
public:
	struct Def
	{
		std::string					strType;
		std::list<SDE_Entity::Def*> listDefEntity;	// 初始化实体列表
		std::list<SDE_System::Def*> listDefSystem;	// 初始化系统列表
	};

	// 场景更新函数
	bool Step();

	// 获取场景名称
	const std::string&	GetType();
	// 获取场景所属的导演
	SDE_Director*		GetDirector();

	// 用类型对应的定义创建一个实体，并返回其指针
	SDE_Entity*			CreateEntity(SDE_Entity::Def* pDefSystem);
	// 用类型对应的定义创建一个系统，并返回其指针
	SDE_System*			CreateSystem(SDE_System::Def* pDefSystem);
	// 用类型对应的定义创建一个组件，并返回其指针
	SDE_Component*		CreateComponent(SDE_Component::Def* pDefComponent);

	// 删除指定实体，并返回是否成功
	bool				DestroyEntity(SDE_Entity* pEntity);
	// 删除指定系统，并返回是否成功
	bool				DestroySystem(SDE_System* pSystem);
	// 删除指定组件，并返回是否成功
	bool				DestroyComponent(SDE_Component* pComponent);

private:
	class Impl;
	Impl* m_pImpl;

private:
	SDE_Scene(SDE_Director* pDirector, Def* pDefScene);
	~SDE_Scene();

	friend class SDE_Director;
};

extern luaL_Reg g_funcSceneMember[];
extern luaL_Reg g_funcSceneDefMember[];

#endif // !_SDE_SCENE_H_