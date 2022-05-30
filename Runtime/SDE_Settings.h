#ifndef _SDE_SETTINGS_H_
#define _SDE_SETTINGS_H_

#include <cJSON.h>
#include <string>

struct SDE_Settings
{
	// ���� SDE ����
	void Reset();

	// ���������ļ�
	bool ParseConfig(cJSON* pJSONConfigRoot);

	SDE_Settings();
	SDE_Settings(const SDE_Settings&) = default;
	SDE_Settings& operator=(const SDE_Settings&) = default;
};

#endif // !_SDE_SETTINGS_H_