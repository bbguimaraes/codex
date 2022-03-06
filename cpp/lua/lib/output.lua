local stack <const> = require "lib.stack"
local utils <const> = require "lib.utils"

local COLOR <const> = "\x1b[38;5;240m"
local COLOR_BOLD <const> = "\x1b[1m"
--local COLOR_RESET <const> = "\x1b[m\x0f"
local COLOR_RESET <const> = "\x1b[m"

local DEBUG_STACK <const> = false

local nop_output <const> = {
    new = function(self) return setmetatable({}, self) end,
    push = function(...) end,
    pop = function(...) end,
    add_step = function(...) return "" end,
    dump = function(...) end,
}
nop_output.__index = nop_output

local output <const> = {
    new = function(self, color)
        return setmetatable({
            color = color,
            stack = stack.stack:new(),
            steps = {},
        }, self)
    end,
    push = function(self, ...) self.stack:push({...}) end,
    pop = function(self) self.stack:pop() end,
}
output.__index = output

function output:add_step(stack, src, fmt, ...)
    local pre = {string.rep(" ", 4 * (#self.stack - 1))}
    if DEBUG_STACK then
        for i = 2, #stack do
            table.insert(pre, stack[i][1])
            table.insert(pre, " | ")
        end
    end
    pre = table.concat(pre, "")
    local t <const> = {}
    table.insert(t, string.format("%d. " .. fmt, #self.steps, ...))
    if src then
        src = src:gsub("\n", "\n  " .. pre)
        local t0 <const>, t1 <const> = {}, {}
        for i = 1, #self.stack do
            table.insert(t0, self.stack[i][2])
        end
        for i = #self.stack, 1, -1 do
            table.insert(t1, self.stack[i][3])
        end
        if self.color then
            table.insert(t, string.format(
                "  %s%s%s%s%s%s%s%s",
                pre, COLOR, table.concat(t0, ""),
                COLOR_RESET, src, COLOR, table.concat(t1, ""), COLOR_RESET))
        else
            table.insert(t, string.format(
                "  %s%s|%s|%s",
                pre, table.concat(t0, ""), src, table.concat(t1, "")))
        end
        table.insert(t, "")
        table.insert(t, "")
    end
    table.insert(self.steps, string.format("%s%s", pre, table.concat(t, "\n")))
end

local function dump_fn_macro(name, t, f)
    local params <const>, repl <const> = table.unpack(t)
    f:write(string.format("  %q:\n", name))
    f:write("    parameters:\n")
    for _, x in ipairs(params) do
        f:write(string.format("    - %q\n", x:str()))
    end
    f:write(string.format("    replacement: %q\n", repl))
end

local function dump_obj_macro(name, repl, f)
    f:write(string.format("  %q:\n", name))
    f:write(string.format("    replacement: %q\n", repl))
end

function output:dump(src, parser, f)
    f:write("=== macros ===\n")
    if next(parser.fn) then
        f:write("\nfunction:\n")
        for name, t in pairs(parser.fn) do
            dump_fn_macro(name, t, f)
        end
    end
    if next(parser.obj) then
        f:write("\nobject:\n")
        for name, repl in pairs(parser.obj) do
            dump_obj_macro(name, repl, f)
        end
    end
    if #self.steps ~= 0 then
        f:write("\n")
        f:write("=== steps ===\n\n")
        for _, x in ipairs(self.steps) do
            f:write(x)
        end
    end
    f:write("\n")
    f:write("=== input ===\n\n", src:str(), "\n")
    f:write("=== output ===\n\n")
end

return {
    nop_output = nop_output,
    output = output,
}
