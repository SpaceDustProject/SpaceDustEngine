#ifndef _SDE_DIRECTOR_H_
#define _SDE_DIRECTOR_H_

#include <lua.hpp>

class SDE_Director
{
public:
	bool Run();

private:
	class Impl;
	Impl* m_pImpl;

public:
	~SDE_Director();
	SDE_Director(const SDE_Director&) = delete;
	SDE_Director& operator=(const SDE_Director&) = delete;
	static SDE_Director& Instance()
	{
		static SDE_Director instance;
		return instance;
	}
private:
	SDE_Director();
};

#endif // !_SDE_DIRECTOR_H_