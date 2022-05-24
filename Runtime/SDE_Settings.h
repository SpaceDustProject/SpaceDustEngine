#ifndef _SDE_SETTINGS_H_
#define _SDE_SETTINGS_H_

#include <cJSON.h>
#include <string>

struct SDE_Settings
{
	std::string strStartScene;		// 开始场景
	std::string strPackagePath;		// 包路径
	std::string strScriptPath;		// 脚本路径

	// 重置 SDE 设置
	void Reset();

	// 解析配置文件修改设置
	bool ParseConfig(cJSON* pJSONConfigRoot);

	SDE_Settings();
	SDE_Settings(const SDE_Settings&) = default;
	SDE_Settings& operator=(const SDE_Settings&) = default;
};

extern SDE_Settings g_settings;

#endif // !_SDE_SETTINGS_H_