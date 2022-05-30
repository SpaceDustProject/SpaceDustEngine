#ifndef _SDE_SETTINGS_H_
#define _SDE_SETTINGS_H_

#include <cJSON.h>
#include <string>

struct SDE_Settings
{
	// 重置 SDE 设置
	void Reset();

	// 解析配置文件
	bool ParseConfig(cJSON* pJSONConfigRoot);

	SDE_Settings();
	SDE_Settings(const SDE_Settings&) = default;
	SDE_Settings& operator=(const SDE_Settings&) = default;
};

#endif // !_SDE_SETTINGS_H_