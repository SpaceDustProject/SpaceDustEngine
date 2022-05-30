#include "SDE_Settings.h"
#include "SDE_Blackboard.h"
#include "SDE_Debug.h"

#include <string>
#include <string.h>

void SDE_Settings::Reset()
{
	SDE_Blackboard::Instance().SetValue("entry", "SDE_DefaultScene");
}

bool SDE_Settings::ParseConfig(cJSON* pJSONConfigRoot)
{
	cJSON* pJSONConfigStartScene = nullptr;
	cJSON* pJSONConfigPackagePath = nullptr;
	cJSON* pJSONConfigScriptPath = nullptr;

	if (!pJSONConfigRoot || pJSONConfigRoot->type != cJSON_Object)
	{
		SDE_Debug::Instance().OutputLog(
			"Configuration Error: "
			"Failed to parse the config.\n"
		);
		return false;
	}

	// 读取开始场景
	if ((pJSONConfigStartScene = cJSON_GetObjectItem(pJSONConfigRoot, "entry")) &&
		pJSONConfigStartScene->type == cJSON_String)
	{
		SDE_Blackboard::Instance().SetValue("entry", pJSONConfigStartScene->valuestring);
		cJSON_Delete(pJSONConfigStartScene);
	}
	else
	{
		SDE_Debug::Instance().OutputLog(
			"Configuration Error: "
			"Invalid start scene configuration.\n"
		);
		return false;
	}

	// 读取包路径
	if ((pJSONConfigPackagePath = cJSON_GetObjectItem(pJSONConfigRoot, "cpath")) &&
		pJSONConfigPackagePath->type == cJSON_Array)
	{
		int nArraySize = cJSON_GetArraySize(pJSONConfigPackagePath);
		cJSON* pJSONConfigPathItem = nullptr;
		std::string _strPackagePath;

		for (int i = 0; i < nArraySize; i++)
		{
			if ((pJSONConfigPathItem = cJSON_GetArrayItem(pJSONConfigPackagePath, i)) &&
				pJSONConfigPathItem->type == cJSON_String)
			{
				_strPackagePath.append(";").append(pJSONConfigPathItem->valuestring);
				cJSON_Delete(pJSONConfigPathItem);
			}
			else
			{
				SDE_Debug::Instance().OutputLog(
					"Configure Error: "
					"Invalid package path value on the index of %d.\n", i
				);
			}
		}
		SDE_Blackboard::Instance().SetValue("cpath", _strPackagePath.c_str());
		cJSON_Delete(pJSONConfigPackagePath);
	}
	else
	{
		SDE_Debug::Instance().OutputLog(
			"Configure Error: "
			"Package path should be an array.\n"
		);
		return false;
	}

	// 读取脚本路径
	if ((pJSONConfigScriptPath = cJSON_GetObjectItem(pJSONConfigRoot, "path")) &&
		pJSONConfigScriptPath->type == cJSON_Array)
	{
		int nArraySize = cJSON_GetArraySize(pJSONConfigScriptPath);
		cJSON* pJSONConfigPathItem = nullptr;
		std::string _strScriptPath;

		for (int i = 0; i < nArraySize; i++)
		{
			if ((pJSONConfigPathItem = cJSON_GetArrayItem(pJSONConfigScriptPath, i)) &&
				pJSONConfigPathItem->type == cJSON_String)
			{
				_strScriptPath.append(";").append(pJSONConfigPathItem->valuestring);
				cJSON_Delete(pJSONConfigPathItem);
			}
			else
			{
				SDE_Debug::Instance().OutputLog(
					"Configure Error: "
					"Invalid script path value on the index of %d.\n", i
				);
			}
		}
		SDE_Blackboard::Instance().SetValue("path", _strScriptPath.c_str());
		cJSON_Delete(pJSONConfigScriptPath);
	}
	else
	{
		SDE_Debug::Instance().OutputLog(
			"Configure Error: "
			"Script path should be an array.\n"
		);
		return false;
	}

	return true;
}

SDE_Settings::SDE_Settings()
{
	Reset();
}

SDE_Settings g_settings = SDE_Settings();