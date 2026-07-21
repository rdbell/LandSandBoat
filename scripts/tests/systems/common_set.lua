-----------------------------------
-- Pure system tests for common.lua set() membership helper.
-----------------------------------

describe('common set pure plans', function()
    it('builds membership table from list', function()
        local s = set({ 1, 2, 3, 2 })
        assert(s[1] == true and s[2] == true and s[3] == true)
        assert(s[4] == nil)
    end)

    it('empty list yields empty set', function()
        local s = set({})
        local n = 0
        for _ in pairs(s) do
            n = n + 1
        end
        assert(n == 0)
    end)

    it('string keys', function()
        local s = set({ 'a', 'b' })
        assert(s['a'] == true and s['c'] == nil)
    end)
end)
