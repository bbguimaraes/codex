local lexer <const> = require "lib.lexer"
local stack <const> = require "lib.stack"
local utils <const> = require "lib.utils"

--- Verifies that `name` is a function-like macro.
local function defined(t, name)
    return t[name:str()] ~= nil
end

local parse_parameters

--- Registers a function-like macro.
--- \param name Name of the macro, i.e. the text that is to be replaced.
--- \param repl_b Beginning of the replacement text.
local function define(l, out, t, name, src, b, e, repl_b)
    local name_str <const> = name:str()
    l:expect(lexer.OPEN_PARENS)
    local params <const>, is_var <const> = parse_parameters(l, src, repl_b)
    l:ignore(lexer.SPACE)
    local repl <const> = l:concat_until(l.iter, lexer.NEW_LINE)
    t[name_str] = {params, repl, is_var}
    local params_str <const> = {}
    for _, x in ipairs(params) do
        table.insert(params_str, x:str())
    end
    out.add_step(
        name_str,
        "added function-like macro %q"
            .. " with parameters %q and replacement %q\n",
        name_str, table.concat(params_str, ", "), repl)
end

--- Processes the parameter section of a function-like macro definition.
function parse_parameters(l)
    local ret <const> = {}
    local is_var = false
    while true do
        local id, name = l:expect(
            lexer.IDENTIFIER, lexer.ELLIPSIS, lexer.CLOSE_PARENS)
        if id == lexer.CLOSE_PARENS then
            break
        end
        is_var = is_var or (id == lexer.ELLIPSIS)
        table.insert(ret, name)
        if l:expect(lexer.COMMA, lexer.CLOSE_PARENS) == lexer.CLOSE_PARENS then
            break
        end
        l:ignore(lexer.SPACE)
    end
    return ret, is_var
end

local function undef(out, t, name)
    if utils.remove(t, name) == nil then
        return false
    end
    out.add_step(nil, "removed function-like macro %q", name)
    return true
end

local replace0

--- Checks that a function-like macro call is valid and expands it.
--- TODO document range
--- \param p
---     The parser object, used to recursively expand the arguments and
---     replacement text.
--- \param name Name of the macro as passed to \ref define.
--- \returns Two values.
---     - If \p name is not a function-like macro or the range is not a valid
---       macro call, `nil, e`
---     - Otherwise, the replaced text and the position in `src` right after the
---       original argument list.
function replace(p, out, t, src, b, e, name)
    if not t then
        return nil, e
    end
    local _, e1, args = src:find("^%s*(%b())", e + 1)
    if not args then
        return nil, e
    end
    local si <close> = p.stack_item(name)
    return replace0(p, out, src, t, name, args:sub(2, -2)), e1
end

local replace_info
local replace_concat0, replace_concat2, replace_args

function replace0(p, out, src, t, name, args)
    out.add_step(src, "replacing invocation of macro %q\n", name)
    local repl <const>, sub <const> = replace_info(t, name, args)
    out.add_step(repl, "replaced macro %q with replacement text\n", name)
    local ret = replace_concat0(out, repl, sub)
    ret = replace_concat2(out, ret)
    ret = replace_args(p, out, ret, sub)
    out.add_step(ret, "replaced function-like macro %q with %q\n", name, ret)
    -- XXX take subsequent tokens into consideration
    -- make sure to integrate with recursive expansion detection (not trivial!)
    -- TODO properly handle string literals
    if ret:sub(1, 1) == '"' then
        return ret
    end
    out.add_step(nil, "recursively expanding output of macro %q\n", name)
    ret = p.replace(ret)
    out.add_step(nil, "macro %q recursively expanded to %q\n", name, ret)
    return ret
end

local fn_args_map, sub_va_opt

