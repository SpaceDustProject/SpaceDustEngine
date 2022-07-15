#include "SDE_Director.h"

#include "SDE_LuaUtility.h"
#include "SDE_Blackboard.h"
#include "SDE_MemoryPool.h"

#include "SDE_Debug.h"

#include "SDE_Scene.h"
#include "SDE_System.h"
#include "SDE_Entity.h"
#include "SDE_Component.h"

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

	std::unordered_map<std::string, SDE_Scene*>		m_mapScenePreloaded;

public:
	Impl()
	{
		m_bIsRunning = false;
		m_pSceneRunning = nullptr;

		m_pState = luaL_newstate();
		luaL_openlibs(m_pState);
		lua_gc(m_pState, LUA_GCINC, 100);

		lua_newtable(m_pState);
		SDE_LuaUtility::SetPackage(m_pState, g_packageDirector);
		lua_setglobal(m_pState, SDE_NAME_DIRECTOR);

		// ע�� SceneDef Ԫ��
		SDE_LuaUtility::RegisterMetatable(m_pState, g_metatableSceneDef);
		// ע�� SystemDef Ԫ��
		SDE_LuaUtility::RegisterMetatable(m_pState, g_metatableSystemDef);
		// ע�� EntityDef Ԫ��
		SDE_LuaUtility::RegisterMetatable(m_pState, g_metatableEntityDef);
		// ע�� ComponentDef Ԫ��
		SDE_LuaUtility::RegisterMetatable(m_pState, g_metatableComponentDef);

		// ע�Ტ���� LightUserdata ����Ԫ��
		lua_pushlightuserdata(m_pState, nullptr);
		luaL_newmetatable(m_pState, SDE_TYPE_LIGHTUSERDATA);
		{
			lua_pushstring(m_pState, "__index");
			lua_createtable(m_pState, 0, 4);
			{
				// ע�� LightUserdata ����
				SDE_LuaUtility::SetPackage(m_pState, g_packageLightUserdata);
				// ע�� Scene ����
				SDE_LuaUtility::SetPackage(m_pState, g_packageScene);
				// ע�� System ����
				SDE_LuaUtility::SetPackage(m_pState, g_packageSystem);
				// ע�� Entity ����
				SDE_LuaUtility::SetPackage(m_pState, g_packageEntity);
				// ע�� Component ����
				SDE_LuaUtility::SetPackage(m_pState, g_packageComponent);
			}
			lua_rawset(m_pState, -3);

			// Component ��ֵ
			lua_pushstring(m_pState, "__newindex");
			lua_pushcfunction(m_pState,
				[](lua_State* pState)->int
				{
					SDE_Component* pComponent = (SDE_Component*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_COMPONENT);

					SDE_LuaUtility::GetRuntime(pState, pComponent->GetEntity()->GetScene()->GetName());
					lua_pushstring(pState, SDE_TYPE_COMPONENT);
					lua_rawget(pState, -2);
					lua_pushstring(pState, pComponent->GetName().c_str());
					lua_rawget(pState, -2);
					lua_rawgeti(pState, -1, pComponent->GetRef());

					lua_pushvalue(pState, 2);
					lua_pushvalue(pState, 3);
					lua_rawset(pState, -3);

					return 0;
				}
			);
			lua_rawset(m_pState, -3);
			
		}
		lua_setmetatable(m_pState, -2);
		lua_pop(m_pState, 1);

		// � SDE ȫ�ֻ���
		lua_pushstring(m_pState, SDE_NAME_GLOBAL);
		lua_createtable(m_pState, 0, 3);
		{
			lua_pushstring(m_pState, SDE_NAME_RUNTIME);
			lua_newtable(m_pState);
			lua_rawset(m_pState, -3);

			lua_pushstring(m_pState, SDE_NAME_REGISTRY);
			lua_createtable(m_pState, 0, 4);
			{
				lua_pushstring(m_pState, SDE_TYPE_SCENEDEF);
				lua_newtable(m_pState);
				lua_rawset(m_pState, -3);

				lua_pushstring(m_pState, SDE_TYPE_SYSTEMDEF);
				lua_newtable(m_pState);
				lua_rawset(m_pState, -3);

				lua_pushstring(m_pState, SDE_TYPE_ENTITYDEF);
				lua_newtable(m_pState);
				lua_rawset(m_pState, -3);

				lua_pushstring(m_pState, SDE_TYPE_COMPONENTDEF);
				lua_newtable(m_pState);
				lua_rawset(m_pState, -3);
			}
			lua_rawset(m_pState, -3);

			lua_pushstring(m_pState, SDE_NAME_TEMPORARY);
			lua_createtable(m_pState, 0, 4);
			{
				lua_pushstring(m_pState, SDE_TYPE_SCENEDEF);
				lua_newtable(m_pState);
				{
					// ʹ�ñ������ֵ
					lua_newtable(m_pState);
					{
						lua_pushstring(m_pState, "__mode");
						lua_pushstring(m_pState, "v");
						lua_rawset(m_pState, -3);
					}
					lua_setmetatable(m_pState, -2);
				}
				lua_rawset(m_pState, -3);

				lua_pushstring(m_pState, SDE_TYPE_SYSTEMDEF);
				lua_newtable(m_pState);
				{
					// ʹ�ñ������ֵ
					lua_newtable(m_pState);
					{
						lua_pushstring(m_pState, "__mode");
						lua_pushstring(m_pState, "v");
						lua_rawset(m_pState, -3);
					}
					lua_setmetatable(m_pState, -2);
				}
				lua_rawset(m_pState, -3);

				lua_pushstring(m_pState, SDE_TYPE_ENTITYDEF);
				lua_newtable(m_pState);
				{
					// ʹ�ñ������ֵ
					lua_newtable(m_pState);
					{
						lua_pushstring(m_pState, "__mode");
						lua_pushstring(m_pState, "v");
						lua_rawset(m_pState, -3);
					}
					lua_setmetatable(m_pState, -2);
				}
				lua_rawset(m_pState, -3);

				lua_pushstring(m_pState, SDE_TYPE_COMPONENTDEF);
				lua_newtable(m_pState);
				{
					// ʹ�ñ������ֵ
					lua_newtable(m_pState);
					{
						lua_pushstring(m_pState, "__mode");
						lua_pushstring(m_pState, "v");
						lua_rawset(m_pState, -3);
					}
					lua_setmetatable(m_pState, -2);
				}
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
			"Failed to run director: "
			"Invalid frame rate value.\n"
		);
		m_pImpl->m_bIsRunning = false;
		return false;
	}

	if (!m_pImpl->m_pSceneRunning)
	{
		SDE_Debug::Instance().OutputInfo(
			"Failed to run director: "
			"No running scene specified.\n"
		);
		m_pImpl->m_bIsRunning = false;
		return false;
	}

	m_pImpl->m_bIsRunning = true;

	std::chrono::steady_clock::time_point t1;		// ��֡������ʼ��
	std::chrono::steady_clock::time_point t2;		// ��֡���������� & ��֡˯����ʼ��
	std::chrono::steady_clock::time_point t3;		// ��֡˯�߽�����

	std::chrono::duration<double> dTargetTime(0.0);	// ��֡Ŀ��Ԥ����ʱ
	std::chrono::duration<double> dUsedTime(0.0);	// ��֡��������ʵ����ʱ
	std::chrono::duration<double> dActualTime(0.0);	// ��֡����+˯�߽���ʵ����ʱ
	std::chrono::duration<double> dAdjustTime(0.0);	// ��֡˯�ߵ�����ʱ
	std::chrono::duration<double> dSleepTime(0.0);	// ��֡˯��Ԥ����ʱ

	SDE_LuaUtility::GetRuntime(m_pImpl->m_pState, m_pImpl->m_pSceneRunning->GetName());

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

	m_pImpl->m_mapScenePreloaded[defScene.strName] = pScene;

	// �� Lua ��Ϊ�ó�����������
	SDE_LuaUtility::GetRuntime(m_pImpl->m_pState);
	lua_pushstring(m_pImpl->m_pState, defScene.strName.c_str());
	lua_newtable(m_pImpl->m_pState);
	{
		// ���ڴ������� System �ı�
		lua_pushstring(m_pImpl->m_pState, SDE_TYPE_SYSTEM);
		lua_newtable(m_pImpl->m_pState);
		lua_rawset(m_pImpl->m_pState, -3);

		// ���ڴ������� Component �ı�
		lua_pushstring(m_pImpl->m_pState, SDE_TYPE_COMPONENT);
		lua_newtable(m_pImpl->m_pState);
		lua_rawset(m_pImpl->m_pState, -3);
	}
	lua_rawset(m_pImpl->m_pState, -3);
	lua_pop(m_pImpl->m_pState, 1);

	return pScene;
}

