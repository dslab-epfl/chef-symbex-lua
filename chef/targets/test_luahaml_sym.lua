
require "symtc"

haml = require "haml"
haml_opts = { format = "html5" }
engine = haml.new(options)

function haml_test (provider)
   local str = provider:getstring("**********", "string")
   engine:render(str, { x = "y" })
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], haml_test)
else
   symtc.execute(arg[1], haml_test)
end
