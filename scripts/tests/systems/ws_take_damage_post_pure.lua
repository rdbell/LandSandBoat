-----------------------------------
-- Pure system tests for takeWeaponskillDamage post-entity residual (slice 6773).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula TakeWeaponskillPostDamage.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

describe('takeWeaponskillPostDamageFromParams', function()
    it('full residual path with Sengikori SC debuff', function()
        local p = ws.takeWeaponskillPostDamageFromParams({
            finalDmg = 100, tpHitsLanded = 1, extraHitsLanded = 1,
            hasTaChar = false, hasOverrideCE = false, hasOverrideVE = false,
            enmityMult = 1.5,
            hasSengikori = true, sengikoriPower = 10, sengikoriBonus = 5,
            targetHasSkillchain = false, wsID = 42,
        })
        assert(p.recordDamage == true)
        assert(p.absFinalDmg == 100)
        assert(p.enmityUsesTA == false)
        assert(p.enmityUsesOverride == false)
        assert(p.enmityAmount == 150)
        assert(p.sengikori.applies == true)
        assert(p.sengikori.power == 15)
        assert(p.sengikori.applyMBDebuff == false)
        assert(p.sengikori.delSengikori == true)
        assert(p.setHitVar == true)
        assert(p.hitLocalVar == bit.lshift(42, 24) + 100)
    end)

    it('override enmity + TA entity + MB Sengikori', function()
        local p = ws.takeWeaponskillPostDamageFromParams({
            finalDmg = 0, tpHitsLanded = 1, extraHitsLanded = 0,
            hasTaChar = true, hasOverrideCE = true, hasOverrideVE = true,
            enmityMult = 2,
            hasSengikori = true, sengikoriPower = 3, sengikoriBonus = 1,
            targetHasSkillchain = true, wsID = 1,
        })
        assert(p.recordDamage == true)
        assert(p.absFinalDmg == 0)
        assert(p.enmityUsesTA == true)
        assert(p.enmityUsesOverride == true)
        assert(p.enmityAmount == 0)
        assert(p.sengikori.applyMBDebuff == true)
        assert(p.sengikori.power == 4)
        assert(p.setHitVar == false)
    end)

    it('no hits: no record, no Sengikori, no hit var', function()
        local p = ws.takeWeaponskillPostDamageFromParams({
            finalDmg = -20, tpHitsLanded = 0, extraHitsLanded = 0,
            enmityMult = 1, hasSengikori = true, sengikoriPower = 10, wsID = 5,
        })
        assert(p.recordDamage == false)
        assert(p.sengikori.applies == false)
        assert(p.setHitVar == false)
        assert(p.absFinalDmg == 20)
    end)
end)
