#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <lua.hpp>

#define SDE_API extern "C"

class VirtualMachine
{
public:
	// 获取全局虚拟机状态
	lua_State* GetLuaState() { return m_pLuaVM; }

	// 添加预加载器
	static void AddPreloadFunc(lua_State* pLuaVM, const char* strName, lua_CFunction funcPreload)
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

	// 获取索引 index 处 table 的长度
	static int GetTableLength(lua_State* pLuaVM, int nIndex)
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

	// table 浅拷贝
	// 浅拷贝指定 index 上的表并推入栈
	static void CopyTable(lua_State* pLuaVM, int nIndex)
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

		if (nIndex < 0) nIndex--;

		// 开始拷贝
		lua_pushnil(pLuaVM);
		while (lua_next(pLuaVM, nIndex))
		{
			lua_pushvalue(pLuaVM, -2);
			lua_rawset(pLuaVM, -3);
			lua_pop(pLuaVM, 2);
		}
	}

private:
	lua_State* m_pLuaVM;

public:
	~VirtualMachine() { lua_close(m_pLuaVM); }
	VirtualMachine(const VirtualMachine&) = delete;
	VirtualMachine& operator=(const VirtualMachine&) = delete;
	static VirtualMachine& Instance()
	{
		static VirtualMachine instance;
		return instance;
	}

private:
	VirtualMachine()
	{
		// 初始化 Lua 虚拟机
		m_pLuaVM = luaL_newstate();
		luaL_openlibs(m_pLuaVM);
		lua_gc(m_pLuaVM, LUA_GCINC, 100);
	}
};

#endif // !_VIRTUAL_MACHINE_H_