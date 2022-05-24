#ifndef _SDE_DATA_H_
#define _SDE_DATA_H_

class SDE_Data
{
public:
	const char* GetData() const;	// 获取数据
	int			GetSize() const;	// 获取数据长度

private:
	class Impl;
	Impl* m_pImpl;

public:
	SDE_Data(char* pData, int nSize);
	SDE_Data(const SDE_Data& dataCopy);
	SDE_Data& operator=(const SDE_Data& dataCopy);
	~SDE_Data();
};

#endif // !_SDE_DATA_H_