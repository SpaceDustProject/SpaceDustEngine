print("Here is test.lua")

SDE_Director.CreateSceneDef("testScene")
SDE_Director.CreateSystemDef("testSystem")

local sceneDef = SDE_Director.GetSceneDef("testScene")
local systemDef = SDE_Director.GetSystemDef("testSystem")

--sceneDef:AddSystemDef(systemDef)

aMeta = getmetatable(sceneDef)
print(aMeta.__name)