
require("muhkuh")

a = muhkuh.capture_std(-1, -1)
aCmd = { '/usr/bin/lua', 'exec_test.lua' }
b = a:run(aCmd[1], aCmd)
print(b)

