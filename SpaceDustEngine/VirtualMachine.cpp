#include "VirtualMachine.h"
#include "DebugModule.h"

lua_State* g_pMainVM;

void VirtualMachine::AddPreloadFunc(lua_State* pLuaVM, const char* strName, lua_CFunction funcPreload)
{
	// 获取 Preload 表
	lua_getglobal(pLuaVM, "package");
	lua_pushstring(pLuaVM, "preload");
	lua_rawget(pLuaVM, -2);

	// 注册 Preload 函数
	lua_pushstring(pLuaVM, strName);
	lua_pushcfunction(pLuaVM, funcPreload);
	lua_rawset(pLuaVM, -3);

	// 清理工作
	lua_pop(pLuaVM, 2);
}

int VirtualMachine::GetTableLength(lua_State* pLuaVM, int nIndex)
{
	int nLength = 0;

	lua_pushnil(pLuaVM);
	while (lua_next(pLuaVM, nIndex))
	{
		nLength++;
		lua_pop(pLuaVM, 1);
	}
	return nLength;
}

void VirtualMachine::CopyTable(lua_State* pLuaVM, int nIndex)
{
	if (!lua_istable(pLuaVM, nIndex))
	{
		DebugModule::Instance().Output(
			"Error Copying: "
			"The value on the top of stack is not a table.\n"
		);
		return;
	}

	lua_newtable(pLuaVM);

	if (nIndex < 0) nIndex -= 2;

	// 开始拷贝
	lua_pushnil(pLuaVM);
	while (lua_next(pLuaVM, nIndex))
	{
		lua_pushvalue(pLuaVM, -2);
		lua_insert(pLuaVM, -2);
		lua_rawset(pLuaVM, -4);
	}
}