void SDE_Director::DestroyScene(SDE_Scene* pScene)
{
	if (pScene == m_pImpl->m_pSceneRunning)
	{
		SDE_Debug::Instance().OutputInfo(
			"You can't destroy a scene that is running.\n"
		);
		return;
	}

	// ���������� Lua ������������
	SDE_LuaUtility::GetRuntime(m_pImpl->m_pState, pScene->GetName());
	lua_pushstring(m_pImpl->m_pState, pScene->GetName().c_str());
	lua_pushnil(m_pImpl->m_pState);
	lua_rawset(m_pImpl->m_pState, -3);

	m_pImpl->m_mapScenePreloaded.erase(pScene->GetName());
	pScene->~SDE_Scene();
	m_pImpl->m_memoryPool.Free(pScene, sizeof(SDE_Scene));
}

void SDE_Director::SwitchScene(SDE_Scene* pScene)
{
	if (m_pImpl->m_pSceneRunning)
	{
		m_pImpl->m_pSceneRunning->~SDE_Scene();
		m_pImpl->m_memoryPool.Free(m_pImpl->m_pSceneRunning, sizeof(SDE_Scene));
	}
	m_pImpl->m_pSceneRunning = pScene;
}

lua_State* SDE_Director::GetLuaState()
{
	return m_pImpl->m_pState;
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

SDE_LUA_FUNC(SDE_Director_GetScene)
{
	lua_pushlightuserdata(pState, SDE_Director::Instance().GetScene());
	return 1;
}

SDE_LUA_FUNC(SDE_Director_CreateScene)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)SDE_LuaUtility::GetLightUserdataDef(pState, 1, SDE_TYPE_SCENEDEF);

	if (!pDefScene) return 1;
	SDE_Scene* pScene = SDE_Director::Instance().CreateScene(*pDefScene);

	// ���ݶ�ջ����������캯����������
	lua_pushlightuserdata(pState, pScene);
	
	if (lua_gettop(pState) >= 4)
	{
		lua_pushvalue(pState, 2);
		lua_pcall(pState, 2, 0, 0);
	}
	else lua_pcall(pState, 1, 0, 0);

	lua_pushlightuserdata(pState, pScene);
	return 1;
}

