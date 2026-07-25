-----------------------------------
-- Pure system tests for takeWeaponskillDamage TP inject dual-wire (slice 6772).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula TakeWeaponskillTPInject.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('takeWeaponskillTPInjectFromParams', function()
    it('non-jump assembles extra-hit TP and core TP hits', function()
        -- mult 1.5, extra 2, store 25 → extraTP = 2*10*1.25+5 = 30
        -- tpHitsForCore = 3 * 1.5 = 4.5
        local r = ws.takeWeaponskillTPInjectFromParams({
            isJump = false, attackerTPMult = 1.5, extraHitsLanded = 2,
            storeTPMod = 25, bonusTP = 5, tpHitsLanded = 3,
        })
        assert(almost(r.attackerTPMult, 1.5))
        assert(r.extraHitsLanded == 2)
        assert(almost(r.extraHitTP, 30))
        assert(almost(r.tpHitsForCore, 4.5))
    end)

    it('Jump zeros mult and extra hits; keeps bonusTP', function()
        local r = ws.takeWeaponskillTPInjectFromParams({
            isJump = true, attackerTPMult = 1.5, extraHitsLanded = 4,
            storeTPMod = 25, bonusTP = 7, tpHitsLanded = 2,
        })
        assert(r.attackerTPMult == 0)
        assert(r.extraHitsLanded == 0)
        assert(almost(r.extraHitTP, 7))
        assert(r.tpHitsForCore == 0)
    end)

    it('zero STORETP uses identity modifier', function()
        -- extra 1 * 10 * 1 + 0 = 10
        local r = ws.takeWeaponskillTPInjectFromParams({
            extraHitsLanded = 1, storeTPMod = 0, bonusTP = 0, tpHitsLanded = 1,
            attackerTPMult = 1,
        })
        assert(almost(r.extraHitTP, 10))
        assert(almost(r.tpHitsForCore, 1))
    end)
end)
