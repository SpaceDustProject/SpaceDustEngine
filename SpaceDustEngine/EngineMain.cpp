#include "VirtualMachine.h"
#include "DebugModule.h"

#include <vector>
#include <sstream>
#include <fstream>

#include <cJSON.h>

int main(int argc, char* argv[])
{
	// 初始化全局 Lua 状态机
	lua_State* pMainVM = VirtualMachine::Instance().GetLuaState();

	// 读取 JSON 配置文件
	std::ifstream fConfigStream("config.json");
	if (!fConfigStream.good())
	{
		DebugModule::Instance().Output("Configure Error: Failed to open config.json\n");
		return -1;
	}

	std::stringstream strConfigStream;
	strConfigStream << fConfigStream.rdbuf();
	fConfigStream.close(); fConfigStream.clear();

	// "entry": ""
	cJSON* pJSONConfigRoot = nullptr, * pJSONConfigEntry = nullptr;
	if (!((pJSONConfigRoot = cJSON_Parse(strConfigStream.str().c_str()))
		&& (pJSONConfigRoot->type == cJSON_Object)))
	{
		strConfigStream.clear();
		cJSON_Delete(pJSONConfigRoot);
		pJSONConfigRoot = nullptr;
		DebugModule::Instance().Output("Configure Error: Failed to parse configuration file\n");
		return -1;
	}
	if (!((pJSONConfigEntry = cJSON_GetObjectItem(pJSONConfigRoot, "entry"))
		&& (pJSONConfigEntry->type == cJSON_String)))
	{
		strConfigStream.clear();
		cJSON_Delete(pJSONConfigRoot);
		pJSONConfigRoot = nullptr;
		DebugModule::Instance().Output("Configure Error: Invalid entry file configuretion\n");
		return -1;
	}
	std::string strNameEntry = pJSONConfigEntry->valuestring;
	strConfigStream.clear();

	// "package": []
	cJSON* pJSONConfigPackage = nullptr;
	if ((pJSONConfigPackage = cJSON_GetObjectItem(pJSONConfigRoot, "package")))
	{
		cJSON* pJSONConfigPath = nullptr, * pJSONConfigCPath = nullptr;
		// "path": []
		if ((pJSONConfigPath = cJSON_GetObjectItem(pJSONConfigPackage, "path"))
			&& (pJSONConfigPath->type == cJSON_Array))
		{
			lua_getglobal(pMainVM, "package");
			lua_pushstring(pMainVM, "path");
			lua_rawget(pMainVM, -2);
			std::string _strPath = lua_tostring(pMainVM, -1);
			int _nArraySize = cJSON_GetArraySize(pJSONConfigPackage);
			for (int i = 0; i < _nArraySize; i++)
			{
				cJSON* _pJSONPathItem = nullptr;
				if ((_pJSONPathItem = cJSON_GetArrayItem(pJSONConfigPath, i))
					&& (_pJSONPathItem->type == cJSON_String))
					_strPath.append(";").append(_pJSONPathItem->valuestring);
			}
			lua_pushstring(pMainVM, _strPath.c_str());
			lua_setfield(pMainVM, -3, "path");
			lua_pop(pMainVM, 2);
		}
		// "cpath": []
		if ((pJSONConfigCPath = cJSON_GetObjectItem(pJSONConfigPackage, "cpath"))
			&& pJSONConfigCPath->type == cJSON_Array)
		{
			lua_getglobal(pMainVM, "package");
			lua_pushstring(pMainVM, "cpath");
			lua_rawget(pMainVM, -2);
			std::string _strCPath = lua_tostring(pMainVM, -1);
			int _nArraySize = cJSON_GetArraySize(pJSONConfigPackage);
			for (int i = 0; i < _nArraySize; i++)
			{
				cJSON* _pJSONPathItem = nullptr;
				if ((_pJSONPathItem = cJSON_GetArrayItem(pJSONConfigCPath, i))
					&& (_pJSONPathItem->type == cJSON_String))
					_strCPath.append(";").append(_pJSONPathItem->valuestring);
			}
			lua_pushstring(pMainVM, _strCPath.c_str());
			lua_setfield(pMainVM, -3, "cpath");
			lua_pop(pMainVM, 2);
		}
	}

	// "command": []
	std::vector<std::string> vCommandList;
	cJSON* pJSONConfigCommand = nullptr;
	if ((pJSONConfigCommand = cJSON_GetObjectItem(pJSONConfigRoot, "command"))
		&& (pJSONConfigCommand->type == cJSON_Array))
	{
		int _nArraySize = cJSON_GetArraySize(pJSONConfigCommand);
		for (int i = 0; i < _nArraySize; i++)
		{
			cJSON* _pJSONPathItem = nullptr;
			if ((_pJSONPathItem = cJSON_GetArrayItem(pJSONConfigCommand, i))
				&& (_pJSONPathItem->type == cJSON_String))
				vCommandList.push_back(_pJSONPathItem->valuestring);
		}
	}

	cJSON_Delete(pJSONConfigRoot);
	
	// 传递启动参数和环境变量
	lua_createtable(pMainVM, argc, 0);
	for (long long i = 0; i < argc; i++)
	{
		lua_pushstring(pMainVM, argv[i]);
		lua_rawseti(pMainVM, -2, i + 1);
	}
	for (long long i = argc; i < vCommandList.size() + argc; i++)
	{
		lua_pushstring(pMainVM, vCommandList[(size_t)i - argc].c_str());
		lua_rawseti(pMainVM, -2, i + 1);
	}
	lua_setglobal(pMainVM, "_ARGV");
	std::vector<std::string>().swap(vCommandList);

	int lenEnviron = 0; while (environ[lenEnviron]) lenEnviron++;
	lua_createtable(pMainVM, 0, lenEnviron);
	for (int i = 0; i < lenEnviron; i++)
	{
		std::string strEnvp = environ[i];
		size_t nIndexEqual = strEnvp.find_first_of('=');
		lua_pushstring(pMainVM, strEnvp.substr(0, nIndexEqual).c_str());
		lua_pushstring(pMainVM, strEnvp.substr(nIndexEqual + 1).c_str());
		lua_rawset(pMainVM, -3);
	}
	lua_setglobal(pMainVM, "_ENVP");

	// 执行入口脚本文件
	try
	{
		if (luaL_dofile(pMainVM, strNameEntry.c_str()))
		{
			DebugModule::Instance().Output(
				"Runtime Error: %s\n",
				lua_tostring(pMainVM, -1)
			);
		}
	}
	catch (const std::exception& err)
	{
		DebugModule::Instance().Output(
			"Engine Crashed: %s\n",
			err.what()
		);
	}

	return 0;
}