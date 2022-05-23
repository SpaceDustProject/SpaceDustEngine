#ifndef _SDE_VIRTUAL_MACHINE_H_
#define _SDE_VIRTUAL_MACHINE_H_

#include <lua.hpp>

class SDE_VirtualMachine
{
public:
	// ��ȡ Lua ��������
	static lua_State* GetMainVM();

public:
	~SDE_VirtualMachine();
	SDE_VirtualMachine(const SDE_VirtualMachine&) = delete;
	SDE_VirtualMachine& operator=(const SDE_VirtualMachine&) = delete;
	static SDE_VirtualMachine& Instance()
	{
		static SDE_VirtualMachine instance;
		return instance;
	}
private:
	SDE_VirtualMachine();
};

#endif // !_SDE_VIRTUAL_MACHINE_H_