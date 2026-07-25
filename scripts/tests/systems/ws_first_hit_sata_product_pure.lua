-----------------------------------
-- Pure system tests for first-hit SA/TA/augment product dual-wire (slice 6771).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula FirstHitSATAProduct.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

describe('firstHitSATAProductFromParams', function()
    it('identity when no SA/TA', function()
        assert(ws.firstHitSATAProductFromParams({ finalDmg = 100 }) == 100)
    end)

    it('non-THF with SA only applies AUGMENTS_SA', function()
        assert(ws.firstHitSATAProductFromParams({
            finalDmg = 100, sneakApplicable = true, augmentsSA = 25,
            isTHFMain = false, pdif = 2, dex = 100, sneakAtkDexMod = 10,
        }) == 125)
    end)

    it('THF SA adds DEX pDIF bonus', function()
        -- floor(100 + 1.5 * floor(100*1.1)) = floor(100 + 165) = 265
        assert(ws.firstHitSATAProductFromParams({
            finalDmg = 100, pdif = 1.5, isTHFMain = true, sneakApplicable = true,
            dex = 100, sneakAtkDexMod = 10,
        }) == 265)
    end)

    it('THF SA then AUGMENTS_SA', function()
        -- 265 * 1.25 = floor 331
        assert(ws.firstHitSATAProductFromParams({
            finalDmg = 100, pdif = 1.5, isTHFMain = true, sneakApplicable = true,
            dex = 100, sneakAtkDexMod = 10, augmentsSA = 25,
        }) == 331)
    end)

    it('THF TA adds AGI pDIF bonus', function()
        -- floor(40 + 2 * floor(80*1.05)) = floor(40 + 168) = 208
        assert(ws.firstHitSATAProductFromParams({
            finalDmg = 40, pdif = 2, isTHFMain = true, trickApplicable = true,
            agi = 80, trickAtkAgiMod = 5,
        }) == 208)
    end)

    it('non-THF TA only applies AUGMENTS_TA', function()
        assert(ws.firstHitSATAProductFromParams({
            finalDmg = 200, trickApplicable = true, augmentsTA = 10,
            isTHFMain = false, pdif = 2, agi = 100,
        }) == 220)
    end)
end)
