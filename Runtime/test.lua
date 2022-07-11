print("Here is SDE_test.lua")

local systemDef = SDE_Director.CreateSystemDef(
    "testSystem",
    function (system, args)
        print(system:GetSystemName())

        system:SetInitFunc(
            function ()
                print("Here is init func")
            end
        )
        system:SetUpdateFunc(
            function ()
                
            end
        )
        system:SetQuitFunc(
            function ()
                print("Here is quit func")
            end
        )
    end
)

print(systemDef:GetName())

local sceneDef = SDE_Director.CreateSceneDef(
    "testScene",
    function (scene, args)
        print(scene:GetSceneName())
        scene:CreateSystem(systemDef)
    end
)

local scene = SDE_Director.CreateScene(sceneDef)
SDE_Director.SwitchScene(scene)