SDE_LUA_FUNC(SDE_Director_SwitchScene)
{
	SDE_Scene* pScene = (SDE_Scene*)SDE_LuaUtility::GetLightUserdata(pState, 1, SDE_TYPE_SCENE);
	SDE_Director::Instance().SwitchScene(pScene);
	return 0;
}

SDE_LUA_FUNC(SDE_Director_CreateSceneDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_SCENEDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_pushvalue(pState, 2);
	lua_rawset(pState, -3);

	void* pLuaMem = lua_newuserdata(pState, sizeof(SDE_SceneDef));
	SDE_SceneDef* pDefScene = new (pLuaMem) SDE_SceneDef(strName);
	luaL_setmetatable(pState, SDE_TYPE_SCENEDEF);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_RegisterSceneDef)
{
	SDE_SceneDef* pDefScene = (SDE_SceneDef*)luaL_checkudata(pState, 1, SDE_TYPE_SCENEDEF);

	if (pDefScene->bIsRegistered)
		return 0;

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_SCENEDEF);
	lua_rawget(pState, -2);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_SCENEDEF);
	lua_rawget(pState, -2);

	lua_pushstring(pState, pDefScene->strName.c_str());
	lua_createtable(pState, 0, 2);
	{
		lua_pushstring(pState, "userdata");
		lua_pushvalue(pState, 1);
		lua_rawset(pState, -3);

		lua_pushstring(pState, "constructor");
		lua_pushstring(pState, pDefScene->strName.c_str());
		lua_rawget(pState, -7);
		lua_rawset(pState, -3);

		// ��� Temporary �е� constructor
		lua_pushstring(pState, pDefScene->strName.c_str());
		lua_pushnil(pState);
		lua_rawset(pState, -7);
	}
	lua_rawset(pState, -3);

	pDefScene->bIsRegistered = true;

	return 0;
}

