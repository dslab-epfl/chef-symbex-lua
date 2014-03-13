-- Symbolic test for the luajson module

-- Mind the lower-case 'j'
require "json"
require "symtc"

function json_test(provider)
   local json_str = provider:getstring("**********", "json")
   json.decode(json_str)
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], json_test)
else
   symtc.execute(arg[1], json_test)
end
