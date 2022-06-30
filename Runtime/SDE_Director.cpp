#include "SDE_Director.h"

#include "SDE_VirtualMachine.h"
#include "SDE_Blackboard.h"
#include "SDE_MemoryPool.h"
#include "SDE_Debug.h"

#include "SDE_Timer.h"

#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>

SDE_Director g_director;

class SDE_Director::Impl
{
public:
	SDE_MemoryPool*		m_pMemoryPool;											// 内部内存池
	lua_State*			m_pLuaVM;												// 内部 Lua 虚拟机

	bool				m_isQuit = false;										// 是否退出
	SDE_Scene*			m_pRunningScene = nullptr;								// 当前正在运行的场景

	std::unordered_map<std::string, SDE_Entity::Def*>		m_mapEntityDef;		// 已注册的实体
	std::unordered_map<std::string, SDE_Component::Def*>	m_mapComponentDef;	// 已注册的组件
	std::unordered_map<std::string, SDE_System::Def*>		m_mapSystemDef;		// 已注册的系统
	std::unordered_map<std::string, SDE_Scene::Def*>		m_mapSceneDef;		// 已注册的场景

	std::unordered_map<lua_Integer, SDE_Timer*> m_mapTimer;

public:
	Impl()
	{
		// 初始内存池
		m_pMemoryPool = new SDE_MemoryPool();

		// 初始化 Lua 虚拟机
		m_pLuaVM = luaL_newstate();
		luaL_openlibs(m_pLuaVM);
		lua_gc(m_pLuaVM, LUA_GCINC, 100);

		// 注册 SDE 全局环境表
		lua_createtable(m_pLuaVM, 0, 2);
		{
			// 新建 SDE 注册表
			lua_createtable(m_pLuaVM, 0, 4);
			{
				// 创建实体注册表
				lua_newtable(m_pLuaVM);
				lua_setfield(m_pLuaVM, -2, "Entity");

				// 创建组件注册表
				lua_newtable(m_pLuaVM);
				lua_setfield(m_pLuaVM, -2, "Component");

				// 创建系统注册表
				lua_newtable(m_pLuaVM);
				lua_setfield(m_pLuaVM, -2, "System");

				// 创建场景注册表
				lua_newtable(m_pLuaVM);
				lua_setfield(m_pLuaVM, -2, "Scene");
			}
			lua_setfield(m_pLuaVM, -2, "Registry");

			// 新建 SDE 运行环境
			lua_createtable(m_pLuaVM, 0, 2);
			{
				lua_newtable(m_pLuaVM);
				lua_setfield(m_pLuaVM, -2, "Component");

				lua_newtable(m_pLuaVM);
				{
					lua_newtable(m_pLuaVM);
					lua_setfield(m_pLuaVM, -2, "OnInit");

					lua_newtable(m_pLuaVM);
					lua_setfield(m_pLuaVM, -2, "OnUpdate");

					lua_newtable(m_pLuaVM);
					lua_setfield(m_pLuaVM, -2, "OnQuit");
				}
				lua_setfield(m_pLuaVM, -2, "System");
			}
			lua_setfield(m_pLuaVM, -2, "Runtime");
		}
		lua_setfield(m_pLuaVM, LUA_REGISTRYINDEX, "SDE_Global");

		// 注册 Entity 元表
		luaL_newmetatable(m_pLuaVM, SDE_ENTITY_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcEntityMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 EntityDef 元表
		luaL_newmetatable(m_pLuaVM, SDE_ENTITYDEF_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcEntityDefMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 Component 元表
		luaL_newmetatable(m_pLuaVM, SDE_COMPONENT_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcComponentMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 ComponentDef 元表
		luaL_newmetatable(m_pLuaVM, SDE_COMPONENTDEF_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcComponentDefMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 System 元表
		luaL_newmetatable(m_pLuaVM, SDE_SYSTEM_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcSystemMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 SystemDef 元表
		luaL_newmetatable(m_pLuaVM, SDE_SYSTEMDEF_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcSystemDefMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 Scene 元表
		luaL_newmetatable(m_pLuaVM, SDE_SCENE_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcSceneMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 SceneDef 元表
		luaL_newmetatable(m_pLuaVM, SDE_SCENEDEF_NAME);
		{
			lua_newtable(m_pLuaVM);
			luaL_setfuncs(m_pLuaVM, g_funcSceneDefMember, 0);
			lua_setfield(m_pLuaVM, -2, "__index");
		}
		lua_pop(m_pLuaVM, 1);

		// 注册 SDE_Director
		lua_newtable(m_pLuaVM);
		luaL_setfuncs(m_pLuaVM, g_funcDirectorMember, 0);
		lua_setglobal(m_pLuaVM, "SDE_Director");
	}
	~Impl()
	{
		// 析构场景
		if (m_pRunningScene)
		{
			m_pRunningScene->~SDE_Scene();
			m_pMemoryPool->Free(m_pRunningScene, sizeof(SDE_Scene));
		}

		// 关闭虚拟机
		lua_close(m_pLuaVM);

		// 释放内存池
		delete m_pMemoryPool;
	}
};

bool SDE_Director::Run()
{
	if (!m_pImpl->m_pRunningScene)
	{
		SDE_Debug::Instance().OutputLog(
			"Director Running Error: "
			"No running scene specified.\n"
		);
		return false;
	}

	std::chrono::steady_clock::time_point t1;		// 单帧工作起始点
	std::chrono::steady_clock::time_point t2;		// 单帧工作结束点 & 单帧睡眠起始点
	std::chrono::steady_clock::time_point t3;		// 单帧睡眠结束点

	std::chrono::duration<double> dTargetTime(0.0);	// 单帧目标预期用时
	std::chrono::duration<double> dUsedTime(0.0);	// 单帧工作结束实际用时
	std::chrono::duration<double> dActualTime(0.0);	// 单帧工作+睡眠结束实际用时
	std::chrono::duration<double> dAdjustTime(0.0);	// 单帧睡眠调整用时
	std::chrono::duration<double> dSleepTime(0.0);	// 单帧睡眠预期用时

	float fFrameRate = 0.0f;						// 当前帧率

	while (!m_pImpl->m_isQuit)
	{
		t1 = std::chrono::steady_clock::now();

		m_pImpl->m_pRunningScene->Step();

		if (!SDE_Blackboard::Instance().GetNumber("SDE_FRAME_RATE", fFrameRate) ||
			fFrameRate <= 0.0f)
		{
			SDE_Debug::Instance().OutputLog(
				"Director Running Error: "
				"Invalid frame rate value.\n"
			);
			return false;
		}

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

lua_State* SDE_Director::GetLuaVM()
{
	return m_pImpl->m_pLuaVM;
}

SDE_MemoryPool* SDE_Director::GetMemoryPool()
{
	return m_pImpl->m_pMemoryPool;
}

SDE_Scene* SDE_Director::GetScene()
{
	return m_pImpl->m_pRunningScene;
}

SDE_Scene* SDE_Director::SwitchScene(SDE_Scene::Def* pDefScene)
{
	if (m_pImpl->m_pRunningScene)
	{
		m_pImpl->m_pRunningScene->~SDE_Scene();
		m_pImpl->m_pMemoryPool->Free(m_pImpl->m_pRunningScene, sizeof(SDE_Scene));
	}

	void* pMem = m_pImpl->m_pMemoryPool->Allocate(sizeof(SDE_Scene));
	SDE_Scene* pScene = new (pMem) SDE_Scene(this, pDefScene);

	m_pImpl->m_pRunningScene = pScene;
	return pScene;
}

SDE_Entity::Def* SDE_Director::GetEntityDef(const std::string& strType)
{
	return m_pImpl->m_mapEntityDef[strType];
}

SDE_Entity::Def* SDE_Director::CreateEntityDef(const std::string& strType)
{
	return nullptr;
}

bool SDE_Director::DestroyEntityDef(const std::string& strType)
{
	if (m_pImpl->m_mapEntityDef.find(strType) != m_pImpl->m_mapEntityDef.end())
	{
		m_pImpl->m_mapEntityDef.erase(strType);
		return true;
	}
	return false;
}

SDE_Component::Def* SDE_Director::GetComponentDef(const std::string& strType)
{
	return m_pImpl->m_mapComponentDef[strType];
}

SDE_Component::Def* SDE_Director::CreateComponentDef(const std::string& strType)
{
	return nullptr;
}

bool SDE_Director::DestroyComponentDef(const std::string& strType)
{
	if (m_pImpl->m_mapComponentDef.find(strType) != m_pImpl->m_mapComponentDef.end())
	{
		m_pImpl->m_mapComponentDef.erase(strType);
		return true;
	}
	return false;
}

SDE_System::Def* SDE_Director::GetSystemDef(const std::string& strType)
{
	return m_pImpl->m_mapSystemDef[strType];
}

SDE_System::Def* SDE_Director::CreateSystemDef(const std::string& strType)
{
	void* pMem = m_pImpl->m_pMemoryPool->Allocate(sizeof(SDE_System::Def));
	SDE_System::Def* pDefSystem = new (pMem) SDE_System::Def({ strType });
	m_pImpl->m_mapSystemDef[strType] = pDefSystem;
	return pDefSystem;
}

bool SDE_Director::DestroySystemDef(const std::string& strType)
{
	std::unordered_map<std::string, SDE_System::Def*>::iterator iterSystemDef =
		m_pImpl->m_mapSystemDef.begin();

	if (iterSystemDef != m_pImpl->m_mapSystemDef.end())
	{
		(*iterSystemDef).second->~Def();
		m_pImpl->m_pMemoryPool->Free((*iterSystemDef).second, sizeof(SDE_System::Def));

		m_pImpl->m_mapSystemDef.erase(iterSystemDef);
		return true;
	}
	return false;
}

SDE_Scene::Def* SDE_Director::GetSceneDef(const std::string& strType)
{
	return m_pImpl->m_mapSceneDef[strType];
}

SDE_Scene::Def* SDE_Director::CreateSceneDef(const std::string& strType)
{
	void* pMem = m_pImpl->m_pMemoryPool->Allocate(sizeof(SDE_Scene::Def));
	SDE_Scene::Def* pDefScene = new (pMem) SDE_Scene::Def({ strType });
	m_pImpl->m_mapSceneDef[strType] = pDefScene;
	return pDefScene;
}

bool SDE_Director::DestroySceneDef(const std::string& strType)
{
	std::unordered_map<std::string, SDE_Scene::Def*>::iterator iterSceneDef =
		m_pImpl->m_mapSceneDef.find(strType);

	if (iterSceneDef != m_pImpl->m_mapSceneDef.end())
	{
		(*iterSceneDef).second->~Def();
		m_pImpl->m_pMemoryPool->Free((*iterSceneDef).second, sizeof(SDE_Scene::Def));

		m_pImpl->m_mapSceneDef.erase(iterSceneDef);
		return true;
	}
	return false;
}

SDE_Director::SDE_Director()
{
	m_pImpl = new Impl();
}

SDE_Director::~SDE_Director()
{
	delete m_pImpl;
}

SDE_API SDE_Director_GetEntityDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushlightuserdata(pState, g_director.GetEntityDef(strType));
	luaL_setmetatable(pState, SDE_ENTITYDEF_NAME);
	return 1;
}

SDE_API SDE_Director_CreateEntityDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushlightuserdata(pState, g_director.CreateEntityDef(strType));
	luaL_setmetatable(pState, SDE_ENTITYDEF_NAME);
	return 1;
}

SDE_API SDE_Director_DestroyEntityDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushboolean(pState, g_director.DestroyEntityDef(strType));
	return 1;
}

SDE_API SDE_Director_GetComponentDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushlightuserdata(pState, g_director.GetComponentDef(strType));
	luaL_setmetatable(pState, SDE_COMPONENTDEF_NAME);
	return 1;
}

SDE_API SDE_Director_CreateComponentDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushlightuserdata(pState, g_director.CreateComponentDef(strType));
	luaL_setmetatable(pState, SDE_COMPONENTDEF_NAME);
	return 1;
}

SDE_API SDE_Director_DestroyComponentDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushboolean(pState, g_director.DestroyComponentDef(strType));
	return 1;
}

SDE_API SDE_Director_GetSystemDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushlightuserdata(pState, g_director.GetSystemDef(strType));
	return 1;
}

SDE_API SDE_Director_CreateSystemDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);

