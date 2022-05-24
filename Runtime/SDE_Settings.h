#ifndef _SDE_SETTINGS_H_
#define _SDE_SETTINGS_H_

#include <cJSON.h>
#include <string>

struct SDE_Settings
{
	std::string strStartScene;		// ��ʼ����
	std::string strPackagePath;		// ��·��
	std::string strScriptPath;		// �ű�·��

	// ���� SDE ����
	void Reset();

	// ���������ļ��޸�����
	bool ParseConfig(cJSON* pJSONConfigRoot);

	SDE_Settings();
	SDE_Settings(const SDE_Settings&) = default;
	SDE_Settings& operator=(const SDE_Settings&) = default;
};

extern SDE_Settings g_settings;

#endif // !_SDE_SETTINGS_H_