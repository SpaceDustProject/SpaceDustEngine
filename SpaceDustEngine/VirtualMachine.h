#ifndef _VIRTUAL_MACHINE_H_
#define _VIRTUAL_MACHINE_H_

#include <lua.hpp>

#define SDE_API extern "C"

class VirtualMachine
{
public:
	// 获取全局虚拟机状态
	static lua_State* GetLuaState();

	// 添加预加载器
	static void AddPreloadFunc(lua_State* pLuaVM, const char* strName, lua_CFunction funcPreload);

	// 获取索引 index 处 table 的长度
	static int GetTableLength(lua_State* pLuaVM, int nIndex);

	// 浅拷贝指定 index 上的 table 并推入栈
	static void CopyTable(lua_State* pLuaVM, int nIndex);

public:
	~VirtualMachine();
	VirtualMachine(const VirtualMachine&) = delete;
	VirtualMachine& operator=(const VirtualMachine&) = delete;
	static VirtualMachine& Instance()
	{
		static VirtualMachine instance;
		return instance;
	}

private:
	VirtualMachine();
};

#endif // !_VIRTUAL_MACHINE_H_