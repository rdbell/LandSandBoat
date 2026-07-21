-----------------------------------
-- Pure system tests for xi.data.augments.evolith catalog.
-----------------------------------

describe('evolith augments pure catalog', function()
    it('id 1 is VS_BEASTS attack with known tiers', function()
        local e = xi.data.augments.evolith[1]
        assert(e[1] == xi.evolith.prefix.VS_BEASTS)
        assert(e[2] == xi.evolith.suffix.ATTACK)
        assert(e[3][1] == 4)
        assert(e[3][4] == 7)
        assert(e[3][8] == 12)
    end)

    it('id 120 is VS_DRAGONS ranged accuracy (DAT example)', function()
        local e = xi.data.augments.evolith[120]
        assert(e[1] == xi.evolith.prefix.VS_DRAGONS)
        assert(e[2] == xi.evolith.suffix.RANGED_ACCURACY)
        -- 1-based tier list: [1]=2 .. [5]=7
        assert(e[3][1] == 2)
        assert(e[3][5] == 7)
    end)

    it('evolithIndex reverse map matches id 1 and 120', function()
        assert(xi.data.augments.evolithIndex[xi.evolith.prefix.VS_BEASTS][xi.evolith.suffix.ATTACK] == 1)
        assert(xi.data.augments.evolithIndex[xi.evolith.prefix.VS_DRAGONS][xi.evolith.suffix.RANGED_ACCURACY] == 120)
    end)

    it('catalog covers 1..325', function()
        assert(xi.data.augments.evolith[1] ~= nil)
        assert(xi.data.augments.evolith[325] ~= nil)
        assert(xi.data.augments.evolith[325][1] == xi.evolith.prefix.SKILLCHAIN_DARK)
    end)
end)
