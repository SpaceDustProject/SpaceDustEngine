#ifndef _SDE_DIRECTOR_H_
#define _SDE_DIRECTOR_H_

#define SDE_NAME_DIRECTOR		"SDE_Director"
#define SDE_NAME_FUNCAFTERSLEEP	"SDE_FuncAfterSleep"

#include <string>

class SDE_Data;

struct lua_State;
struct SDE_LuaReg;
class SDE_LuaPackage;

class SDE_Scene;
struct SDE_SceneDef;

class SDE_MemoryPool;
class SDE_Blackboard;

class SDE_Director
{
public:
	bool				Run();										// ����
	void				Pause();									// ֹͣ

	bool				RunScript(const SDE_Data& dataScript);		// ���нű�

	SDE_Scene*			CreateScene(const SDE_SceneDef& defScene);	// ���ݶ��崴������
	void				DestroyScene(SDE_Scene* pScene);			// �ݻ�ָ������

	void				RunScene(const SDE_SceneDef& defScene);		// ���������г���

	void				PreloadScene(const SDE_SceneDef& defScene);	// ���ݶ���Ԥ��������
	void				RunScene(const std::string& strName);		// ���г���
	SDE_Scene*			GetScene(const std::string& strName);		// ��ȡԤ���س���

	SDE_Scene*			GetRunningScene();							// ��ȡ��ǰ�������еĳ���

	lua_State*			GetLuaState();
	SDE_MemoryPool*		GetMemoryPool();
	SDE_Blackboard*		GetBlackboard();

private:
	class Impl;
	Impl* m_pImpl;

public:
	~SDE_Director();
	SDE_Director(const SDE_Director&) = delete;
	SDE_Director& operator=(const SDE_Director&) = delete;
	static SDE_Director& Instance()
	{
		static SDE_Director instance;
		return instance;
	}

private:
	SDE_Director();
};

extern SDE_LuaPackage g_packageDirector;

#endif // !_SDE_DIRECTOR_H_