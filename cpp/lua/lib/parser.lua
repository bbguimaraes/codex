local fn <const> = require "lib.fn"
local lexer <const> = require "lib.lexer"
local obj <const> = require "lib.obj"
local span <const> = require "lib.span"
local stack <const> = require "lib.stack"
local utils <const> = require "lib.utils"

local parser <const> = {}
parser.__index = parser

--- Initializes a new parser.
--- \param src \ref span containing the complete source code fragment.
--- \param output Debug output stack/handler.  \see lib.output
function parser:new(lexer, output)
    local ret <const> = {
        lexer = lexer,
        fn = {},
        obj = {},
        stack = stack.stack:new(),
        output = output,
        out = "",
    }
    -- Interfaces for other modules.
    ret.interface = {
        replace = function(...) return ret:replace(...) end,
        stack_item = function(name)
            return stack.item:new(ret.stack, name)
        end,
    }
    ret.out_interface = {
        add_step = function(...) return ret:add_step(...) end,
        stack_item = function(...)
            return stack.output_item:new(ret.output, ...)
        end,
    }
    return setmetatable(ret, self)
end

--- Adds details about a step to the output.
--- \param src The section of the source currently being examined.
function parser:add_step(src, fmt, ...)
    self.output:add_step(self.stack, src, fmt, ...)
end

--- Removes all C/++ comments from `src`.
function parser.remove_comments(src)
    return (src:str():gsub("//.-*\n", ""):gsub("/*.-*/", ""))
end

--- Public entry point, processes the entire input and populates `self.out`.
function parser:parse()
    local l <const>, out <const> = self.lexer, {}
    while not l:eof() do
        local id <const>, str <const> = l:peek()
        if id == lexer.DEFINE then
            self:parse_define()
        elseif id == lexer.UNDEFINE then
            self:parse_undef()
        else
            str = self.lexer:concat_until(lexer.NEW_LINE)
            self:process_source(id, str, out)
        end
    end
    self.out = table.concat(out, "\n")
    assert(not self.lexer:peek())
end

--- Processes the name/replacement portion of a `#define` line.
function parser:parse_define()
    local l <const> = self.lexer
    self:add_step(
        l:concat_until(l.iter_peek, lexer.NEW_LINE),
        "processing #define line\n")
    l:expect(lexer.DEFINE)
    l:expect(lexer.SPACE)
    local _, ident <const> = l:expect(lexer.IDENTIFIER)
    if fn.defined(self.fn, ident) or obj.defined(self.obj, ident) then
        error("macro redefined: " .. ident:str())
    end
--    local si <close> = stack.output_item:new(self.output, nil, "TODO", 1, 1)
    if l:peek() == lexer.OPEN_PARENS then
        fn.define(l, self.out_interface, self.fn, ident, "TODO")
    else
        obj.define(l, self.out_interface, self.obj, ident, "TODO")
    end
end

--- Processes the name portion of an `#undef` line.
function parser:parse_undef()
    local l <const> = self.lexer
    self:add_step(
        l:concat_until(l.iter_peek, lexer.NEW_LINE),
        "processing #undef line\n")
    l:expect(lexer.UNDEFINE)
    l:expect(lexer.SPACE)
    local _, name = l:expect(lexer.IDENTIFIER)
    l:expect(lexer.NEW_LINE)
    name = name:str()
    return obj.undef(self.out_interface, self.obj, name)
        or fn.undef(self.out_interface, self.fn, name)
        or error(string.format("undefined macro %q in #undef\n", name))
end

--- Processes a source block without preprocessor directives.
function parser:process_source(id, src, out)
    self:add_step(src, "processing source block\n")
    local si <close> = stack.item:new(self.stack)
    table.insert(out, self:replace(src))
end

--- Performs a complete, recursive replacement of a source fragment.
--- \param is_arg \see check_recursion
function parser:replace(src, is_arg)
    local ret <const> = {}
    local i = 1
    while true do
        local b <const>, e, name <const> = src:find("%f[%w_]([%w_]+)%f[^%w_]")
        if not b then
            table.insert(ret, src)
            return table.concat(ret, "")
        end
        local repl
        if not self:check_recursion(src, b, e, name, is_arg) then
            repl, e = fn.replace(
                self.interface, self.out_interface,
                self.fn[name], src, b, e, name)
            if not repl then
                repl = obj.replace(
                    self.interface, self.out_interface,
                    self.obj[name], src, b, e, name)
            end
        end
        if repl then
            table.insert(ret, src:sub(1, b - 1))
            -- TODO handle string literals properly
            if repl:sub(1, 1) == '"' then
                table.insert(ret, repl)
                src = src:sub(e + 1)
            else
                src = repl .. src:sub(e + 1)
            end
        else
            table.insert(ret, src:sub(1, e))
            src = src:sub(e + 1)
        end
    end
end

--- Checks whether the expansion of `name` is recursive and should be prevented.
--- According to the C standard and the GCC documentation, if a macro is found
--- again during its own replacement, it is not expanded.  Additionally, it is
--- marked as already expanded such that further expansions of the replacement
--- text also do not expand it.  This function performs both the verification
--- and the marking.
--- \param is_arg
---     An exception to the rule above is when recursion happens during the
---     expansion of the arguments of a macro, in which case this parameter
---     is set to `true`.
--- \returns `true` if the expansion should be prevented.
function parser:check_recursion(src, b, e, name, is_arg)
    for _, x in ipairs(self.stack) do
        if not x[2][name] then
            goto continue
        end
        local si <close> = stack.output_item:new(self.output, name, src, b, e)
        self:add_step(src:sub(b, e), "macro %q already expanded\n", name)
        do return true end
        ::continue::
    end
    if is_arg then
        return
    end
    for _, x in ipairs(self.stack) do
        if x[1] ~= name then
            goto continue
        end
        local si <close> = stack.output_item:new(self.output, name, src, b, e)
        self:add_step(src:sub(b, e), "macro %q already expanded\n", name)
        for _, x in ipairs(self.stack) do
            x[2][name] = true
        end
        do return true end
        ::continue::
    end
end

return {
    parser = parser,
}
