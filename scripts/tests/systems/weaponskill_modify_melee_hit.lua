require('scripts/globals/weaponskills')

describe('Weaponskill modify melee hit product', function()
    it('skips physical taken and SDT when formless', function()
        -- formless: raw * scarlet + souleater, physicalTaken/sdt ignored
        assert(xi.weaponskills.modifyMeleeHitDamageProduct(100, true, 50, 0.5, 1.2, 10) == 130)
    end)

    it('uses physical taken times SDT when not formless', function()
        -- not formless: 80 * 1.25 = 100, * 1 = 100, + 0 = 100
        assert(xi.weaponskills.modifyMeleeHitDamageProduct(200, false, 80, 1.25, 1, 0) == 100)
    end)

    it('applies scarlet then souleater in that order', function()
        -- 100 * 1.5 = 150 + 20 = 170
        assert(xi.weaponskills.modifyMeleeHitDamageProduct(100, true, 0, 0, 1.5, 20) == 170)
        -- non-formless: 40 * 2 = 80 * 1.25 = 100 + 5 = 105
        assert(xi.weaponskills.modifyMeleeHitDamageProduct(100, false, 40, 2, 1.25, 5) == 105)
    end)

    it('preserves fractional intermediate products', function()
        -- 33 * 1.5 = 49.5 * 1.1 = 54.45 + 0.55 = 55
        assert(xi.weaponskills.modifyMeleeHitDamageProduct(33, false, 33, 1.5, 1.1, 0.55) == 55)
    end)
end)
