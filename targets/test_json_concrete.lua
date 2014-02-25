-- Symbolic test for the Json module

require "Json"

local t = { 
   ["name1"] = "value1",
   ["name2"] = {1, false, true, 23.54, "a \021 string"},
   name3 = Json.Null() 
}

local json = Json.Encode(t)
print(json)

local t = Json.Decode(json)
print(t.name2[4])
