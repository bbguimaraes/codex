local stack <const> = {
    new = function(self) return setmetatable({}, self) end,
    push = table.insert,
    pop = table.remove,
}
stack.__index = stack

local base_item <const> = {
    __close = function(self) self.t:pop() end,
}
base_item.__index = base_item

local item <const> = setmetatable({
    new = function(self, t, name)
        t:push({name, {}})
        return setmetatable({t = t}, self)
    end,
}, base_item)
item.__index = item
item.__close = item.__close

local output_item <const> = setmetatable({
    new = function(self, t, name, src, b, e)
        t:push(name, src:sub(1, b - 1), src:sub(e + 1))
        return setmetatable({t = t}, self)
    end,
}, base_item)
output_item.__index = output_item
output_item.__close = output_item.__close

return {
    stack = stack,
    item = item,
    output_item = output_item,
}
