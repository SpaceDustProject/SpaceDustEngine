#ifndef _SDE_SYSTEM_H_
#define _SDE_SYSTEM_H_

#define SDE_SYSTEM_NAME		"SDE_System"
#define SDE_SYSTEMDEF_NAME	"SDE_System_Def"

#include <lua.hpp>
#include <string>

class SDE_Scene;

class SDE_System
{
public:
	struct Def
	{
		std::string strName;
	};

	const std::string&	GetName();
	SDE_Scene*			GetScene();

	int					GetInitRef();
	int					GetUpdateRef();
	int					GetQuitRef();

private:
	std::string m_strName;		// System ������
	SDE_Scene*	m_pScene;		// System �����ĳ���

	int			m_nRefInit;		// System ��ʼ����������
	int			m_nRefUpdate;	// System ���º�������
	int			m_nRefQuit;		// System �˳���������

private:
	SDE_System(SDE_Scene* pScene, const SDE_System::Def& defSystem);
	~SDE_System();

	friend class SDE_Scene;
};

extern luaL_Reg g_funcSystemMember[];
extern luaL_Reg g_funcSystemDefMember[];

#endif // !_SDE_SYSTEM_H_