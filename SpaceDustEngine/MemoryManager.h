#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

class MemoryManager
{
public:
	// 为指定大小对象分配空闲内存
	// @param 对象大小
	// @return 指向该空闲内存的指针
	void* Allocate(size_t nSize);

	// 释放指针对应的内存
	// @param pObject: 对象指针
	// @param nSize: 对象大小
	void  Free(void* pObject, size_t nSize);

	// 清空整个内存池
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