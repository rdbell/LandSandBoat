require('scripts/globals/dynamis')

describe('Dynamis time-extension groups', function()
    it('normalizes single and grouped extension mobs and rejects missing IDs', function()
        local single = { mob = 100, ki = 200, minutes = 10 }
        local grouped = { mob = { 101, 102 }, ki = 201, minutes = 20 }
        local entries = { single, grouped }

        local te, group = xi.dynamis.findTimeExtensionGroup(entries, 100)
        assert(te == single and #group == 1 and group[1] == 100)

        te, group = xi.dynamis.findTimeExtensionGroup(entries, 102)
        assert(te == grouped and #group == 2 and group[1] == 101 and group[2] == 102)

        te, group = xi.dynamis.findTimeExtensionGroup(entries, 999)
        assert(te == nil and group == nil)

        te, group = xi.dynamis.findTimeExtensionGroup(nil, 100)
        assert(te == nil and group == nil)
    end)
end)
