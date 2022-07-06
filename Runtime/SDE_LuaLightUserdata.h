#ifndef _SDE_LUA_LIGHT_USERDATA_H_
#define _SDE_LUA_LIGHT_USERDATA_H_

#include <string>

#define SDE_TYPE_LIGHTUSERDATA	"SDE_LightUserdata"

class SDE_LuaLightMetatable;

class SDE_LuaLightUserdata
{
public:
	const std::string& GetTypeLU() const {
		return m_strTypeLU;
	}

private:
	std::string m_strTypeLU;

public:
	SDE_LuaLightUserdata(const std::string& strTypeLU)
		: m_strTypeLU(strTypeLU) {}

	~SDE_LuaLightUserdata() = default;
};

#endif // !_SDE_LUA_LIGHT_USERDATA_H_