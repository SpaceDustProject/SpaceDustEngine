#include "SDE_Director.h"

#include "SDE_LuaUtility.h"
#include "SDE_Blackboard.h"
#include "SDE_MemoryPool.h"

#include "SDE_Debug.h"

#include "SDE_Scene.h"
#include "SDE_System.h"

#include <chrono>
#include <thread>

#include <unordered_set>
#include <unordered_map>

class SDE_Director::Impl
{
public:
	bool			m_bIsRunning;
	SDE_Scene*		m_pSceneRunning;

	lua_State*		m_pState;
	SDE_MemoryPool	m_memoryPool;
	SDE_Blackboard	m_blackboard;

	std::unordered_map<std::string, SDE_Scene*> m_mapScenePreloaded;

public:
	Impl()
	{
		m_bIsRunning = false;
		m_pSceneRunning = nullptr;

		m_pState = luaL_newstate();
		luaL_openlibs(m_pState);
		lua_gc(m_pState, LUA_GCINC, 100);

		lua_newtable(m_pState);
		SDE_LuaUtility::SetPackage(m_pState, g_directorPackage);
		lua_setglobal(m_pState, SDE_NAME_DIRECTOR);

		// 注册 SceneDef 元表
		SDE_LuaUtility::RegisterMetatable(m_pState, g_metatableSceneDef);

		// 注册并设置 LightUserdata 共用元表
		lua_pushlightuserdata(m_pState, nullptr);
		luaL_newmetatable(m_pState, SDE_TYPE_LIGHTUSERDATA);
		{
			// 注册 Scene 函数
			SDE_LuaUtility::SetPackage(m_pState, g_metatableScene.GetPackage());
		}
		lua_setmetatable(m_pState, -2);
		lua_pop(m_pState, 1);

		// 搭建 SDE 全局环境
		lua_pushstring(m_pState, SDE_NAME_GLOBAL);
		lua_createtable(m_pState, 0, 3);
		{
			lua_pushstring(m_pState, SDE_NAME_RUNTIME);
			lua_newtable(m_pState);
			lua_rawset(m_pState, -3);

			lua_pushstring(m_pState, SDE_NAME_REGISTRY);
			lua_createtable(m_pState, 0, 2);
			{
				lua_pushstring(m_pState, SDE_TYPE_SYSTEMDEF);
				lua_newtable(m_pState);
				lua_rawset(m_pState, -3);
			}
			lua_rawset(m_pState, -3);

			lua_pushstring(m_pState, SDE_NAME_TEMPORARY);
			lua_createtable(m_pState, 0, 2);
			{
				lua_pushstring(m_pState, SDE_TYPE_SYSTEMDEF);
				lua_newtable(m_pState);
				lua_rawset(m_pState, -3);
			}
			lua_rawset(m_pState, -3);
		}
		lua_rawset(m_pState, LUA_REGISTRYINDEX);
	}
	~Impl()
	{
		if (m_pSceneRunning)
		{
			delete m_pSceneRunning;
		}

		lua_close(m_pState);
	}
};

bool SDE_Director::Run()
{
	float fFrameRate = 0.0f;

	if (!m_pImpl->m_blackboard.GetNumber("SDE_FRAME_RATE", fFrameRate) ||
		fFrameRate <= 0.0f)
	{
		SDE_Debug::Instance().OutputInfo(
			"Failed to run director: Invalid frame rate value.\n"
		);
		m_pImpl->m_bIsRunning = false;
		return false;
	}

	m_pImpl->m_bIsRunning = true;

	std::chrono::steady_clock::time_point t1;		// 单帧工作起始点
	std::chrono::steady_clock::time_point t2;		// 单帧工作结束点 & 单帧睡眠起始点
	std::chrono::steady_clock::time_point t3;		// 单帧睡眠结束点

	std::chrono::duration<double> dTargetTime(0.0);	// 单帧目标预期用时
	std::chrono::duration<double> dUsedTime(0.0);	// 单帧工作结束实际用时
	std::chrono::duration<double> dActualTime(0.0);	// 单帧工作+睡眠结束实际用时
	std::chrono::duration<double> dAdjustTime(0.0);	// 单帧睡眠调整用时
	std::chrono::duration<double> dSleepTime(0.0);	// 单帧睡眠预期用时

	while (m_pImpl->m_bIsRunning)
	{
		t1 = std::chrono::steady_clock::now();



		t2 = std::chrono::steady_clock::now();

		dUsedTime = t2 - t1;
		dTargetTime = std::chrono::duration<double>(1.0 / fFrameRate);
		dSleepTime = dTargetTime - dUsedTime + dAdjustTime;

		if (dSleepTime > std::chrono::duration<double>(0.0))
		{
			std::this_thread::sleep_for(dSleepTime);
		}

		t3 = std::chrono::steady_clock::now();
		dActualTime = t3 - t1;
		dAdjustTime = dAdjustTime * 0.9 + 0.1 * (dTargetTime - dActualTime);
	}

	return true;
}

