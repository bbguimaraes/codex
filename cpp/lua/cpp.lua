#!/usr/bin/env lua
require "lib.strict"
local output <const> = require "lib.output"
local lexer <const> = require "lib.lexer"
local parser <const> = require "lib.parser"
local span <const> = require "lib.span"
local utils <const> = require "lib.utils"

local verbose = false
local lexeis = false
local lexer_only = false
local color = true

local function parse_args(args)
    local ret
    for i, a in ipairs(args) do
        if a:sub(1, 1) ~= "-" then
            ret = i
            break
        end
        if a == "-v" then
            verbose = true
        elseif a == "-l" or a == "--lexer-only" then
            lexeis = true
            lexer_only = true
        elseif a == "--lexer" then
            lexeis = true
        elseif a == "--no-color" then
            color = false
        end
    end
    if not ret then
        table.insert(args, "-")
        ret = #args
    end
    return ret
end

local function print_lexeis(f, l)
    local ret = true
    for id, s, line, col in l:iter() do
        ret = ret and id ~= lexer.UNKNOWN
        print(string.format(
            "%d:%d %s %q",
            line, col, lexer.to_str(id), utils.escape(s:str())))
    end
    return ret
end

local function open_file(path)
    if verbose then
        io.stderr:write(path, "\n\n")
    end
    if path == "-" then
        return io.stdin
    else
        return assert(io.open(path, "r"))
    end
end

local function process_file(f)
    local src <const> = span.new(open_file(f):read("a"))
    local out <const> = verbose
        and output.output:new(color)
        or output.nop_output:new()
    if lexeis then
        print_lexeis(f, lexer.new(src, out))
        if lexer_only then return true end
    end
    local lexer <const> = lexer.new(src, out)
    local parser <const> = parser.parser:new(lexer, out)
    local ret <const> = assert(xpcall(parser.parse, debug.traceback, parser))
    out:dump(src, parser, io.stderr)
    io.write(parser.out, "\n")
    return ret
end

local function main()
    local ret = true
    for i = parse_args(arg), #arg do
        ret = process_file(arg[i]) and ret
    end
    return ret
end

if not main() then
    os.exit(1)
end
