#include "SDE_Settings.h"
#include "SDE_Debug.h"

#include <string>
#include <string.h>

void SDE_Settings::Reset()
{
	strStartScene = "SDE_DefaultScene";
	strPackagePath = "";
	strScriptPath = "";
}

bool SDE_Settings::ParseConfig(cJSON* pJSONConfigRoot)
{
	// 重置设置
	Reset();

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
	if ((pJSONConfigStartScene = cJSON_GetObjectItem(pJSONConfigRoot, "startScene")) &&
		pJSONConfigStartScene->type == cJSON_String)
	{
		strStartScene = pJSONConfigStartScene->valuestring;
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
	if ((pJSONConfigPackagePath = cJSON_GetObjectItem(pJSONConfigRoot, "packagePath")) &&
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
				strPackagePath.append(pJSONConfigPathItem->valuestring);
				if (i != nArraySize - 1) strPackagePath.append(";");
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
	if ((pJSONConfigScriptPath = cJSON_GetObjectItem(pJSONConfigRoot, "scriptPath")) &&
		pJSONConfigScriptPath->type == cJSON_Array)
	{
		int nArraySize = cJSON_GetArraySize(pJSONConfigScriptPath);
		cJSON* pJSONConfigPathItem = nullptr;

		for (int i = 0; i < nArraySize; i++)
		{
			if ((pJSONConfigPathItem = cJSON_GetArrayItem(pJSONConfigScriptPath, i)) &&
				pJSONConfigPathItem->type == cJSON_String)
			{
				strScriptPath.append(pJSONConfigPathItem->valuestring);
				if (i != nArraySize - 1) strScriptPath.append(";");
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