defScene = SDE_Director.CreateSceneDef()
SDE_Director.RegisterSceneDef("aDef", defScene)

defScene2 = SDE_Director.GetSceneDef("aDef")
SDE_Director.UnregisterSceneDef("aDef")