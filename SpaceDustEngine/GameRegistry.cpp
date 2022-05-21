#include "GameRegistry.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"
#include "DebugModule.h"

#include <new>
#include <functional> 

class GameRegistry::Impl
{
public:
	static int RegisterComponent(lua_State* pLuaVM)
	{
		lua_getglobal(pLuaVM, "SDE_Registry");
		lua_pushstring(pLuaVM, "Component");
		lua_rawget(pLuaVM, -2);
		lua_pushvalue(pLuaVM, 1);
		lua_pushvalue(pLuaVM, 2);
		lua_rawset(pLuaVM, -3);
		return 0;
	}

	static int RegisterSystem(lua_State* pLuaVM)
	{
		lua_getglobal(pLuaVM, "SDE_Registry");
		lua_pushstring(pLuaVM, "System");
		lua_rawget(pLuaVM, -2);
		lua_pushvalue(pLuaVM, 1);
		lua_pushvalue(pLuaVM, 2);
		lua_rawset(pLuaVM, -3);
		return 0;
	}

	static int GetComponent(lua_State* pLuaVM)
	{
		// ��ȡ��ע������
		lua_getglobal(pLuaVM, "SDE_Registry");
		lua_pushstring(pLuaVM, "Component");
		lua_rawget(pLuaVM, -2);
		lua_pushvalue(pLuaVM, 1);
		lua_rawget(pLuaVM, -2);

		// ���Ƹ����
		VirtualMachine::CopyTable(pLuaVM, -1);

		return 1;
	}

	static int GetSystem(lua_State* pLuaVM)
	{
		// ��ȡ��ע���ϵͳ
		lua_getglobal(pLuaVM, "SDE_Registry");
		lua_pushstring(pLuaVM, "System");
		lua_rawget(pLuaVM, -2);
		lua_pushvalue(pLuaVM, 1);
		lua_rawget(pLuaVM, -2);

		// ���Ƹ�ϵͳ
		VirtualMachine::CopyTable(pLuaVM, -1);

		return 1;
	}

public:
	Impl()
	{
		lua_State* pMainVM = VirtualMachine::Instance().GetLuaState();

		// ��ʼ��ȫ��ע���
		lua_newtable(pMainVM);

		// ��ʼ�� Component �� System ���ű�
		lua_pushstring(pMainVM, "Component");
		lua_newtable(pMainVM);
		lua_rawset(pMainVM, -3);

		lua_pushstring(pMainVM, "System");
		lua_newtable(pMainVM);
		lua_rawset(pMainVM, -3);

		// �����ű�ע��Ϊ SDE ��ע���
		lua_setglobal(pMainVM, "SDE_Registry");

		VirtualMachine::AddPreloadFunc(
			pMainVM, "SDE_Register",

			[](lua_State* pLuaVM)->int
			{
				lua_createtable(pLuaVM, 0, 4);

				lua_pushstring(pLuaVM, "RegisterComponent");
				lua_pushcfunction(pLuaVM, RegisterComponent);
				lua_rawset(pLuaVM, -3);

				lua_pushstring(pLuaVM, "RegisterSystem");
				lua_pushcfunction(pLuaVM, RegisterSystem);
				lua_rawset(pLuaVM, -3);

				lua_pushstring(pLuaVM, "GetComponent");
				lua_pushcfunction(pLuaVM, GetComponent);
				lua_rawset(pLuaVM, -3);

				lua_pushstring(pLuaVM, "GetSystem");
				lua_pushcfunction(pLuaVM, GetSystem);
				lua_rawset(pLuaVM, -3);

				return 1;
			}
		);
	}
};

GameRegistry::GameRegistry()
{
	void* pMem = MemoryManager::Instance().Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl();
}

GameRegistry::~GameRegistry()
{
	m_pImpl->~Impl();
	MemoryManager::Instance().Free(m_pImpl, sizeof(Impl));
}