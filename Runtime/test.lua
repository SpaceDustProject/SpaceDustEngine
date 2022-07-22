print("Here is SDE_test.lua")

local TestDLL = require("TestDLL")

local systemDef = SDE_Director.CreateSystemDef(
    "testSystem",
    function (system, args)
        system:SetUpdateFunc(
            function (system)
                print(TestDLL.Subtraction(2, 3))
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

local entityDef = SDE_Director.CreateEntityDef(
    "testEntity",
    function (entity, args)
        entity:CreateComponent(componentDef, { x = args.x, y = args.y, angle = args.angle })
    end
)

local sceneDef = SDE_Director.CreateSceneDef(
    "testScene",
    function (scene, args)
        for i = 1, 10, 1 do
            scene:CreateEntity(entityDef, { x = i * 3, y = i * 6, angle = i * 15.0 })
        end
        scene:CreateSystem(systemDef)
    end
)

SDE_Director.RunScene(sceneDef)