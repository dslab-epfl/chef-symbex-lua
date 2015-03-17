-- Execution and replay support for symbolic test cases
-- Copyright 2013 EPFL.  All rights reserved.

local lpeg = require("lpeg")
lpeg.locale(lpeg)

local Json = require("Json")

symtc = {}
symtc.replay_time_out = 0

-- The symbolic input provider
local symprovider = {}

function symprovider:getstring (defvalue, name)
   return symbex.symstring(defvalue, name)
end

function symprovider:getint (defvalue, name)
   return symbex.symint(defvalue, name)
end

function symtc.execute(max_time, fn, ...)
   pcall(fn, symprovider, ...)
   symbex.killstate(0, "done")
end

-- The replay provider
local replayprovider = {}

function replayprovider:new (assignments)
   assignments = assignments or {}
   setmetatable(assignments, self)
   self.__index = self
   return assignments
end

function replayprovider:getstring (defvalue, name)
   return self[name]
end

replayprovider.getint = replayprovider.getstring

local timestamp = lpeg.R("09")^1

local hexdigit = lpeg.R("09", "af", "AF")
local hexvalue = lpeg.P("0x") * (hexdigit^1)
local fork_pc = hexvalue + lpeg.P("0")

local esc_hex = lpeg.P("\\x") * lpeg.C(hexdigit * hexdigit) * lpeg.Cc(16) / tonumber / string.char
local esc_char = lpeg.P("\\\\") / "\\" + 
                 lpeg.P("\\t") / "\t" +
                 lpeg.P("\\n") / "\n" +
                 lpeg.P("\\\"") / "\""
local reg_char = lpeg.P(1) - lpeg.P('"')

local esc_string = lpeg.P('"') * lpeg.Cs((esc_hex + esc_char + reg_char)^0) * lpeg.P('"')

local name = lpeg.C((lpeg.alnum + lpeg.P("_"))^1)

local assgn = lpeg.Cg(name * lpeg.P("=>") * esc_string)

local space = lpeg.P(" ")^1

local tc_line = timestamp * space * fork_pc * lpeg.Cf(lpeg.Ct("") * (space * assgn)^0, rawset) * space^-1 * -1

function symtc.replay(repfile, fn, ...)
   local errors = {}
   local i = 1
   local start_time = os.time()

   -- We need to chain our hook so it doesn't replace the
   -- coverage measurement hook.

   local old_hook, old_mask, old_count = debug.gethook()
   if symtc.replay_time_out > 0 then
      debug.sethook(
	 function (...)
	    if old_hook then
	       old_hook(...)
	    end

	    local now = os.time()
	    if now > start_time + symtc.replay_time_out then
	       -- First, reset the time, so the hook stops triggering
	       -- in the handler code.
	       start_time = now
	       io.stderr:write("Replay timed out\n")
	       error("Replay timed-out")
	    end
	 end, old_mask, old_count)
   end

   for line in io.lines(repfile) do
      local assignments = tc_line:match(line)
      if assignments ~= nil then
	 provider = replayprovider:new(assignments)
	 start_time = os.time()
	 local status, err = pcall(fn, provider, ...)
	 if not status then
	    errors[i] = {
	       err,
	       assignments,
	       debug.traceback()
	    }
	    i = i + 1
	 end
      end
   end

   if symtc.replay_time_out > 0 and old_hook then
      debug.sethook(old_hook, old_mask, old_count)
   end

   local json = Json.Encode({ ["errors"] = errors })
   local f = assert(io.open("errors.json", "w"))
   f:write(json)
   f:close()
end

return symtc
