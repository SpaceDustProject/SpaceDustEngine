#ifndef _SDE_FILE_IO_H_
#define _SDE_FILE_IO_H_

#include "SDE_Data.h"

class SDE_FileIO
{
public:
	// 读取指定文件构造 SDE_Data 对象
	SDE_Data Input(const char* strFileName, bool isBinary = true);

	// 将 SDE_Data 对象写为指定文件
	void Output(const char* strFileName, const SDE_Data& dataWritten, bool isBinary = true);

private:
	class Impl;
	Impl* m_pImpl;

public:
	~SDE_FileIO();
	SDE_FileIO(const SDE_FileIO&) = delete;
	SDE_FileIO& operator=(const SDE_FileIO&) = delete;
	static SDE_FileIO& Instance()
	{
		static SDE_FileIO instance;
		return instance;
	}
private:
	SDE_FileIO();
};

#endif // !_SDE_FILE_IO_H_