#include "GameFactory.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"
#include "DebugModule.h"

#include "GameEntity.h"
#include "GameScene.h"

#include <new>
#include <functional>

class GameFactory::Impl
{
public:
	static int CreateEntityDef(lua_State* pLuaVM)
	{
		lua_createtable(pLuaVM, 0, 2);

		// SDE_EntityDef AddComponent ����
		lua_pushstring(pLuaVM, "AddComponent");
		lua_pushcfunction(pLuaVM,
			[](lua_State* pLuaVM)->int
			{
				// ��ע����л�ȡ���
				lua_getglobal(pLuaVM, "SDE_Registry");
				lua_pushvalue(pLuaVM, -3);
				lua_rawget(pLuaVM, -2);

				// ���û�л�ȡ������ͷ���
				if (lua_isnil(pLuaVM, -1))
				{
					DebugModule::Instance().Output(
						"Error Adding: There is no component named %s.\n",
						lua_tostring(pLuaVM, 2)
					);
					return 0;
				}

				lua_remove(pLuaVM, -2);	// ����ע���
				VirtualMachine::CopyTable(pLuaVM, -1);
				lua_remove(pLuaVM, -2);	// �����������ı�

				// ��ʼ�����û��ṩ�ĳ�ʼ�����ݣ����丳ֵ�����
				lua_pushnil(pLuaVM);
				while (lua_next(pLuaVM, -3))
				{
					lua_pushvalue(pLuaVM, -2);
					
					// ��������û�и�ֵ���򲻽��и�ֵ
					lua_rawget(pLuaVM, -4);
					if (lua_isnil(pLuaVM, -1))
					{
						DebugModule::Instance().Output(
							"Error Adding: This component has no %s value.\n",
							lua_tostring(pLuaVM, -2)
						);
						lua_pop(pLuaVM, 3);
						continue;
					}

					// ��������г�ʼ����ֵ
					lua_copy(pLuaVM, -3, -1);
					lua_rawset(pLuaVM, -5);
					lua_pop(pLuaVM, 1);
				}

				// ��������뵽ʵ����ȥ
				lua_remove(pLuaVM, -2);
				lua_rawset(pLuaVM, -3);

				return 0;
			}
		);
		lua_rawset(pLuaVM, -3);

		// SDE_EntityDef DeleteComponent ����
		lua_pushstring(pLuaVM, "DeleteComponent");
		lua_pushcfunction(pLuaVM,
			[](lua_State* pLuaVM)->int
			{
				lua_pushvalue(pLuaVM, -1);
				lua_rawget(pLuaVM, -2);

				// �����ʵ��ӵ����� Component
				// ��������Ϊ nil
				if (!lua_isnil(pLuaVM, -1))
				{
					lua_pop(pLuaVM, 1);
					lua_pushnil(pLuaVM);
					lua_rawset(pLuaVM, -3);
					return 0;
				}

				DebugModule::Instance().Output(
					"Error Deletion: "
					"This table doesn't have %s component.\n",
					lua_tostring(pLuaVM, -2)
				);
				return 0;
			}
		);
		lua_rawset(pLuaVM, -3);

		// SDE_EntityDef GetComponent ����
		lua_pushstring(pLuaVM, "GetComponent");
		lua_pushcfunction(pLuaVM,
			[](lua_State* pLuaVM)->int
			{

			}
		);

		return 1;
	}

	static int CreateSceneDef(lua_State* pLuaVM)
	{
		lua_createtable(pLuaVM, 0, 2);
		return 1;
	}

public:
	Impl()
	{
		VirtualMachine::AddPreloadFunc(

			VirtualMachine::Instance().GetLuaState(),

			"SDE_Factory",

			[](lua_State* pLuaVM)->int
			{
				// SDE_Factory ģ�鷽��
				lua_createtable(pLuaVM, 0, 2);

				lua_pushstring(pLuaVM, "CreateEntityDef");
				lua_pushcfunction(pLuaVM, CreateEntityDef);
				lua_rawset(pLuaVM, -3);

				lua_pushstring(pLuaVM, "CreateSceneDef");
				lua_pushcfunction(pLuaVM, CreateSceneDef);
				lua_rawset(pLuaVM, -3);

				return 1;
			}
		);
	}
};

GameFactory::GameFactory()
{
	void* pMem = MemoryManager::Instance().Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl();
}

GameFactory::~GameFactory()
{
	m_pImpl->~Impl();
	MemoryManager::Instance().Free(m_pImpl, sizeof(Impl));
}