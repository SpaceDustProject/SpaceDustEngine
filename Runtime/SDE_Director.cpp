#include "SDE_Director.h"
#include "SDE_VirtualMachine.h"
#include "SDE_Blackboard.h"
#include "SDE_Debug.h"

#include "SDE_Timer.h"
#include "SDE_Scene.h"

#include "SDE_Config.h"

#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>

class SDE_Director::Impl
{
public:
	// �Ƿ��˳�
	bool m_isQuit = false;
	// ��ǰ�������еĳ���
	SDE_Scene* m_pRunningScene = nullptr;

	// ��ʱ�������
	std::unordered_map<lua_Integer, SDE_Timer*> m_mapTimer;

public:
	Impl()
	{
		// ע�� Entity Ԫ��
		luaL_newmetatable(g_pMainVM, SDE_ENTITY_NAME);

		lua_pushstring(g_pMainVM, "__index");
		lua_createtable(g_pMainVM, 0, 0);
		luaL_setfuncs(g_pMainVM, SDE_Entity::GetMemberFunc(), 0);
		lua_rawset(g_pMainVM, -3);

		lua_pushstring(g_pMainVM, "__gc");
		lua_pushcfunction(g_pMainVM, SDE_Entity::GetGCFunc());
		lua_rawset(g_pMainVM, -3);

		lua_pushstring(g_pMainVM, "__component");
		lua_createtable(g_pMainVM, 0, 0);
		lua_rawset(g_pMainVM, -3);

		lua_pop(g_pMainVM, 1);

		// ע�� System Ԫ��
		luaL_newmetatable(g_pMainVM, SDE_SYSTEM_NAME);

		lua_pushstring(g_pMainVM, "__index");
		lua_createtable(g_pMainVM, 0, 0);
		luaL_setfuncs(g_pMainVM, SDE_System::GetMemberFunc(), 0);
		lua_rawset(g_pMainVM, -3);

		lua_pushstring(g_pMainVM, "__gc");
		lua_pushcfunction(g_pMainVM, SDE_System::GetGCFunc());
		lua_rawset(g_pMainVM, -3);

		lua_pop(g_pMainVM, 1);
	}
};

bool SDE_Director::Run()
{
	std::chrono::steady_clock::time_point t1;		// ��֡������ʼ��
	std::chrono::steady_clock::time_point t2;		// ��֡���������� & ��֡˯����ʼ��
	std::chrono::steady_clock::time_point t3;		// ��֡˯�߽�����

	std::chrono::duration<double> dTargetTime(0.0);	// ��֡Ŀ��Ԥ����ʱ
	std::chrono::duration<double> dUsedTime(0.0);	// ��֡��������ʵ����ʱ
	std::chrono::duration<double> dActualTime(0.0);	// ��֡����+˯�߽���ʵ����ʱ
	std::chrono::duration<double> dAdjustTime(0.0);	// ��֡˯�ߵ�����ʱ
	std::chrono::duration<double> dSleepTime(0.0);	// ��֡˯��Ԥ����ʱ

	float fFrameRate = 0.0f;						// ��ǰ֡��

	while (!m_pImpl->m_isQuit)
	{
		t1 = std::chrono::steady_clock::now();



		t2 = std::chrono::steady_clock::now();

		if (!SDE_Blackboard::Instance().GetNumber("SDE_FRAME_RATE", fFrameRate) ||
			fFrameRate <= 0.0f)
		{
			SDE_Debug::Instance().OutputLog(
				"Director Running Error: "
				"Invalid frame rate value.\n"
			);
			return false;
		}

		dUsedTime = t2 - t1;
		dTargetTime = std::chrono::duration<double>(1.0 / fFrameRate);
		dSleepTime = dTargetTime - dUsedTime + dAdjustTime;

		if (dSleepTime > std::chrono::duration<double>(0.0))
		{
			std::this_thread::sleep_for(dSleepTime);
		}

		t3 = std::chrono::steady_clock::now();
		dActualTime = t3 - t1;
		dAdjustTime = dAdjustTime * 0.9 + 0.1 * (dTargetTime - dActualTime);
	}

	return true;
}

SDE_Director::SDE_Director()
{
	m_pImpl = new Impl();
}

SDE_Director::~SDE_Director()
{
	delete m_pImpl;
}