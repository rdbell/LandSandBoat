-----------------------------------
-- Pure system tests for isParried PARRY_HP_RECOVERY inject (slice 6098).
-- Mirrors scripts/globals/combat/physical_utilities.lua isParried (~1397–1403).
-----------------------------------

describe('parry HP recovery pure injects', function()
    local function parryHPRecovery(p)
        if not p.parried then
            return 0
        end
        if p.parryHPRecoveryMod <= 0 then
            return 0
        end
        if p.hasCurseII then
            return 0
        end
        return p.parryHPRecoveryMod
    end

    it('requires successful parry', function()
        assert(parryHPRecovery({ parried = false, parryHPRecoveryMod = 50 }) == 0)
        assert(parryHPRecovery({ parried = true, parryHPRecoveryMod = 50 }) == 50)
    end)

    it('requires positive PARRY_HP_RECOVERY mod', function()
        assert(parryHPRecovery({ parried = true, parryHPRecoveryMod = 0 }) == 0)
        assert(parryHPRecovery({ parried = true, parryHPRecoveryMod = -5 }) == 0)
        assert(parryHPRecovery({ parried = true, parryHPRecoveryMod = 1 }) == 1)
    end)

    it('blocked by Curse II', function()
        assert(parryHPRecovery({
            parried = true,
            parryHPRecoveryMod = 100,
            hasCurseII = true,
        }) == 0)
        assert(parryHPRecovery({
            parried = true,
            parryHPRecoveryMod = 100,
            hasCurseII = false,
        }) == 100)
    end)
end)
