local utils <const> = require "lib.utils"

local span <const> = {
    __len = function(self) return self.e - self.b end,
}
span.__index = span

local function check(s)
    assert(s._s)
    assert(s.b)
    assert(s.e)
    assert(0 < s.b)
    assert(0 < s.e)
    assert(s.b <= s.e)
    assert(s.b <= #s._s + 1)
    assert(s.e <= #s._s + 1)
end

local function new(self, t)
    check(t)
    return setmetatable(t, self)
end

function span:new(s, b, e)
    if self == getmetatable(s) then
        b = b and s.b + b - 1 or s.b
        e = e and (s.b + e) or s.e
        return new(self, {_s = s._s, b = b, e = e})
    else
        b = b or 1
        e = e or #s + 1
        return new(self, {_s = s, b = b, e = e})
    end
end

function span:begin() return self.b end
function span:end_() return self.e end
function span:empty() return self.b == self.e end

function span:__tostring()
    return string.format(
        "span<%d, %d, %q>",
        self.b, self.e, utils.escape(self:str()))
end

function span:str(b, e)
    b = b or 1
    e = e and (self.b + e - 1) or (self.e - 1)
    return self._s:sub(self.b + b - 1, e)
end

function span:find(p, b, plain)
    local ret <const> = {self._s:find(p, self.b + (b or 1) - 1, plain)}
    if #ret ~= 0 then
        ret[1] = ret[1] - self.b + 1
        ret[2] = ret[2] - self.b + 1
    end
    return table.unpack(ret)
end

function span:sub(b, e)
    local max <const> = #self._s + 1
    self.b = math.min(max, self.b + b - 1)
    if e then
        self.e = math.min(max, self.b + e)
    end
    check(self)
    return self
end

function span:match(p, b)
    local b <const>, e <const> = self._s:find(p, self.b + (b or 1) - 1)
    if not b or self.e <= b or self.e <= e then
        return
    end
    return new(getmetatable(self), {_s = self._s, b = b, e = e + 1})
end

return {
    new = function(...) return span:new(...) end,
}
