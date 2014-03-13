-- Simple moonscript testing

moonscript = require "moonscript"

fn = moonscript.loadstring('print "hi!"')
fn()
