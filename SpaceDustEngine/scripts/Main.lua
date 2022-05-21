register = require("SDE_Register")

Transform = {}
Transform.x = 0.0
Transform.y = 0.0

register.RegisterComponent("Transform", Transform)
defTransform = register.GetComponent("Transform")

for key, value in pairs(defTransform) do
    print(key, value)
end

defTransform.x = 1.0

for key, value in pairs(defTransform) do
    print(key, value)
end