#ifndef _SDE_VIRTUAL_MACHINE_H_
#define _SDE_VIRTUAL_MACHINE_H_

#include <lua.hpp>
#include <string>

class SDE_Data;

class SDE_VirtualMachine
{
public:
	
	bool RunScript(lua_State* pState, const SDE_Data& dataScript);

	bool AddPath(lua_State* pState, const std::string& strPath);
	bool DeletePath(lua_State* pState, const std::string& strPath);

	bool AddCPath(lua_State* pState, const std::string& strCPath);
	bool DeleteCPath(lua_State* pState, const std::string& strCPath);

public:
	~SDE_VirtualMachine();
	SDE_VirtualMachine(const SDE_VirtualMachine&) = delete;
	SDE_VirtualMachine& operator=(const SDE_VirtualMachine&) = delete;
	static SDE_VirtualMachine& Instance()
	{
		static SDE_VirtualMachine instance;
		return instance;
	}
private:
	SDE_VirtualMachine();
};

extern lua_State* g_pMainVM;

#endif // !_SDE_VIRTUAL_MACHINE_H_