SDE_LUA_FUNC(SDE_Director_GetSceneDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_SCENEDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_rawget(pState, -2);
	lua_pushstring(pState, "userdata");
	lua_rawget(pState, -2);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_CreateSystemDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_pushvalue(pState, 2);
	lua_rawset(pState, -3);

	void* pLuaMem = lua_newuserdata(pState, sizeof(SDE_SystemDef));
	SDE_SystemDef* pSystemDef = new (pLuaMem) SDE_SystemDef{ strName };
	luaL_setmetatable(pState, SDE_TYPE_SYSTEMDEF);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_RegisterSystemDef)
{
	SDE_SystemDef* pDefSystem = (SDE_SystemDef*)luaL_checkudata(pState, 1, SDE_TYPE_SYSTEMDEF);

	if (pDefSystem->bIsRegistered)
		return 0;

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
	lua_rawget(pState, -2);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
	lua_rawget(pState, -2);

	lua_pushstring(pState, pDefSystem->strName.c_str());
	lua_createtable(pState, 0, 2);
	{
		lua_pushstring(pState, "userdata");
		lua_pushvalue(pState, 1);
		lua_rawset(pState, -3);

		lua_pushstring(pState, "constructor");
		lua_pushstring(pState, pDefSystem->strName.c_str());
		lua_rawget(pState, -7);
		lua_rawset(pState, -3);

		// ��� Temporary �е� constructor
		lua_pushstring(pState, pDefSystem->strName.c_str());
		lua_pushnil(pState);
		lua_rawset(pState, -7);
	}
	lua_rawset(pState, -3);

	pDefSystem->bIsRegistered = true;

	return 0;
}

SDE_LUA_FUNC(SDE_Director_GetSystemDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_SYSTEMDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_rawget(pState, -2);
	lua_pushstring(pState, "userdata");
	lua_rawget(pState, -2);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_CreateEntityDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_ENTITYDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_pushvalue(pState, 2);
	lua_rawset(pState, -3);

	void* pLuaMem = lua_newuserdata(pState, sizeof(SDE_EntityDef));
	SDE_EntityDef* pEntityDef = new (pLuaMem) SDE_EntityDef(strName);
	luaL_setmetatable(pState, SDE_TYPE_ENTITYDEF);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_RegisterEntityDef)
{
	SDE_EntityDef* pDefEntity = (SDE_EntityDef*)luaL_checkudata(pState, 1, SDE_TYPE_ENTITYDEF);

	if (pDefEntity->bIsRegistered)
		return 0;

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_ENTITYDEF);
	lua_rawget(pState, -2);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_ENTITYDEF);
	lua_rawget(pState, -2);

	lua_pushstring(pState, pDefEntity->strName.c_str());
	lua_createtable(pState, 0, 2);
	{
		lua_pushstring(pState, "userdata");
		lua_pushvalue(pState, 1);
		lua_rawset(pState, -3);

		lua_pushstring(pState, "constructor");
		lua_pushstring(pState, pDefEntity->strName.c_str());
		lua_rawget(pState, -7);
		lua_rawset(pState, -3);

		// ��� Temporary �е� constructor
		lua_pushstring(pState, pDefEntity->strName.c_str());
		lua_pushnil(pState);
		lua_rawset(pState, -7);
	}
	lua_rawset(pState, -3);

	pDefEntity->bIsRegistered = true;

	return 0;
}

