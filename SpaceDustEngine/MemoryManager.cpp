#include "MemoryManager.h"

#include <string.h>

// 单个区块的内存大小
static const int s_nChunkSize = 16 * 1024;

// 开辟新区块时单次开辟的区块总数
static const int s_nChunkIncrement = 128;

// 不同内存块大小的区块种类总数
static const int s_nChunkSizeCount = 5;

// 所有种类区块的内存块大小
static const int s_arrBlockSize[s_nChunkSizeCount] = {
	16, 32, 64, 128, 256
};

// 内存块最大尺寸
static const int s_nMaxBlockSize = 256;

class MemoryManager::Impl
{
public:
	struct Block
	{
		Block*	pNext;
	};

	struct ChunkInfo
	{
		int		nChunkSize;
		Block*	pBlocks;
	};

	int			m_nChunkCount;
	int			m_nChunkSpace;
	ChunkInfo*	m_arrChunkInfo;

	int			m_arrSizeMap[s_nMaxBlockSize + 1];
	Block*		m_arrFreeBlockHead[s_nChunkSizeCount];

public:
	Impl()
	{
		m_nChunkCount = 0;
		m_nChunkSpace = s_nChunkIncrement;
		m_arrChunkInfo = new ChunkInfo[m_nChunkSpace];

		memset(m_arrChunkInfo, 0, m_nChunkSpace * sizeof(Block));
		memset(m_arrFreeBlockHead, 0, sizeof(m_arrFreeBlockHead));

		m_arrSizeMap[0] = 0;
		int nCurrentSizeIndex = 0;
		for (int i = 1; i <= s_nMaxBlockSize; i++)
		{
			if (i > s_arrBlockSize[nCurrentSizeIndex]) {
				nCurrentSizeIndex++;
			}
			m_arrSizeMap[i] = nCurrentSizeIndex;
		}
	}
	~Impl()
	{
		for (int i = 0; i < m_nChunkCount; i++)
		{
			delete[] m_arrChunkInfo[i].pBlocks;
		}
		delete[] m_arrChunkInfo;
	}
};

void* MemoryManager::Allocate(size_t nSize)
{
	if (nSize == 0)
	{
		return nullptr;
	}

	if (nSize > s_nMaxBlockSize)
	{
		return new char[nSize];
	}

	// 找到适合该对象的区块种类
	int index = m_pImpl->m_arrSizeMap[nSize];

	// 如果该类区块有空余位置，则可返回其指针
	if (m_pImpl->m_arrFreeBlockHead[index])
	{
		Impl::Block* pBlock = m_pImpl->m_arrFreeBlockHead[index];
		m_pImpl->m_arrFreeBlockHead[index] = pBlock->pNext;

		return pBlock;
	}
	// 如果该类区块没有空余位置，则开辟指定数量的新区块
	else
	{
		// 如果区块信息表不够用了，则开辟一张更大的信息表
		if (m_pImpl->m_nChunkCount == m_pImpl->m_nChunkSpace)
		{
			Impl::ChunkInfo* arrOldChunk = m_pImpl->m_arrChunkInfo;

			m_pImpl->m_nChunkSpace += s_nChunkIncrement;
			m_pImpl->m_arrChunkInfo = new Impl::ChunkInfo[m_pImpl->m_nChunkSpace];

			// 转移旧区块信息并初始化新区块信息
			memcpy(m_pImpl->m_arrChunkInfo, arrOldChunk, m_pImpl->m_nChunkCount * sizeof(Impl::ChunkInfo));
			memset(m_pImpl->m_arrChunkInfo + m_pImpl->m_nChunkCount, 0, s_nChunkIncrement * sizeof(Impl::ChunkInfo));
			delete[] arrOldChunk;
		}

		// 开辟新区块并初始化
		Impl::ChunkInfo* pChunkInfo = m_pImpl->m_arrChunkInfo + m_pImpl->m_nChunkCount;
		pChunkInfo->pBlocks = (Impl::Block*)new char[s_nChunkSize];
		pChunkInfo->nChunkSize = s_arrBlockSize[index];

		uintptr_t nBlockCount = s_nChunkSize / pChunkInfo->nChunkSize;
		for (uintptr_t i = 0; i < nBlockCount - 1; i++)
		{
			Impl::Block* pBlock = (Impl::Block*)((uintptr_t)pChunkInfo->pBlocks + i * pChunkInfo->nChunkSize);
			Impl::Block* pNext = (Impl::Block*)((uintptr_t)pChunkInfo->pBlocks + (i + 1) * pChunkInfo->nChunkSize);
			pBlock->pNext = pNext;
		}
		Impl::Block* pLastBlock = (Impl::Block*)((uintptr_t)pChunkInfo->pBlocks + (nBlockCount - 1) * pChunkInfo->nChunkSize);
		pLastBlock->pNext = nullptr;
		m_pImpl->m_nChunkCount++;

		return pChunkInfo->pBlocks;
	}
}

void MemoryManager::Free(void* pObject, size_t nSize)
{
	if (nSize == 0)
	{
		return;
	}

	if (nSize > s_nMaxBlockSize)
	{
		delete[] pObject;
		return;
	}

	int index = m_pImpl->m_arrSizeMap[nSize];
	Impl::Block* pBlock = (Impl::Block*)pObject;
	pBlock->pNext = m_pImpl->m_arrFreeBlockHead[index];
	m_pImpl->m_arrFreeBlockHead[index] = pBlock;
}

void MemoryManager::Clear()
{
	for (int i = 0; i < m_pImpl->m_nChunkCount; i++)
	{
		delete[] m_pImpl->m_arrChunkInfo[i].pBlocks;
	}
	m_pImpl->m_nChunkCount = 0;

	memset(m_pImpl->m_arrChunkInfo, 0, m_pImpl->m_nChunkSpace * sizeof(Impl::ChunkInfo));
	memset(m_pImpl->m_arrFreeBlockHead, 0, sizeof(m_pImpl->m_arrFreeBlockHead));
}

MemoryManager::MemoryManager()
{
	m_pImpl = new Impl();
}

MemoryManager::~MemoryManager()
{
	delete m_pImpl;
}