function replace_info(t, name, args_str)
    local params <const>, repl <const>, is_var <const> = table.unpack(t)
    local args <const> = utils.split(args_str, "[^,]+")
    local sub <const> = fn_args_map(name, args, args_str, params, is_var)
    if is_var then
        return sub_va_opt(repl, #args), sub
    else
        return repl, sub
    end
end

local check_argc

function fn_args_map(name, args, args_str, params, is_var)
    check_argc(name, args_str, #params, #args, is_var)
    local ret <const> = {}
    for i, x in ipairs(args) do
        local p <const> = params[i]:str()
        if p == "..." then
            ret["__VA_ARGS__"] = table.concat(args, ", ", i)
            break
        end
        ret[p] = x
    end
    return ret
end

function check_argc(name, s, n_params, n_args, is_var)
    if is_var then
        if n_args < n_args then
            error(string.format(
                "incorrect number of arguments for macro %q (%d < %d): %s",
                name, n_args, n_params, s))
        end
    else
        if n_args ~= n_params then
            error(string.format(
                "incorrect number of arguments for macro %q (%d != %d): %s",
                name, n_args, n_params, s))
        end
    end
end

function sub_va_opt(s, argc)
    if argc == 0 then
        return s:gsub("%f[%w_]__VA_OPT__(%b())", "")
    else
        return s:gsub("%f[%w_]__VA_OPT__(%b())", function(x)
            return x:sub(2, #x - 1)
        end)
    end
end

local replace_concat1

function replace_concat0(out, src, sub)
    src = src:gsub("()%f[%w_]([%w_]+)()(%s*##)", function(b, name, e, tail)
        local ret <const> =
            replace_concat1(out, src, b, e, sub, name, "left")
        return ret and (ret .. tail)
    end)
    src = src:gsub("(##%s*)()([%w_]+)%f[^%w_]()", function(head, b, name, e)
        local ret <const> =
            replace_concat1(out, src, b, e, sub, name, "right")
        return ret and (head .. ret)
    end)
    return src
end

function replace_concat1(out, src, b, e, sub, name, side)
    local ret <const> = sub[name]
    if not ret then
        return
    end
    e = e - 1
    local oi <close> = out.stack_item(nil, src, b, e)
    out.add_step(
        src:sub(b, e),
        "replacing argument %q on the %s-hand side of ##\n", name, side)
    out.add_step(
        ret, "replaced argument %q on the %s-hand side of ## with %q\n",
        name, side, ret)
    return ret
end

function replace_concat2(out, src)
    return src:gsub(
        -- XXX #, other characters?
        "()%f[%w_#]([%w_#]+)%s*##%s*([%w_#]+)%f[^%w_#]()",
        function(b, lhs, rhs, e)
            e = e - 1
            local oi <close> = out.stack_item(nil, src, b, e)
            out.add_step(src:sub(b, e), "concatenating %q and %q\n", lhs, rhs)
            local ret <const> = lhs .. rhs
            out.add_step(ret, "concatenated %q and %q as %q\n", lhs, rhs, ret)
            return ret
        end)
end

local replace_arg

function replace_args(p, out, repl, sub)
    return repl:gsub("()(#?)([%w_]+)%f[^%w_]()", function(b, is_str, param, e)
        e = e - 1
        local oi <close> = out.stack_item(nil, repl, b, e)
        if is_str == "" then
            return replace_arg(p, out, param, sub[param])
        else
            return replace_str_arg(out, param, sub[param])
        end
    end)
end

function replace_arg(p, out, param, repl)
    if not repl then
        return
    end
    out.add_step(param, "replacing parameter %q with argument\n", param)
    out.add_step(repl, "replaced parameter %q with value %q\n", param, repl)
    out.add_step(nil, "expanding parameter %q\n", param, repl)
    repl = p.replace(repl, true)
    out.add_step(repl, "replaced parameter %q with %q\n", param, repl)
    return repl
end

function replace_str_arg(out, param, sub)
    if not sub then
        return
    end
    sub = string.format("%q", sub)
    out.add_step(sub, "replaced parameter %q with literal %s\n", param, sub)
    return sub
end

return {
    defined = defined,
    define = define,
    undef = undef,
    replace = replace,
    -- XXX
    replace_concat2 = replace_concat2,
}
