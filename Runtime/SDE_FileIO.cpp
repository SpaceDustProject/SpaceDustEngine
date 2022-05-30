#include "SDE_FileIO.h"
#include "SDE_Debug.h"

#include <fstream>
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
	if (!isBinary) m_pImpl->m_finStream.open(strFileName, std::ios_base::in);
	else m_pImpl->m_finStream.open(strFileName, std::ios_base::in, std::ios_base::binary);

	if (!m_pImpl->m_finStream.is_open())
	{
		SDE_Debug::Instance().OutputLog(
			"Error Input: "
			"Failed to open %s.\n",
			strFileName
		);
		m_pImpl->m_finStream.clear();

		return { nullptr, 0 };
	}

	m_pImpl->m_finStream.seekg(0, std::ios::end);
	int nSize = m_pImpl->m_finStream.tellg();
	char* pData = new char[nSize];

	m_pImpl->m_finStream.seekg(0, std::ios::beg);
	m_pImpl->m_finStream.read(pData, nSize);

	m_pImpl->m_finStream.clear();
	m_pImpl->m_finStream.close();

	return { pData, nSize };
}

void SDE_FileIO::Output(const std::string& strFileName, const SDE_Data& dataWritten, bool isBinary)
{
	if (!isBinary) m_pImpl->m_finStream.open(strFileName, std::ios_base::out);
	else m_pImpl->m_finStream.open(strFileName, std::ios_base::out, std::ios_base::binary);

	if (!m_pImpl->m_foutStream.is_open())
	{
		SDE_Debug::Instance().OutputLog(
			"Error Output: "
			"Failed to open %s.\n",
			strFileName
		);
		m_pImpl->m_foutStream.clear();

		return;
	}

	m_pImpl->m_foutStream.write(dataWritten.GetData(), dataWritten.GetSize());
	m_pImpl->m_foutStream.flush();

	m_pImpl->m_foutStream.clear();
	m_pImpl->m_foutStream.close();

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