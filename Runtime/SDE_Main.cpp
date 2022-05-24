#include "SDE_Settings.h"
#include "SDE_VirtualMachine.h"
#include "SDE_FileIO.h"

int main(int argc, char* argv[])
{
	// ∂¡»°≈‰÷√Œƒº˛
	SDE_Data dataConfig = SDE_FileIO::Instance().Input("config.json", false);
	cJSON* pJSONConfigRoot = cJSON_Parse(dataConfig.GetData());
	g_settings.ParseConfig(pJSONConfigRoot);

	return 0;
}