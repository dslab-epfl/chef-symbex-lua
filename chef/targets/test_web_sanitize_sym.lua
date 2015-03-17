
web_sanitize = require "web_sanitize"
require "symtc"

function sanitize_test (provider)
   local html = provider:getstring("**********", "html")
   web_sanitize.sanitize_html(html)
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], sanitize_test)
else
   symtc.execute(arg[1], sanitize_test)
end
