-----------------------------------
-- Pure system tests for hybrid WS magic product dual-wire (slice 6769).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula HybridWeaponskillMagic*.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

describe('hybridWeaponskillMagicPreSevereFromParams', function()
    it('applies bonus fTP then resist/shell floors', function()
        -- afterAbility 100, bonusFTP 0.5 on physical 100 → 150
        -- resist 0.5, adj 1 → floor(floor(150*0.5)*1) = 75
        assert(ws.hybridWeaponskillMagicPreSevereFromParams({
            afterAbility = 100, physicaldmg = 100,
            hasBonusFTP = true, bonusFTP = 0.5,
            resist = 0.5, damageAdj = 1,
        }) == 75)

        -- resist 1, adj 0.75 → floor(150*0.75) = 112
        assert(ws.hybridWeaponskillMagicPreSevereFromParams({
            afterAbility = 100, physicaldmg = 100,
            hasBonusFTP = true, bonusFTP = 0.5,
            resist = 1, damageAdj = 0.75,
        }) == 112)
    end)

    it('zero bonus fTP when not present', function()
        assert(ws.hybridWeaponskillMagicPreSevereFromParams({
            afterAbility = 100, physicaldmg = 100,
            hasBonusFTP = false, bonusFTP = 0.5,
            resist = 1, damageAdj = 1,
        }) == 100)
    end)
end)

describe('hybridWeaponskillMagicFinalFromParams', function()
    it('absorb/nullify then floor', function()
        assert(ws.hybridWeaponskillMagicFinalFromParams({
            afterSevere = 100, absorb = 0.5, nullify = 1,
        }) == 50)
        assert(ws.hybridWeaponskillMagicFinalFromParams({
            afterSevere = 0, absorb = 0.5, nullify = 1,
        }) == 0)
    end)

    it('uses afterMitigate when still positive', function()
        assert(ws.hybridWeaponskillMagicFinalFromParams({
            afterSevere = 100, absorb = 1, nullify = 1,
            useAfterMitigate = true, afterMitigate = 40,
        }) == 40)
        -- nullify to 0 ignores mitigate inject
        assert(ws.hybridWeaponskillMagicFinalFromParams({
            afterSevere = 100, absorb = 1, nullify = 0,
            useAfterMitigate = true, afterMitigate = 40,
        }) == 0)
    end)
end)

describe('hybridWeaponskillMagicFromParams', function()
    it('full product without residual injects', function()
        -- base floor(100*1.5)=150; bonus 0; resist 1; adj 1; absorb 1
        assert(ws.hybridWeaponskillMagicFromParams({
            physicaldmg = 100, ftp = 1.5,
            absorb = 1, nullify = 1,
        }) == 150)
    end)

    it('uses afterAbility / afterSevere injects', function()
        assert(ws.hybridWeaponskillMagicFromParams({
            physicaldmg = 100, ftp = 1.5,
            useAfterAbility = true, afterAbility = 100,
            hasBonusFTP = true, bonusFTP = 0.5,
            resist = 0.5, damageAdj = 1,
            useAfterSevere = true, afterSevere = 70,
            absorb = 1, nullify = 1,
        }) == 70)
    end)

    it('stacks base WSD then residual absorb', function()
        -- base floor(100*1.0 * 120/100)=120; absorb 0.5 → 60
        assert(ws.hybridWeaponskillMagicFromParams({
            physicaldmg = 100, ftp = 1, allWSDMG = 20,
            absorb = 0.5, nullify = 1,
        }) == 60)
    end)
end)