void SDE_Director::Pause()
{
	m_pImpl->m_bIsRunning = false;
}

bool SDE_Director::RunScript(const SDE_Data& dataScript)
{
	return SDE_LuaUtility::RunScript(m_pImpl->m_pState, dataScript);
}

void SDE_Director::RunScene(const std::string& strName)
{
	std::unordered_map<std::string, SDE_Scene*>::iterator iterScene =
		m_pImpl->m_mapScenePreloaded.find(strName);

	if (iterScene == m_pImpl->m_mapScenePreloaded.end())
		return;

	m_pImpl->m_pSceneRunning = (*iterScene).second;
	m_pImpl->m_mapScenePreloaded.erase(iterScene);
}

void SDE_Director::RunScene(const SDE_SceneDef& defScene)
{
	m_pImpl->m_pSceneRunning = CreateScene(defScene);
}

SDE_Scene* SDE_Director::GetScene()
{
	return m_pImpl->m_pSceneRunning;
}

SDE_Scene* SDE_Director::CreateScene(const SDE_SceneDef& defScene)
{
	void* pMem = m_pImpl->m_memoryPool.Allocate(sizeof(SDE_SceneDef));
	SDE_Scene* pScene = new (pMem) SDE_Scene(defScene);

	// 在 Lua 中为该场景创建环境(主要储存连续的 Component)
	SDE_LuaUtility::GetRuntime(m_pImpl->m_pState);
	lua_pushstring(m_pImpl->m_pState, defScene.strName.c_str());
	lua_newtable(m_pImpl->m_pState);
	lua_rawset(m_pImpl->m_pState, -3);

	m_pImpl->m_mapScenePreloaded[defScene.strName] = pScene;
	return pScene;
}

SDE_MemoryPool* SDE_Director::GetMemoryPool()
{
	return &m_pImpl->m_memoryPool;
}

SDE_Blackboard* SDE_Director::GetBlackboard()
{
	return &m_pImpl->m_blackboard;
}

SDE_Director::SDE_Director()
{
	m_pImpl = new Impl();
}

SDE_Director::~SDE_Director()
{
	delete m_pImpl;
}

SDE_LUA_FUNC(SDE_Director_CreateSceneDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	switch (lua_gettop(pState))
	{
	case 3:
		if (!lua_istable(pState, 3))
		{
			SDE_Debug::Instance().OutputError(
				"The parameter on index 3 of this function should be a table.\n"
			);
		}

		lua_pushnil(pState);
		while (lua_next(pState, 3))
		{
			
		}

	case 2:
		if (!lua_istable(pState, 2))
		{
			SDE_Debug::Instance().OutputError(
				"The parameter on index 2 of this function should be a table.\n"
			);

			lua_pushnil(pState);
			while (lua_next(pState, 2))
			{

			}
		}

	}

	void* pLuaMem = lua_newuserdata(pState, sizeof(SDE_SceneDef));
	SDE_SceneDef* pDefScene = new (pLuaMem) SDE_SceneDef();

	return 1;
}

SDE_LUA_FUNC(SDE_Director_GetScene)
{
	lua_pushlightuserdata(pState, SDE_Director::Instance().GetScene());
	return 1;
}

SDE_LUA_FUNC(SDE_Director_CreateScene)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);
	lua_pushlightuserdata(pState, SDE_Director::Instance().CreateScene(*pDefScene));
	return 1;
}

SDE_LUA_FUNC(SDE_Director_CreateSystemDef)
{


	return 1;
}

SDE_LuaPackage g_directorPackage =
{
	{ "GetScene",			SDE_Director_GetScene },
	{ "CreateScene",		SDE_Director_CreateScene },
	{ "CreateSystemDef",	SDE_Director_CreateSystemDef }
};