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
		std::initializer_list<SDE_Entity::Def> listDefEntity;	// ��ʼ��ʵ���б�
		std::initializer_list<SDE_System::Def> listDefSystem;	// ��ʼ��ϵͳ�б�
	};

	// �������º���
	bool Step();

	// ��ȡ���������ĵ���
	SDE_Director*	GetDirector();

	// �ö��崴��һ��ʵ�壬��������ָ��
	SDE_Entity*		CreateEntity(const SDE_Entity::Def& defEntity);
	// �ö��崴��һ��ϵͳ����������ָ��
	SDE_System*		CreateSystem(const SDE_System::Def& defSystem);
	// Ϊָ��ʵ�崴��һ�����
	SDE_Component*	CreateComponent(SDE_Entity* pEntity, const SDE_Component::Def& pComponent);

	// ɾ��ָ��ʵ�壬�������Ƿ�ɹ�
	bool			DestroyEntity(SDE_Entity* pEntity);
	// ɾ��ָ��ϵͳ���������Ƿ�ɹ�
	bool			DestroySystem(SDE_System* pSystem);
	// Ϊָ��ʵ��ɾ��������������Ƿ�ɹ�
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