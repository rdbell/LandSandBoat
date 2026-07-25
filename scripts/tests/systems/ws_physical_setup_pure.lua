-----------------------------------
-- Pure system tests for physical WS setup product dual-wire (slice 6775).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula PhysicalWeaponskillSetup.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('physicalWeaponskillSetupFromParams', function()
    it('Perfect Dodge forces mustMiss; normal bonus injects', function()
        local r = ws.physicalWeaponskillSetupFromParams({
            hasPerfectDodge = true,
            gearFTP = 0.25, gearAcc = 25, wsAccMod = 10, bonusWSmods = 5,
        })
        assert(r.mustMiss == true)
        assert(r.sneakApplicable == false)
        assert(r.guaranteedHit == false)
        assert(almost(r.bonusFTP, 0.25))
        assert(almost(r.bonusAcc, 35))
        assert(r.bonusWSmods == 5)
        assert(almost(r.firstHitAccBonus, 135))
    end)

    it('SA + TA + Assassin → guaranteed and forced first crit', function()
        local r = ws.physicalWeaponskillSetupFromParams({
            hasSneakAttack = true, isBehind = true,
            hasTaChar = true, hasAssassinTrait = true,
            gearFTP = 0.1, gearAcc = 0,
        })
        assert(r.sneakApplicable == true)
        assert(r.trickApplicable == true)
        assert(r.assassinApplicable == true)
        assert(r.guaranteedHit == true)
        assert(r.forcedFirstCrit == true)
    end)

    it('Jump zeros fTP/WSmods and uses jump acc only', function()
        local r = ws.physicalWeaponskillSetupFromParams({
            isJump = true, jumpAccBonus = 40,
            gearFTP = 0.5, gearAcc = 99, wsAccMod = 20, bonusWSmods = 7,
        })
        assert(r.bonusFTP == 0)
        assert(r.bonusAcc == 40)
        assert(r.bonusWSmods == 0)
        assert(r.firstHitAccBonus == 140)
    end)

    it('accVaries adds onto bonusAcc', function()
        local r = ws.physicalWeaponskillSetupFromParams({
            gearAcc = 10, wsAccMod = 5, hasAccVaries = true, accVariesFTP = 0.5,
        })
        assert(almost(r.bonusAcc, 15.5))
        assert(almost(r.firstHitAccBonus, 115.5))
    end)

    it('ALL_MISS mustMiss unless hitsHigh', function()
        assert(ws.physicalWeaponskillSetupFromParams({
            hasAllMiss = true, hitsHigh = false,
        }).mustMiss == true)
        assert(ws.physicalWeaponskillSetupFromParams({
            hasAllMiss = true, hitsHigh = true,
        }).mustMiss == false)
    end)
end)
