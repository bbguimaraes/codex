local function escape(s)
    return s:gsub("\n", "\\n")
end

local function remove(t, k)
    local ret <const> = t[k]
    t[k] = nil
    return ret
end

local function find(t, x, f)
    for i, y in (f or ipairs)(t) do
        if x == y then
            return i
        end
    end
end

local function trim_i(s)
    local _, b <const> = s:find("^%s+")
    local e <const> = s:find("%s+$")
    return b, e
end

local function trim(s)
    local b, e = trim_i(s)
    b = (b or 0) + 1
    e = (e or 0) - 1
    return s:sub(b, e)
end

local function split(s, pat)
    local ret <const> = {}
    for x in s:gmatch(pat) do
        table.insert(ret, trim(x))
    end
    return ret
end

return {
    escape = escape,
    remove = remove,
    find = find,
    split = split,
}