	// 在 SDE_Registry System 中为其创建空表
	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_createtable(pState, 0, 3);
	lua_setfield(pState, -2, strType.c_str());

	lua_pushlightuserdata(pState, g_director.CreateSystemDef(strType));
	luaL_setmetatable(pState, SDE_SYSTEMDEF_NAME);

	return 1;
}

SDE_API SDE_Director_DestroySystemDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);

	// 在 SDE_Registry System 中删除该表
	SDE_VirtualMachine::Instance().GetGlobal(pState);
	lua_getfield(pState, -1, "Registry");
	lua_getfield(pState, -1, "System");
	lua_pushnil(pState);
	lua_setfield(pState, -2, strType.c_str());

	lua_pushboolean(pState, g_director.DestroySystemDef(strType));

	return 1;
}

SDE_API SDE_Director_GetSceneDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushlightuserdata(pState, g_director.GetSceneDef(strType));
	return 1;
}

SDE_API SDE_Director_CreateSceneDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);

	SDE_Scene::Def* pDefScene = g_director.CreateSceneDef(strType);
	if (pDefScene)
	{
		lua_pushlightuserdata(pState, pDefScene);
		luaL_setmetatable(pState, SDE_SCENEDEF_NAME);
	}
	else lua_pushnil(pState);

	return 1;
}

