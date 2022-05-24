#ifndef _SDE_MEMORY_POOL_H_
#define _SDE_MEMORY_POOL_H_

#include <new>

class SDE_MemoryPool
{
public:
	// Ϊָ����С�����������ڴ�
	// @param �����С
	// @return ָ��ÿ����ڴ��ָ��
	void* Allocate(size_t nSize);

	// �ͷ�ָ���Ӧ���ڴ�
	// @param pObject: ����ָ��
	// @param nSize: �����С
	void  Free(void* pObject, size_t nSize);

	// ��������ڴ��
	void  Clear();

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_MemoryPool();
	~SDE_MemoryPool();
};

extern SDE_MemoryPool g_memoryPool;

#endif // !_SDE_MEMORY_POOL_H_