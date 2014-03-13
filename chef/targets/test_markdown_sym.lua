
markdown = require "markdown"
require "symtc"

function markdown_test (provider)
   local mkdown = provider:getstring("**********", "string")
   markdown(mkdown)
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], markdown_test)
else
   symtc.execute(arg[1], markdown_test)
end
