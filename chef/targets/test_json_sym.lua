-- Symbolic test for the Json module

require "Json"
require "symtc"

function json_test (provider)
   local json = provider:getstring("**********", "json")
   Json.Decode(json)
end

if type(arg[1]) == "string" then
   symtc.replay_time_out = 1
   symtc.replay(arg[1], json_test)
else
   symtc.execute(arg[1], json_test)
end