SDE_LUA_FUNC(SDE_Director_GetEntityDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_ENTITYDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_rawget(pState, -2);
	lua_pushstring(pState, "userdata");
	lua_rawget(pState, -2);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_CreateComponentDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_pushvalue(pState, 2);
	lua_rawset(pState, -3);

	void* pLuaMem = lua_newuserdata(pState, sizeof(SDE_ComponentDef));
	SDE_ComponentDef* pComponentDef = new (pLuaMem) SDE_ComponentDef(strName);
	luaL_setmetatable(pState, SDE_TYPE_COMPONENTDEF);

	return 1;
}

SDE_LUA_FUNC(SDE_Director_RegisterComponentDef)
{
	SDE_ComponentDef* pDefComponent = (SDE_ComponentDef*)luaL_checkudata(pState, 1, SDE_TYPE_COMPONENTDEF);

	if (pDefComponent->bIsRegistered)
		return 0;

	SDE_LuaUtility::GetTemporary(pState);
	lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
	lua_rawget(pState, -2);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
	lua_rawget(pState, -2);

	lua_pushstring(pState, pDefComponent->strName.c_str());
	lua_createtable(pState, 0, 2);
	{
		lua_pushstring(pState, "userdata");
		lua_pushvalue(pState, 1);
		lua_rawset(pState, -3);

		lua_pushstring(pState, "constructor");
		lua_pushstring(pState, pDefComponent->strName.c_str());
		lua_rawget(pState, -7);
		lua_rawset(pState, -3);

		// ��� Temporary �е� constructor
		lua_pushstring(pState, pDefComponent->strName.c_str());
		lua_pushnil(pState);
		lua_rawset(pState, -7);
	}
	lua_rawset(pState, -3);

	pDefComponent->bIsRegistered = true;

	return 0;
}

SDE_LUA_FUNC(SDE_Director_GetComponentDef)
{
	std::string strName = luaL_checkstring(pState, 1);

	SDE_LuaUtility::GetRegistry(pState);
	lua_pushstring(pState, SDE_TYPE_COMPONENTDEF);
	lua_rawget(pState, -2);
	lua_pushstring(pState, strName.c_str());
	lua_rawget(pState, -2);
	lua_pushstring(pState, "userdata");
	lua_rawget(pState, -2);

	return 1;
}

SDE_LuaPackage g_packageDirector =
{
	{ "GetScene",				SDE_Director_GetScene },
	{ "CreateScene",			SDE_Director_CreateScene },
	{ "SwitchScene",			SDE_Director_SwitchScene },

	{ "CreateSceneDef",			SDE_Director_CreateSceneDef },
	{ "RegisterSceneDef",		SDE_Director_RegisterSceneDef },
	{ "GetSceneDef",			SDE_Director_GetSceneDef },

	{ "CreateSystemDef",		SDE_Director_CreateSystemDef },
	{ "RegisterSystemDef",		SDE_Director_RegisterSystemDef },
	{ "GetSystemDef",			SDE_Director_GetSystemDef },

	{ "CreateEntityDef",		SDE_Director_CreateEntityDef },
	{ "RegisterEntityDef",		SDE_Director_RegisterEntityDef },
	{ "GetEntityDef",			SDE_Director_GetEntityDef },

	{ "CreateComponentDef",		SDE_Director_CreateComponentDef },
	{ "RegisterComponentDef",	SDE_Director_RegisterComponentDef },
	{ "GetComponentDef",		SDE_Director_GetComponentDef }
};

SDE_LUA_FUNC(SDE_LightUserdata_GetType)
{
	SDE_LuaLightUserdata* pLightUserdata = (SDE_LuaLightUserdata*)luaL_checkudata(pState, 1, SDE_TYPE_LIGHTUSERDATA);
	lua_pushstring(pState, pLightUserdata->GetTypeLU().c_str());
	return 1;
}

SDE_LUA_FUNC(SDE_LightUserdata_GetName)
{
	SDE_LuaLightUserdata* pLightUserdata = (SDE_LuaLightUserdata*)luaL_checkudata(pState, 1, SDE_TYPE_LIGHTUSERDATA);
	lua_pushstring(pState, pLightUserdata->GetName().c_str());
	return 1;
}

SDE_LuaPackage g_packageLightUserdata =
{
	{ "GetType", SDE_LightUserdata_GetType },
	{ "GetName", SDE_LightUserdata_GetName }
};