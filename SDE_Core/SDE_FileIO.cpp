#include "SDE_FileIO.h"
#include "SDE_Debug.h"

#include "SDE_Data.h"

#include <fstream>
#include <sstream>
#include <filesystem>

class SDE_FileIO::Impl
{
public:
	std::ifstream m_finStream;
	std::ofstream m_foutStream;
public:
	~Impl()
	{
		m_finStream.clear(); m_finStream.close();
		m_foutStream.clear(); m_foutStream.close();
	}
};

SDE_Data SDE_FileIO::Input(const std::string& strFileName, bool isBinary)
{
	if (!isBinary) {
		m_pImpl->m_finStream.open(strFileName, std::ios_base::in);
	}
	else {
		m_pImpl->m_finStream.open(strFileName, std::ios_base::in | std::ios_base::binary);
	}

	if (!m_pImpl->m_finStream.good())
	{
		SDE_Debug::Instance().OutputInfo(
			"Failed to open %s when try to read.\n",
			strFileName
		);
		m_pImpl->m_finStream.clear();
		return { nullptr, 0 };
	}

	std::stringstream ssScriptContent;
	ssScriptContent << m_pImpl->m_finStream.rdbuf();
	m_pImpl->m_finStream.close();
	m_pImpl->m_finStream.clear();

	size_t nSize = ssScriptContent.str().size() + 1;
	char* pData = new char[nSize];
	strcpy_s(pData, nSize, ssScriptContent.str().c_str());

	return { pData, nSize };
}

void SDE_FileIO::Output(const std::string& strFileName, const SDE_Data& dataWritten, bool isBinary)
{
	if (!isBinary) {
		m_pImpl->m_foutStream.open(strFileName, std::ios_base::out);
	}
	else {
		m_pImpl->m_foutStream.open(strFileName, std::ios_base::out | std::ios_base::binary);
	}

	if (!m_pImpl->m_foutStream.good())
	{
		SDE_Debug::Instance().OutputInfo(
			"Failed to open %s when try to write.\n",
			strFileName
		);
		m_pImpl->m_foutStream.clear();
		return;
	}

	if (!isBinary)
	{
		m_pImpl->m_foutStream << dataWritten.GetData();
	}
	else
	{
		m_pImpl->m_foutStream.write(dataWritten.GetData(), dataWritten.GetSize());
	}

	m_pImpl->m_foutStream.flush();
	m_pImpl->m_foutStream.close();
	m_pImpl->m_foutStream.clear();
	
	return;
}

SDE_FileIO::SDE_FileIO()
{
	m_pImpl = new Impl();
}

SDE_FileIO::~SDE_FileIO()
{
	delete m_pImpl;
}