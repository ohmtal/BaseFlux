local tools = {}

function tools.greet() print("Hi") end

function tools.dumpPackage(t)
    for k, v in pairs(t) do
        print(string.format("%-15s type: %s", tostring(k), type(v)))
    end
end

return tools
