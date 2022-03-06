local span <const> = require "lib.span"
local utils <const> = require "lib.utils"

local lexer <const> = {}
lexer.__index = lexer

local UNKNOWN <const> = 0
local NEW_LINE <const> = 1
local SPACE <const> = 2
local DEFINE <const> = 3
local UNDEFINE <const> = 4
local DOUBLE_POUND <const> = 5
local POUND <const> = 6
local OPEN_PARENS <const> = 7
local CLOSE_PARENS <const> = 8
local COMMA <const> = 9
local ELLIPSIS <const> = 10
local IDENTIFIER <const> = 11

local STR = {
    [UNKNOWN] = "UNKNOWN",
    [NEW_LINE] = "NEW_LINE",
    [SPACE] = "SPACE",
    [DEFINE] = "DEFINE",
    [UNDEFINE] = "UNDEFINE",
    [DOUBLE_POUND] = "DOUBLE_POUND",
    [POUND] = "POUND",
    [OPEN_PARENS] = "OPEN_PARENS",
    [CLOSE_PARENS] = "CLOSE_PARENS",
    [COMMA] = "COMMA",
    [ELLIPSIS] = "ELLIPSIS",
    [IDENTIFIER] = "IDENTIFIER",
}

function lexer:new(src)
    return setmetatable({
        src = span.new(src),
        line = 1,
        column = 1,
        queue = {},
        seen = {},
    }, self)
end

function lexer:eof() return self.src:empty() end
function lexer:pos() return self.src:begin() end
local function iter(l) return l:next() end
function lexer:iter() return iter, self end
function lexer:iter_seen() return next, self.seen end

function lexer:iter_peek()
    local i = 0
    return function(l)
        if i then
            if i < #l.queue then
                i = i + 1
                return table.unpack(l.queue[i])
            end
            i = nil
        end
        return l:peek_next()
    end, self
end

local function new_line(self)
    self.line = self.line + 1
    self.column = 1
end

local function ignore_escaped_new_lines(self)
    local src <const> = self.src
    while true do
        local m = src:match("^\\\n")
        if not m then
            return
        end
        new_line(self)
        src:sub(#m + 1)
    end
end

function consume(self, id, m)
    table.insert(self.seen, m)
    local col <const> = self.column
    self.column = self.column + #m
    self.src:sub(#m + 1)
    return id, m, self.line, col
end

function lexer:match()
    local src <const> = self.src
    ignore_escaped_new_lines(self)
    local m = src:match("^\n")
    if m then
        new_line(self)
        return consume(self, NEW_LINE, m)
    end
    m = src:match("^%s+")
    if m then return consume(self, SPACE, m) end
    m = src:match("^#")
    if m then
        local mm = src:match("^define%f[^%w]", 2)
        if mm then return consume(self, DEFINE, m:sub(1, 1 + #mm)) end
        mm = src:match("^undef%f[^%w]", 2)
        if mm then return consume(self, UNDEFINE, m:sub(1, 1 + #mm)) end
        mm = src:match("^#", 2)
        if mm then return consume(self, DOUBLE_POUND, m:sub(1, 2)) end
        return consume(self, POUND, m)
    end
    m = src:match("^%(")
    if m then return consume(self, OPEN_PARENS, m) end
    m = src:match("^%)")
    if m then return consume(self, CLOSE_PARENS, m) end
    m = src:match("^,")
    if m then return consume(self, COMMA, m) end
    m = src:match("^%.%.%.")
    if m then return consume(self, ELLIPSIS, m) end
    m = src:match("^[%w_]+%f[^%w]")
    if m then return consume(self, IDENTIFIER, m) end
    local c <const> = span.new(src)
    c:sub(1, 1)
    return consume(self, UNKNOWN, c)
end

local function next_common(self)
    if not self:eof() then
        return self:match()
    end
end

function lexer:peek_next()
    local ret <const> = {next_common(self)}
    if #ret ~= 0 then
        table.insert(self.queue, ret)
    end
    return table.unpack(ret)
end

function lexer:peek()
    if #self.queue ~= 0 then
        return table.unpack(self.queue[1])
    end
    return self:peek_next()
end

function lexer:next()
    if #self.queue ~= 0 then
        return table.unpack(table.remove(self.queue, 1))
    end
    return next_common(self)
end

function lexer:expect(...)
    local id <const>, str <const> = self:next()
    if utils.find({...}, id) then
        return id, str
    end
    error(string.format("invalid token %s %q", STR[id], str:str()))
end

function lexer:ignore(...)
    if utils.find({...}, (self:peek())) then
        table.remove(self.queue, 1)
    end
end

function lexer:find(...)
    local ids <const> = {...}
    while true do
        local id <const>, str <const> = self:peek()
        if utils.find(ids, id) then
            return id, str
        end
        table.remove(self.queue, 1)
    end
end

function lexer:concat_until(iter, ...)
    local ids <const>, ret <const> = {...}, {}
    for id, str in iter(self) do
        if utils.find(ids, id) then
            break
        end
        table.insert(ret, str:str())
    end
    return table.concat(ret, "")
end

return {
    UNKNOWN = UNKNOWN,
    NEW_LINE = NEW_LINE,
    SPACE = SPACE,
    DEFINE = DEFINE,
    UNDEFINE = UNDEFINE,
    DOUBLE_POUND = DOUBLE_POUND,
    POUND = POUND,
    OPEN_PARENS = OPEN_PARENS,
    CLOSE_PARENS = CLOSE_PARENS,
    COMMA = COMMA,
    ELLIPSIS = ELLIPSIS,
    IDENTIFIER = IDENTIFIER,
    lexer = lexer,
    to_str = function(x) return STR[x] end,
    new = function(...) return lexer:new(...) end,
}
