-----------------------------------
-- Pure system tests for utils.splitStr / trimStr / splitArg.
-----------------------------------

describe('Utils string split pure plans', function()
    it('splitStr on single-char sep drops empties', function()
        local t = utils.splitStr('a b c', ' ')
        assert(#t == 3 and t[1] == 'a' and t[2] == 'b' and t[3] == 'c')
        t = utils.splitStr('a  b', ' ')
        assert(#t == 2 and t[1] == 'a' and t[2] == 'b')
        t = utils.splitStr('one,two,three', ',')
        assert(#t == 3 and t[3] == 'three')
        t = utils.splitStr('a ', ' ')
        assert(#t == 1 and t[1] == 'a')
    end)

    it('trimStr: leading s+ exactly once + trailing whitespace (LSB quirk)', function()
        -- Pattern '^s%+' = "s" then one literal "+" (Lua %+ is escaped +).
        -- Almost certainly a typo for '^%s+' (leading whitespace).
        assert(utils.trimStr('s+hello') == 'hello')
        assert(utils.trimStr('s+++hello') == '++hello') -- only first "s+" removed
        assert(utils.trimStr('ssshello') == 'ssshello')
        assert(utils.trimStr('  hello  ') == '  hello') -- leading spaces kept
        assert(utils.trimStr('hello   ') == 'hello')
        assert(utils.trimStr('') == '')
    end)

    it('splitArg: commas to spaces, collapse, trim, split', function()
        local t = utils.splitArg('a,b,c')
        assert(#t == 3 and t[1] == 'a' and t[2] == 'b' and t[3] == 'c')
        t = utils.splitArg('  foo   bar  ')
        assert(#t == 2 and t[1] == 'foo' and t[2] == 'bar')
        t = utils.splitArg('one,  two   three')
        assert(#t == 3 and t[1] == 'one' and t[2] == 'two' and t[3] == 'three')
        t = utils.splitArg('s+arg1 arg2')
        assert(#t == 2 and t[1] == 'arg1' and t[2] == 'arg2')
    end)
end)
