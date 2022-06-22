#include "SDE_VirtualMachine.h"
#include "SDE_Blackboard.h"
#include "SDE_Debug.h"
#include "SDE_Data.h"

#include <string>
#include <filesystem>

bool SDE_VirtualMachine::RunScript(lua_State* pState, const SDE_Data& dataScript)
{
	switch (luaL_loadstring(pState, dataScript.GetData()))
	{
	case LUA_OK:
		lua_pcall(pState, 0, LUA_MULTRET, 0);
		return true;

	case LUA_ERRSYNTAX:
		SDE_Debug::Instance().OutputLog(
			"Script Running Error: "
			"Syntax error during precompiling.\n"
		);
		return false;

	case LUA_ERRMEM:
		SDE_Debug::Instance().OutputLog(
			"Script Running Error: "
			"Memory allocation error.\n"
		);
		return false;

	case LUA_ERRRUN:
		SDE_Debug::Instance().OutputLog(
			"Script Running Error: "
			"Runtime error.\n"
		);
		return false;

	case LUA_ERRERR:
		SDE_Debug::Instance().OutputLog(
			"Script Running Error: "
			"Error while running the message handler.\n"
		);
		return false;

	case LUA_ERRFILE:
		SDE_Debug::Instance().OutputLog(
			"Script Running Error: "
			"A file-related error occurred.\n"
		);
		return false;

	default:
		SDE_Debug::Instance().OutputLog(	
			"Script Running Error: "
			"An unknown error occurred.\n"
		);
		return false;
	}
}

bool SDE_VirtualMachine::AddPath(lua_State* pState, const std::string& strPath)
{
	if (strPath.empty()) return false;

	lua_getglobal(pState, "package");
	lua_getfield(pState, -1, "path");
	std::string strPackagePath = lua_tostring(pState, -1);
	strPackagePath.append(";").append(strPath);
	lua_pushstring(pState, strPackagePath.c_str());
	lua_setfield(pState, -3, "path");
	lua_pop(pState, 2);

	return true;
}

bool SDE_VirtualMachine::DeletePath(lua_State* pState, const std::string& strPath)
{
	if (strPath.empty()) return false;

	lua_getglobal(pState, "package");
	lua_getfield(pState, -1, "path");
	std::string strPackagePath = lua_tostring(pState, -1);

	size_t nIndex = strPackagePath.find(strPath);
	if (nIndex == strPackagePath.npos)
	{
		lua_pop(pState, 2);
		return false;
	}

	strPackagePath.erase(nIndex - 1, strPath.size() + 1);
	lua_pushstring(pState, strPackagePath.c_str());
	lua_setfield(pState, -3, "path");
	lua_pop(pState, 2);

	return true;
}

bool SDE_VirtualMachine::AddCPath(lua_State* pState, const std::string& strCPath)
{
	if (strCPath.empty()) return false;

	lua_getglobal(pState, "package");
	lua_getfield(pState, -1, "cpath");
	std::string strPackageCPath = lua_tostring(pState, -1);
	strPackageCPath.append(";").append(strCPath);
	lua_pushstring(pState, strPackageCPath.c_str());
	lua_setfield(pState, -3, "cpath");
	lua_pop(pState, 2);

	return true;
}

bool SDE_VirtualMachine::DeleteCPath(lua_State* pState, const std::string& strCPath)
{
	if (strCPath.empty()) return false;

	lua_getglobal(pState, "package");
	lua_getfield(pState, -1, "cpath");
	std::string strPackageCPath = lua_tostring(pState, -1);

	size_t nIndex = strPackageCPath.find(strCPath);
	if (nIndex == strPackageCPath.npos)
	{
		lua_pop(pState, 2);
		return false;
	}

	strPackageCPath.erase(nIndex - 1, strCPath.size() + 1);
	lua_pushstring(pState, strPackageCPath.c_str());
	lua_setfield(pState, -3, "cpath");
	lua_pop(pState, 2);

	return true;
}

void SDE_VirtualMachine::GetGlobal(lua_State* pState)
{
	lua_getfield(pState, LUA_REGISTRYINDEX, "SDE_Global");
}