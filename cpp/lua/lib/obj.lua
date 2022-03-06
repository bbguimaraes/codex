local fn <const> = require "lib.fn"
local lexer <const> = require "lib.lexer"
local stack <const> = require "lib.stack"
local utils <const> = require "lib.utils"

--- Verifies that `name` is an object-like macro.
local function defined(t, name)
    return t[name:str()] ~= nil
end

--- Registers an object-like macro.
--- \param name Name of the macro, i.e. the text that is to be replaced.
local function define(l, out, t, name, src)
    local name_str <const> = name:str()
    local id <const> = l:peek()
    out.add_step(name_str, "processing object-like macro %q\n", name_str)
    if id == lexer.SPACE then
        l:next()
    elseif id ~= lexer.NEW_LINE then
        error(string.format(
            "invalid token %s %q",
            lexer.to_str(id), utils.escape(name:str())))
    end
    local repl <const> = l:concat_until(l.iter, lexer.NEW_LINE)
    t[name:str()] = repl
    out.add_step(
        string.format("%q", repl),
        "added object-like macro %q with replacement text\n", name:str())
end

--- Unregisters an object-like macro.
--- \param name As passed to \ref define.
local function undef(out, t, name)
    if utils.remove(t, name) == nil then
        return false
    end
    out.add_step(nil, "removed object-like macro %q\n", name)
    return true
end

--- Checks that \p name is an object-like macro and expands it.
--- \param p The parser object, used to recursively expand the replacement text.
--- \returns `nil` or the expanded replacement text.
local function replace(p, out, repl, src, b, e, name)
    if not repl then
        return
    end
    local si <close> = p.stack_item(name)
    local oi <close> = out.stack_item(nil, src, b, e)
    out.add_step(name, "replacing object-like macro %q\n", name)
    out.add_step(
        repl, "replaced object-like macro %q with replacement text\n", name)
    repl = fn.replace_concat2(out, repl)
    out.add_step(repl, "replaced object-like macro %q with %q\n", name, repl)
    local si <close> = p.stack_item(name)
    out.add_step(nil, "recursively expanding output of macro %q\n", name)
    repl = p.replace(repl)
    out.add_step(nil, "macro %q recursively expanded to %q\n", name, repl)
    return repl
end

return {
    defined = defined,
    define = define,
    undef = undef,
    replace = replace,
}
