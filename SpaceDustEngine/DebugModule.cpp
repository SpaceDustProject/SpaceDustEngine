#include "DebugModule.h"
#include "MemoryManager.h"

#include <new>
#include <stdio.h>
#include <stdarg.h>

void DebugModule::Output(const char* strFormat, ...)
{
	va_list valist;
	va_start(valist, strFormat);
	vprintf(strFormat, valist);
	va_end(valist);
}