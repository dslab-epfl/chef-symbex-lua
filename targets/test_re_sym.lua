-- Concrete test for Lpeg's re package

require "re"
require "symtc"

function re_test (provider)
   local pattern = provider:getstring("          ", "pattern")
   re.compile(pattern)
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], re_test)
else
   symtc.execute(arg[1], re_test)
end
