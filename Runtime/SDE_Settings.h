#ifndef _SDE_SETTINGS_H_
#define _SDE_SETTINGS_H_

#include <cJSON.h>

struct SDE_Settings
{
	const char* strStartScene;		// ��ʼ����
	const char* strPackagePath;		// ��·��
	const char* strScriptPath;		// �ű�·��

	// ���� SDE ����
	void Reset();

	// ���������ļ��޸�����
	bool ParseConfig(cJSON* pJSONConfigRoot);

	SDE_Settings();
};

extern SDE_Settings g_settings;

#endif // !_SDE_SETTINGS_H_