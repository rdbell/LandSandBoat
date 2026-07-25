-----------------------------------
-- Pure system tests for magic WS damage product dual-wire (slice 6770).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula MagicWeaponskillDamage*.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

describe('magicWeaponskillDamagePreSevereFromParams', function()
    it('raw → scarlet → WSD → resist floors', function()
        -- raw (0+75+2+8)*1 = 85; scarlet 1; WSD 0; resist 0.5 → 42
        assert(ws.magicWeaponskillDamagePreSevereFromParams({
            wsc = 0, mainLvl = 75, fint = 8, ftp = 1, gearFTP = 0,
            scarletMult = 1, resist = 0.5, damageAdj = 1,
        }) == 42)
    end)

    it('afterAbility inject replaces scarlet+WSD product', function()
        -- afterAbility 200, resist 1, adj 0.75 → floor(150)=150
        assert(ws.magicWeaponskillDamagePreSevereFromParams({
            wsc = 0, mainLvl = 75, fint = 8, ftp = 1,
            scarletMult = 1, useAfterAbility = true, afterAbility = 200,
            resist = 1, damageAdj = 0.75,
        }) == 150)
    end)

    it('applies WSD percent before ability inject gate', function()
        -- without ability inject: raw 85 * 1.2 = 102; resist 1 → 102
        assert(ws.magicWeaponskillDamagePreSevereFromParams({
            wsc = 0, mainLvl = 75, fint = 8, ftp = 1,
            scarletMult = 1, allWSDMG = 20,
            resist = 1, damageAdj = 1,
        }) == 102)
    end)
end)

describe('magicWeaponskillDamageFinalFromParams', function()
    it('negative severe early-returns without power', function()
        local dmg, early = ws.magicWeaponskillDamageFinalFromParams({
            afterSevere = -10, absorb = 1, nullify = 1, weaponSkillPower = 1.5,
        })
        assert(early == true)
        assert(dmg == -10)
    end)

    it('absorb then weapon skill power', function()
        local dmg, early = ws.magicWeaponskillDamageFinalFromParams({
            afterSevere = 100, absorb = 0.5, nullify = 1, weaponSkillPower = 1.5,
        })
        assert(early == false)
        assert(dmg == 75)
    end)

    it('mitigate inject then power', function()
        local dmg = ws.magicWeaponskillDamageFinalFromParams({
            afterSevere = 100, absorb = 1, nullify = 1,
            useAfterMitigate = true, afterMitigate = 40, weaponSkillPower = 2,
        })
        assert(dmg == 80)
    end)
end)
