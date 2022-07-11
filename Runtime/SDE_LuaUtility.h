#ifndef SDE_LUA_UTILITY_H_
#define SDE_LUA_UTILITY_H_

#define SDE_API extern "C" int

#define SDE_LUA_FUNC_X(FUNC_NAME, STATE_NAME) \
		SDE_API FUNC_NAME(lua_State* STATE_NAME)

#define SDE_LUA_FUNC(FUNC_NAME) SDE_LUA_FUNC_X(FUNC_NAME, pState)

#define SDE_NAME_GLOBAL		"SDE_Global"
#define SDE_NAME_REGISTRY	"SDE_Registry"
#define SDE_NAME_RUNTIME	"SDE_Runtime"
#define SDE_NAME_TEMPORARY	"SDE_Temporary"

#include <lua.hpp>
#include <list>
#include <string>
#include <initializer_list>

typedef int (*SDE_LuaFunc) (lua_State* L);

class SDE_Data;
class SDE_LuaLightMetatable;

struct SDE_LuaReg
{
	const char* name;
	SDE_LuaFunc func;
};

class SDE_LuaPackage
{
public:
	const std::list<SDE_LuaReg>& GetFuncList() const {
		return m_listFunc;
	}

private:
	std::list<SDE_LuaReg> m_listFunc;

public:
	SDE_LuaPackage(const std::initializer_list<SDE_LuaReg>& listFunc) :
		m_listFunc(listFunc) {}

	~SDE_LuaPackage() = default;
};

// 为 light userdata 这类引擎层管理的对象提供元表
class SDE_LuaLightMetatable
{
public:
	const std::string& GetType() const {
		return m_strType;
	}

	const SDE_LuaPackage& GetPackage() const {
		return m_packageFunc;
	}

private:
	std::string		m_strType;
	SDE_LuaPackage	m_packageFunc;

public:
	SDE_LuaLightMetatable(
		const std::string& strType,
		const SDE_LuaPackage& packageFunc
	) :
		m_strType(strType), m_packageFunc(packageFunc) {}

	~SDE_LuaLightMetatable() = default;
};

// 为 full userdata 和 table 这类 lua 管理的对象提供元表
class SDE_LuaMetatable
{
public:
	const std::string& GetType() const {
		return m_strType;
	}

	const SDE_LuaPackage& GetPackage() const {
		return m_packageFunc;
	}

	SDE_LuaFunc GetGCFunc() const {
		return m_funcGC;
	}

private:
	std::string		m_strType;
	SDE_LuaPackage	m_packageFunc;
	SDE_LuaFunc		m_funcGC;

public:
	SDE_LuaMetatable(
		const std::string& strType,
		const SDE_LuaPackage& packageFunc,
		SDE_LuaFunc funcGC = nullptr
	) :
		m_strType(strType), m_packageFunc(packageFunc), m_funcGC(funcGC) {}

	~SDE_LuaMetatable() = default;
};

namespace SDE_LuaUtility
{
	// 将指定数据作为脚本运行
	bool RunScript(lua_State* pState, const SDE_Data& dataScript);

	// 获取 SDE 全局表
	void GetGlobal(lua_State* pState);

	// 获取 SDE 全局表中的注册表
	void GetRegistry(lua_State* pState);

	// 获得 SDE 全局表中的运行环境
	void GetRuntime(lua_State* pState);

	// 获得指定场景在全局表中的运行环境
	void GetRuntime(lua_State* pState, const std::string& strName);

	// 获得 SDE 全局表中的临时表
	void GetTemporary(lua_State* pState);
	
	// 将包中的函数注册到栈顶的表中
	void SetPackage(lua_State* pState, const SDE_LuaPackage& package);

	// 将元表注册到全局
	void RegisterMetatable(lua_State* pState, const SDE_LuaMetatable& metatable);

	// 获取轻量用户数据并查看其类型
	void* GetLightUserdata(lua_State* pState, int nIndex, const std::string& strName);

	// 查看堆栈情况
	void CheckStack(lua_State* pState);

	bool AddPath(lua_State* pState, const std::string& strPath);
	bool DeletePath(lua_State* pState, const std::string& strPath);

	bool AddCPath(lua_State* pState, const std::string& strCPath);
	bool DeleteCPath(lua_State* pState, const std::string& strCPath);
};

#endif // !SDE_LUA_UTILITY_H_