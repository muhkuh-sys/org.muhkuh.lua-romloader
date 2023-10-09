-- An emulation library for the "Lua Bit Operations Module" (http://bitop.luajit.org/).
-- Found here: https://github.com/moteus/lua-lluv-websocket/blob/master/src/lluv/websocket/bit.lua

local bit = {}

function bit.bnot(a)
  return ~a
end
function bit.bor(a, b, ...)
  a = a | b
  if ... then return bit.bor(a, ...) end
  return a
end
function bit.band(a, b, ...)
  a = a & b
  if ... then return bit.band(a, ...) end
  return a
end
function bit.bxor(a, b, ...)
  a = a ~ b
  if ... then return bit.bxor(a, ...) end
  return a
end
function bit.lshift(a, b)
  return a << b
end
function bit.rshift(a, b)
  return a >> b
end
function bit.bswap(a)
  return ((a & 0x000000ff) << 24) | ((a & 0x0000ff00) << 8) | ((a & 0x00ff0000) >> 8) | ((a & 0xff000000) >> 24)
end
function bit.tohex(a, n)
  n = n or 8
  local cHex = 'x'
  if n<0 then
    cHex = 'X'
    n = -n
  end
  return string.format(string.format('%%0%d%s', n, cHex), a)
end
function bit.tobit(a)
  return a
end

return bit