#ifndef _GAME_REGISTRY_H_
#define _GAME_REGISTRY_H_

// ECS ���ע���ģ��
// ��¼ע������� Component �� System
class GameRegistry
{
private:
	class Impl;
	Impl* m_pImpl;

public:
	~GameRegistry();
	GameRegistry(const GameRegistry&) = delete;
	GameRegistry& operator=(const GameRegistry&) = delete;
	static GameRegistry& Instance()
	{
		static GameRegistry instance;
		return instance;
	}

private:
	GameRegistry();
};

#endif // !_GAME_REGISTRY_H_