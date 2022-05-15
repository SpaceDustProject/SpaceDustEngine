#include "GameRegistry.h"

#include "MemoryManager.h"
#include "VirtualMachine.h"
#include "DebugModule.h"

#include <new>
#include <functional>

class GameRegistry::Impl
{
public:
	Impl()
	{
		lua_State* pMainVM = VirtualMachine::Instance().GetLuaState();

		// 初始化全局注册表
		lua_newtable(pMainVM);
		lua_setglobal(pMainVM, "SDE_Registry");

		VirtualMachine::AddPreloadFunc(

			VirtualMachine::Instance().GetLuaState(),

			"SDE_Register",

			[](lua_State* pLuaVM)->int
			{
				lua_createtable(pLuaVM, 0, 2);

				lua_pushstring(pLuaVM, "RegisterComponent");
				lua_pushcfunction(pLuaVM,
					[](lua_State* pLuaVM)->int
					{
						lua_getglobal(pLuaVM, "SDL_Registry");
						lua_pushstring(pLuaVM, "Component");
						lua_rawget(pLuaVM, -2);
						lua_insert(pLuaVM, 1);
						lua_pop(pLuaVM, 1);
						lua_rawset(pLuaVM, -3);
					}
				);
				lua_rawset(pLuaVM, -3);

				lua_pushstring(pLuaVM, "RegisterSystem");
				lua_pushcfunction(pLuaVM,
					[](lua_State* pLuaVM)->int
					{
						lua_getglobal(pLuaVM, "SDL_Registry");
						lua_pushstring(pLuaVM, "System");
						lua_rawget(pLuaVM, -2);
						lua_insert(pLuaVM, 1);
						lua_pop(pLuaVM, 1);
						lua_rawset(pLuaVM, -3);
					}
				);
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