-- Simple moonscript testing

moonscript = require "moonscript"
require "symtc"

function moonscript_test (provider)
   local moonstring = provider:getstring("**********", "moonstring")
   moonscript.loadstring(moonstring)
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], moonscript_test)
else
   symtc.execute(arg[1], moonscript_test)
end
