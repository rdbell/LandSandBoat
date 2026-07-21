-----------------------------------
-- Pure system tests for utils.appendArrays / slice / join (array form).
-----------------------------------

describe('utils table helpers pure plans', function()
    it('appendArrays concatenates ipairs sequences', function()
        local r = utils.appendArrays({ 1, 2 }, { 3, 4 })
        assert(#r == 4)
        assert(r[1] == 1 and r[2] == 2 and r[3] == 3 and r[4] == 4)

        local r2 = utils.appendArrays({}, { 'a' })
        assert(#r2 == 1 and r2[1] == 'a')
    end)

    it('slice is 1-based with optional first/last/step', function()
        local t = { 'a', 'b', 'c', 'd', 'e' }
        local s = utils.slice(t, 2, 4, 1)
        assert(#s == 3 and s[1] == 'b' and s[2] == 'c' and s[3] == 'd')

        local s2 = utils.slice(t, 1, 5, 2)
        assert(#s2 == 3 and s2[1] == 'a' and s2[2] == 'c' and s2[3] == 'e')

        local full = utils.slice(t)
        assert(#full == 5 and full[1] == 'a' and full[5] == 'e')

        local rev = utils.slice(t, 5, 1, -2)
        assert(#rev == 3 and rev[1] == 'e' and rev[2] == 'c' and rev[3] == 'a')
    end)

    it('join merges array keys (second wins)', function()
        -- join({ 'a', 'b' }, { 'c' }) → { 'c', 'b' }
        local j = utils.join({ 'a', 'b' }, { 'c' })
        assert(#j == 2)
        assert(j[1] == 'c')
        assert(j[2] == 'b')

        local j2 = utils.join({ 'a' }, { 'x', 'y' })
        assert(#j2 == 2 and j2[1] == 'x' and j2[2] == 'y')
    end)

    it('append merges into base table', function()
        local base = { a = 1, nested = { x = 1 } }
        utils.append(base, { b = 2, nested = { y = 2 } })
        assert(base.a == 1 and base.b == 2)
        assert(base.nested.x == 1 and base.nested.y == 2)
    end)

    it('append overwrites non-table and replaces table over scalar', function()
        local base = { k = 1 }
        utils.append(base, { k = 'str' })
        assert(base.k == 'str')

        local base2 = { k = 1 }
        utils.append(base2, { k = { z = 9 } })
        assert(type(base2.k) == 'table' and base2.k.z == 9)
    end)

    it('join nested second wins on shared keys', function()
        local j = utils.join(
            { a = 1, nested = { x = 1 } },
            { b = 2, nested = { y = 2, x = 9 } }
        )
        assert(j.a == 1 and j.b == 2)
        assert(j.nested.x == 9 and j.nested.y == 2)
    end)
end)
