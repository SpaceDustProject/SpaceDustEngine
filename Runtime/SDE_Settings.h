#ifndef _SDE_SETTINGS_H_
#define _SDE_SETTINGS_H_

#include <cJSON.h>

struct SDE_Settings
{
	const char* strStartScene;		// 开始场景
	const char* strPackagePath;		// 包路径
	const char* strScriptPath;		// 脚本路径

	// 重置 SDE 设置
	void Reset();

	// 解析配置文件修改设置
	bool ParseConfig(cJSON* pJSONConfigRoot);

	SDE_Settings();
};

extern SDE_Settings g_settings;

#endif // !_SDE_SETTINGS_H_