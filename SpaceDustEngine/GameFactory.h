#ifndef _GAME_FACTORY_H_
#define _GAME_FACTORY_H_

// ECS 框架工厂模块
// 用于构造 userdata
class GameFactory
{
private:
	class Impl;
	Impl* m_pImpl;

public:
	~GameFactory();
	GameFactory(const GameFactory&) = delete;
	GameFactory& operator=(const GameFactory&) = delete;
	static GameFactory& Instance()
	{
		static GameFactory instance;
		return instance;
	}

private:
	GameFactory();
};

#endif // !_GAME_FACTORY_H_