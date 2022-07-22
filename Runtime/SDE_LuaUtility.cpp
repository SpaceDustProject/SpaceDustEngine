#include "SDE_LuaUtility.h"

#include "SDE_Debug.h"
#include "SDE_Data.h"

#include "SDE_LuaLightUserdata.h"

#include <string>
#include <filesystem>

void SDE_LuaUtility::GetGlobal(lua_State* pState)
{
	lua_getfield(pState, LUA_REGISTRYINDEX, SDE_NAME_GLOBAL);
}

void SDE_LuaUtility::GetRegistry(lua_State* pState)
{
	GetGlobal(pState);

	lua_pushstring(pState, SDE_NAME_REGISTRY);
	lua_rawget(pState, -2);
	lua_remove(pState, -2);
}

void SDE_LuaUtility::GetRuntime(lua_State* pState)
{
	GetGlobal(pState);

	lua_pushstring(pState, SDE_NAME_RUNTIME);
	lua_rawget(pState, -2);
	lua_remove(pState, -2);
}

void SDE_LuaUtility::GetRuntime(lua_State* pState, const std::string& strName)
{
	GetGlobal(pState);

	lua_pushstring(pState, SDE_NAME_RUNTIME);
	lua_rawget(pState, -2);
	lua_remove(pState, -2);

	lua_pushstring(pState, strName.c_str());
	lua_rawget(pState, -2);
	lua_remove(pState, -2);
}

void SDE_LuaUtility::GetTemporary(lua_State* pState)
{
	GetGlobal(pState);

	lua_pushstring(pState, SDE_NAME_TEMPORARY);
	lua_rawget(pState, -2);
	lua_remove(pState, -2);
}

bool SDE_LuaUtility::RunScript(lua_State* pState, const SDE_Data& dataScript)
{
	switch (luaL_loadstring(pState, dataScript.GetData()))
	{
	case LUA_OK:
		lua_call(pState, 0, LUA_MULTRET);
		return true;

	case LUA_ERRSYNTAX:
		SDE_Debug::Instance().OutputInfo(
			"Failed to run script: Syntax error during precompiling.\n"
		);
		break;

	case LUA_ERRMEM:
		SDE_Debug::Instance().OutputInfo(
			"Failed to run script: Memory allocation error.\n"
		);
		break;

	case LUA_ERRRUN:
		SDE_Debug::Instance().OutputInfo(
			"Failed to run script: Runtime error.\n"
		);
		break;

	case LUA_ERRERR:
		SDE_Debug::Instance().OutputInfo(
			"Failed to run script: Error while running the message handler.\n"
		);
		break;

	case LUA_ERRFILE:
		SDE_Debug::Instance().OutputInfo(
			"Failed to run script: A file-related error occurred.\n"
		);
		break;

	default:
		SDE_Debug::Instance().OutputInfo(
			"Failed to run script: An unknown error occurred.\n"
		);
		break;
	}

	SDE_Debug::Instance().OutputInfo(
		"Lua's error message: %s\n",
		luaL_checkstring(pState, -1)
	);

	return false;
}

void SDE_LuaUtility::SetPackage(lua_State* pState, const SDE_LuaPackage& package)
{
	if (package.GetFuncList().size() <= 0) return;

	for (const SDE_LuaReg& funcReg : package.GetFuncList())
	{
		lua_pushstring(pState, funcReg.name);
		lua_pushcfunction(pState, funcReg.func);
		lua_rawset(pState, -3);
	}
}

void SDE_LuaUtility::RegisterMetatable(lua_State* pState, const SDE_LuaMetatable& metatable)
{
	luaL_newmetatable(pState, metatable.GetType().c_str());
	{
		lua_pushstring(pState, "__index");
		lua_createtable(pState, 0, metatable.GetPackage().GetFuncList().size());

		for (const SDE_LuaReg& funcReg : metatable.GetPackage().GetFuncList())
		{
			lua_pushstring(pState, funcReg.name);
			lua_pushcfunction(pState, funcReg.func);
			lua_rawset(pState, -3);
		}
		lua_rawset(pState, -3);

		if (metatable.GetGCFunc())
		{
			lua_pushstring(pState, "__gc");
			lua_pushcfunction(pState, metatable.GetGCFunc());
			lua_rawset(pState, -3);
		}
	}
	lua_pop(pState, 1);
}

void* SDE_LuaUtility::GetLightUserdata(lua_State* pState, int nIndex, const std::string& strName)
{
	SDE_LuaLightUserdata* pLightUserdata = (SDE_LuaLightUserdata*)luaL_checkudata(pState, nIndex, SDE_TYPE_LIGHTUSERDATA);

	if (pLightUserdata->GetTypeLU() != strName)
	{
		SDE_Debug::Instance().OutputError(
			"Failed to precompile script. "
			"%s can't call functions of type %s.\n",
			pLightUserdata->GetTypeLU().c_str(),
			strName.c_str()
		);
	}
	return pLightUserdata;
}

void* SDE_LuaUtility::GetLightUserdataDef(lua_State* pState, int nIndex, const std::string& strType)
{
	SDE_LuaLightUserdataDef* pDefLightUserdata = nullptr;

	if (lua_type(pState, nIndex) == LUA_TSTRING)
	{
		std::string strName = lua_tostring(pState, nIndex);

		SDE_LuaUtility::GetRegistry(pState);

		lua_pushstring(pState, strType.c_str());
		lua_rawget(pState, -2);
		lua_remove(pState, -2);

		lua_pushstring(pState, strName.c_str());
		lua_rawget(pState, -2);
		lua_remove(pState, -2);

		// 如果注册表内不存在该定义
		if (lua_type(pState, -1) == LUA_TNIL)
			return nullptr;

		lua_pushstring(pState, "userdata");
		lua_rawget(pState, -2);
		pDefLightUserdata = (SDE_LuaLightUserdataDef*)lua_touserdata(pState, -1);
		lua_pop(pState, 1);

		lua_pushstring(pState, "constructor");
		lua_rawget(pState, -2);
		lua_remove(pState, -2);
	}
	else
	{
		pDefLightUserdata = (SDE_LuaLightUserdataDef*)luaL_checkudata(pState, nIndex, strType.c_str());

		SDE_LuaUtility::GetTemporary(pState);

		lua_pushstring(pState, strType.c_str());
		lua_rawget(pState, -2);
		lua_remove(pState, -2);

		lua_pushstring(pState, pDefLightUserdata->strName.c_str());
		lua_rawget(pState, -2);
		lua_remove(pState, -2);
	}
	return pDefLightUserdata;
}

void SDE_LuaUtility::CheckStack(lua_State* pState)
{
	for (int i = 1; i <= lua_gettop(pState); i++)
	{
		SDE_Debug::Instance().OutputInfo(
			"Value at index %d: %s\n",
			i, luaL_typename(pState, i)
		);
	}
}

bool SDE_LuaUtility::AddPath(lua_State* pState, const std::string& strPath)
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

bool SDE_LuaUtility::DeletePath(lua_State* pState, const std::string& strPath)
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

bool SDE_LuaUtility::AddCPath(lua_State* pState, const std::string& strCPath)
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

bool SDE_LuaUtility::DeleteCPath(lua_State* pState, const std::string& strCPath)
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