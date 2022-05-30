#include "SDE_Blackboard.h"

#include <unordered_map>

class SDE_Blackboard::Impl
{
public:
	std::unordered_map<std::string, int>			m_mapInteger;
	std::unordered_map<std::string, float>			m_mapNumber;
	std::unordered_map<std::string, bool>			m_mapBool;
	std::unordered_map<std::string, std::string>	m_mapString;
};

bool SDE_Blackboard::GetInteger(std::string strKey, int& strValue)
{
	if (m_pImpl->m_mapInteger.find(strKey) != m_pImpl->m_mapInteger.end())
	{
		strValue = m_pImpl->m_mapInteger[strKey];
		return true;
	}
	return false;
}

bool SDE_Blackboard::SetValue(std::string strKey, int strValue, bool canOverride)
{
	if (canOverride)
	{
		m_pImpl->m_mapInteger[strKey] = strValue;
		return true;
	}

	if (m_pImpl->m_mapInteger.find(strKey) != m_pImpl->m_mapInteger.end())
	{
		return false;
	}

	m_pImpl->m_mapInteger[strKey] = strValue;
	return true;
}

bool SDE_Blackboard::GetNumber(std::string strKey, float& strValue)
{
	if (m_pImpl->m_mapNumber.find(strKey) != m_pImpl->m_mapNumber.end())
	{
		strValue = m_pImpl->m_mapNumber[strKey];
		return true;
	}
	return false;
}

bool SDE_Blackboard::SetValue(std::string strKey, float strValue, bool canOverride)
{
	if (canOverride)
	{
		m_pImpl->m_mapNumber[strKey] = strValue;
		return true;
	}

	if (m_pImpl->m_mapNumber.find(strKey) != m_pImpl->m_mapNumber.end())
	{
		return false;
	}

	m_pImpl->m_mapNumber[strKey] = strValue;
	return true;
}

bool SDE_Blackboard::GetBool(std::string strKey, bool& strValue)
{
	if (m_pImpl->m_mapBool.find(strKey) != m_pImpl->m_mapBool.end())
	{
		strValue = m_pImpl->m_mapBool[strKey];
		return true;
	}
	return false;
}

bool SDE_Blackboard::SetValue(std::string strKey, bool strValue, bool canOverride)
{
	if (canOverride)
	{
		m_pImpl->m_mapBool[strKey] = strValue;
		return true;
	}

	if (m_pImpl->m_mapBool.find(strKey) != m_pImpl->m_mapBool.end())
	{
		return false;
	}

	m_pImpl->m_mapBool[strKey] = strValue;
	return true;
}

bool SDE_Blackboard::GetString(std::string strKey, std::string& strValue)
{
	if (m_pImpl->m_mapString.find(strKey) != m_pImpl->m_mapString.end())
	{
		strValue = m_pImpl->m_mapString[strKey];
		return true;
	}
	return false;
}

bool SDE_Blackboard::SetValue(std::string strKey, std::string strValue, bool canOverride)
{
	if (canOverride)
	{
		m_pImpl->m_mapString[strKey] = strValue;
		return true;
	}

	if (m_pImpl->m_mapString.find(strKey) != m_pImpl->m_mapString.end())
	{
		return false;
	}

	m_pImpl->m_mapString[strKey] = strValue;
	return true;
}

SDE_Blackboard::SDE_Blackboard()
{
	m_pImpl = new Impl();
}

SDE_Blackboard::~SDE_Blackboard()
{
	delete m_pImpl;
}