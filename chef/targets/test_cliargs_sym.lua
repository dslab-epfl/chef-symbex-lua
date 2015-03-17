
require "symtc"

local cli = require "cliargs"
cli:set_name("symtest.lua")

function cli_test (provider)
   local old_arg = arg
   local sym_template = "**"

   arg = { provider:getstring(sym_template, "arg1"),
	   provider:getstring(sym_template, "arg2") }

   cli:add_argument(provider:getstring(sym_template, "req_arg"), "required argument")
   cli:optarg(provider:getstring(sym_template, "opt_arg"), "optional argument")
   cli:add_option(provider:getstring(sym_template, "option"), "option")
   cli:add_flag(provider:getstring(sym_template, "flag"), "flag")

   cli:parse_args()

   arg = old_arg
end

if type(arg[1]) == "string" then
   symtc.replay(arg[1], cli_test)
else
   symtc.execute(arg[1], cli_test)
end
