-----------------------------------
-- Pure system tests for physical/ranged WS final product dual-wire (slice 6774).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula PhysicalWeaponskillFinal / RangedWeaponskillFinal.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

describe('physicalWeaponskillFinalFromParams', function()
    it('floors raw, applies power', function()
        assert(ws.physicalWeaponskillFinalFromParams({
            rawFinalDmg = 100.9, weaponSkillPower = 1.5,
        }) == 150)
    end)

    it('adds hybrid magic when gated on', function()
        assert(ws.physicalWeaponskillFinalFromParams({
            rawFinalDmg = 100, useHybridMagic = true, hybridMagicDmg = 25,
            weaponSkillPower = 1,
        }) == 125)
    end)

    it('ignores hybrid inject when gate false', function()
        assert(ws.physicalWeaponskillFinalFromParams({
            rawFinalDmg = 100, useHybridMagic = false, hybridMagicDmg = 25,
            weaponSkillPower = 1,
        }) == 100)
    end)
end)

describe('rangedWeaponskillFinalFromParams', function()
    it('mitigates with Pierce SDT then power', function()
        -- floor(100 * 1.1) = 110
        assert(ws.rangedWeaponskillFinalFromParams({
            rangedDmgTakenResult = 100, pierceSDT = 1000, weaponSkillPower = 1,
        }) == 110)
    end)

    it('hybrid after mitigation then power', function()
        -- mitigation 100 + hybrid 20 = 120 * 1.5 = 180
        assert(ws.rangedWeaponskillFinalFromParams({
            rangedDmgTakenResult = 100, pierceSDT = 0,
            useHybridMagic = true, hybridMagicDmg = 20, weaponSkillPower = 1.5,
        }) == 180)
    end)
end)
