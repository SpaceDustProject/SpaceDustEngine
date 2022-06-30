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
		std::list<SDE_Entity::Def*> listDefEntity;	// ��ʼ��ʵ���б�
		std::list<SDE_System::Def*> listDefSystem;	// ��ʼ��ϵͳ�б�
	};

	// �������º���
	bool Step();

	// ��ȡ��������
	const std::string&	GetType();
	// ��ȡ���������ĵ���
	SDE_Director*		GetDirector();

	// �����Ͷ�Ӧ�Ķ��崴��һ��ʵ�壬��������ָ��
	SDE_Entity*			CreateEntity(SDE_Entity::Def* pDefSystem);
	// �����Ͷ�Ӧ�Ķ��崴��һ��ϵͳ����������ָ��
	SDE_System*			CreateSystem(SDE_System::Def* pDefSystem);
	// �����Ͷ�Ӧ�Ķ��崴��һ���������������ָ��
	SDE_Component*		CreateComponent(SDE_Component::Def* pDefComponent);

	// ɾ��ָ��ʵ�壬�������Ƿ�ɹ�
	bool				DestroyEntity(SDE_Entity* pEntity);
	// ɾ��ָ��ϵͳ���������Ƿ�ɹ�
	bool				DestroySystem(SDE_System* pSystem);
	// ɾ��ָ��������������Ƿ�ɹ�
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