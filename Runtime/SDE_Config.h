#ifndef _SDE_CONFIG_H_
#define _SDE_CONFIG_H_

#define SDE_MAJOR_VERSION	"0"
#define SDE_MINOR_VERSION	"0"
#define SDE_PATCH_LEVEL		"2"

#define SDE_VERSION	"SDE"			\
		SDE_MAJOR_VERSION "."		\
		SDE_MINOR_VERSION "."		\
		SDE_PATCH_LEVEL

#define SDE_VERSION_NUM				\
		SDE_MAJOR_VERSION * 1000 +	\
		SDE_MINOR_VERSION * 100 +	\
		SDE_PATCH_LEVEL

#define SDE_AUCHORS		\
		"hsz, "			\
		"Voidmatrix"	\

#endif // !_SDE_CONFIG_H_