#include "SDE_Data.h"
#include "SDE_MemoryPool.h"

class SDE_Data::Impl
{
public:
	char*	pData;
	size_t	nSize;
	int		nRefCount;

public:
	Impl(char* pDataIn, size_t nSizeIn) :
		pData(pDataIn), nSize(nSizeIn), nRefCount(1) {}

	~Impl() { if (pData) delete[] pData; }
};

const char* SDE_Data::GetData() const
{
	return m_pImpl->pData;
}

size_t SDE_Data::GetSize() const
{
	return m_pImpl->nSize;
}

SDE_Data::SDE_Data(char* pData, size_t nSize)
{
	void* pMem = g_memoryPool.Allocate(sizeof(Impl));
	m_pImpl = new (pMem) Impl(pData, nSize);
}

SDE_Data::SDE_Data(const SDE_Data& dataCopy)
{
	m_pImpl->nRefCount++;
	m_pImpl = dataCopy.m_pImpl;
}

SDE_Data& SDE_Data::operator=(const SDE_Data& dataCopy)
{
	m_pImpl->nRefCount++;
	m_pImpl = dataCopy.m_pImpl;
}

SDE_Data::~SDE_Data()
{
	if (--m_pImpl->nRefCount <= 0)
	{
		m_pImpl->~Impl();
		g_memoryPool.Free(m_pImpl, sizeof(Impl));
	}
}