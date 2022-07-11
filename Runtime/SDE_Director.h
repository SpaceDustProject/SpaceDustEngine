#ifndef _SDE_DIRECTOR_H_
#define _SDE_DIRECTOR_H_

#define SDE_NAME_DIRECTOR "SDE_Director"

#include <string>

class SDE_Data;

struct SDE_LuaReg;
class SDE_LuaPackage;

class SDE_Scene;
struct SDE_SceneDef;

class SDE_MemoryPool;
class SDE_Blackboard;

class SDE_Director
{
public:
	bool				Run();										// 运行
	void				Pause();									// 停止

	bool				RunScript(const SDE_Data& dataScript);		// 运行脚本

	void				RunScene(const std::string& strName);		// 运行场景
	void				RunScene(const SDE_SceneDef& defScene);		// 创建并运行场景

	SDE_Scene*			GetScene();									// 获取当前正在运行的场景
	SDE_Scene*			CreateScene(const SDE_SceneDef& defScene);	// 创建场景
	void				SwitchScene(SDE_Scene* pScene);				// 切换当前场景

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

extern SDE_LuaPackage g_directorPackage;

#endif // !_SDE_DIRECTOR_H_