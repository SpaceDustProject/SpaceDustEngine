print("Here is SDE_test.lua")

local Ether = require("Ether")

local testSystemDef = SDE_Director.CreateSystemDef(
    "testSystem",
    function (system, args)
        system:SetStartFunc(
            function (system)
                print("system start")
            end
        )

        system:SetUpdateFunc(
            function (system)
                print("update")
            end
        )

        system:SetStopFunc(
            function (system)
                print("system stop")
            end
        )
    end
)

local inputSystemDef = SDE_Director.CreateSystemDef(
    "InputSystem",
    function (system, args)
        system:SetUpdateFunc(
            function (system)
                while Ether.Input.UpdateEvent() do
                    if Ether.Input.GetEventType() == Ether.Input.EVENT_QUIT then
                        print("bye bye")
                    elseif Ether.Input.GetEventType() == Ether.Input.EVENT_MOUSEBTNUP then
                        if Ether.Input.GetMouseButtonID() == Ether.Input.MOUSEBTN_LEFT then
                            system:GetScene():GetSystem("testSystem"):Start()
                        elseif Ether.Input.GetMouseButtonID() == Ether.Input.MOUSEBTN_RIGHT then
                            system:GetScene():GetSystem("testSystem"):Stop()
                        end 
                    end
                end
            end
        )
    end
)

local componentDef = SDE_Director.CreateComponentDef(
    "testComponent",
    function (component, args)
        component:SetValue("x", args.x or 0)
        component:SetValue("y", args.y or 0)
        component:SetValue("angle", args.angle or 0.0)
    end
)

local sceneDef = SDE_Director.CreateSceneDef(
    "testScene",
    function (scene, args)
        Ether.Window.Create("testWindow", { x = Ether.Window.DEFAULT_POS, y = Ether.Window.DEFAULT_POS, w = 800, h = 600 })
        scene:CreateSystem(inputSystemDef)
        scene:CreateSystem(testSystemDef)
    end
)

SDE_Director.RunScene(sceneDef)