SDE_API SDE_Director_DestroySceneDef(lua_State* pState)
{
	std::string strType = luaL_checkstring(pState, 1);
	lua_pushboolean(pState, g_director.DestroySceneDef(strType));
	return 1;
}

SDE_API SDE_Director_GetScene(lua_State* pState)
{
	SDE_Scene* pScene = g_director.GetScene();

	if (pScene)
	{
		lua_pushlightuserdata(pState, g_director.GetScene());
		luaL_setmetatable(pState, SDE_SCENE_NAME);
	}
	else lua_pushnil(pState);

	return 1;
}

SDE_API SDE_Director_SwitchScene(lua_State* pState)
{
	SDE_Scene::Def* pDefScene = (SDE_Scene::Def*)luaL_checkudata(pState, 1, SDE_SCENEDEF_NAME);
	lua_pushlightuserdata(pState, g_director.SwitchScene(pDefScene));
	luaL_setmetatable(pState, SDE_SCENE_NAME);
	return 1;
}

luaL_Reg g_funcDirectorMember[] =
{
	{ "GetScene",				SDE_Director_GetScene },
	{ "SwitchScene",			SDE_Director_SwitchScene },

	{ "GetEntityDef",			SDE_Director_GetEntityDef },
	{ "CreateEntityDef",		SDE_Director_CreateEntityDef },
	{ "DestroyEntityDef",		SDE_Director_DestroyEntityDef },

	{ "GetComponentDef",		SDE_Director_GetComponentDef },
	{ "CreateComponentDef",		SDE_Director_CreateComponentDef },
	{ "DestroyComponentDef",	SDE_Director_DestroyComponentDef },

	{ "GetSystemDef",			SDE_Director_GetSystemDef },
	{ "CreateSystemDef",		SDE_Director_CreateSystemDef },
	{ "DestroySystemDef",		SDE_Director_DestroySystemDef },

	{ "GetSceneDef",			SDE_Director_GetSceneDef },
	{ "CreateSceneDef",			SDE_Director_CreateSceneDef },
	{ "DestroySceneDef",		SDE_Director_DestroySceneDef },

	{ nullptr,					nullptr }
};