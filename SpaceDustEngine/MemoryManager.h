#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

class MemoryManager
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
	~MemoryManager();
	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;
	static MemoryManager& Instance()
	{
		static MemoryManager instance;
		return instance;
	}
private:
	MemoryManager();
};

#endif // !_MEMORY_MANAGER_H_