#ifndef _GAME_REGISTRY_H_
#define _GAME_REGISTRY_H_

// ECS 框架注册表模块
// 记录注册的所有 Component 和 System
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