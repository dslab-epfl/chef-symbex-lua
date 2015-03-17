-- Test whether making a symbolic string works properly.

local x = symbex.symstring("abcd", "test")
local message = "didn't work"

if x == "cdef" then
   message = "it worked!"
   print("yay")
else
   print("nay")
end

symbex.killstate(0, message)
