-----------------------------------
-- Pure system tests for ranged WS setup product dual-wire (slice 6776).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula RangedWeaponskillSetup.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('rangedWeaponskillSetupFromParams', function()
    it('normal bonus injects without first-hit bonus', function()
        local r = ws.rangedWeaponskillSetupFromParams({
            gearFTP = 0.2, gearAcc = 30, wsAccMod = 5, bonusWSmods = 3,
        })
        assert(almost(r.bonusFTP, 0.2))
        assert(almost(r.bonusAcc, 35))
        assert(r.bonusWSmods == 3)
        assert(r.useFirstHitBonusAcc == false)
        assert(almost(r.firstHitBonusAcc, 35))
    end)

    it('accVaries + rangedAccuracyBonus for first hit only', function()
        local r = ws.rangedWeaponskillSetupFromParams({
            gearAcc = 10, wsAccMod = 0, hasAccVaries = true, accVariesFTP = 0.5,
            hasRangedAccuracyBonus = true, rangedAccuracyBonus = 20,
        })
        assert(almost(r.bonusAcc, 10.5))
        assert(almost(r.firstHitBonusAcc, 30.5))
        assert(r.useFirstHitBonusAcc == true)
    end)

    it('rangedAccuracyBonus alone adds only to first-hit acc', function()
        local r = ws.rangedWeaponskillSetupFromParams({
            gearAcc = 50, wsAccMod = 0,
            hasRangedAccuracyBonus = true, rangedAccuracyBonus = 15,
        })
        assert(almost(r.bonusAcc, 50))
        assert(almost(r.firstHitBonusAcc, 65))
        assert(r.useFirstHitBonusAcc == true)
    end)
end)
