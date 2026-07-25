-----------------------------------
-- Pure system tests for pDIF effective defense / final caps / crit mult
-- dual-wire (slice 6758).
-- Goldens match internal/wsformula.EffectiveDefense and
-- internal/pdif FinalCap* / CritDamageMult.
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('effectiveDefenseFromParams', function()
    it('floors def to 1 and skips ignore when not ignoring', function()
        assert(p.effectiveDefenseFromParams({ defense = 100 }) == 100)
        assert(p.effectiveDefenseFromParams({ defense = 0 }) == 1)
        assert(p.effectiveDefenseFromParams({
            defense = 100, ignoresDefense = false, ignoreFraction = 0.5,
        }) == 100)
    end)

    it('applies ignore fraction with floor and min 1', function()
        assert(p.effectiveDefenseFromParams({
            defense = 100, ignoresDefense = true, ignoreFraction = 0.5,
        }) == 50)
        assert(p.effectiveDefenseFromParams({
            defense = 100, ignoresDefense = true, ignoreFraction = 1.0,
        }) == 1)
        -- floor(99 * 0.75) = 74
        assert(p.effectiveDefenseFromParams({
            defense = 99, ignoresDefense = true, ignoreFraction = 0.25,
        }) == 74)
    end)
end)

describe('finalCap pure injects', function()
    it('finalCapPC includes optional melee crit bonus', function()
        -- (3.5 + 0) * 1 = 3.5
        assert(almost(p.finalCapPCFromParams({
            weaponCap = 3.5, damageLimit = 0, damageLimitP = 0,
        }), 3.5))
        -- (3 + 100/100) * (1 + 50/100) = 4 * 1.5 = 6; +1 crit = 7
        assert(almost(p.finalCapPCFromParams({
            weaponCap = 3, damageLimit = 100, damageLimitP = 50, meleeCritBonus = true,
        }), 7))
        -- ranged PC: no melee crit bonus
        assert(almost(p.finalCapPCFromParams({
            weaponCap = 3.25, damageLimit = 0, damageLimitP = 0, meleeCritBonus = false,
        }), 3.25))
    end)

    it('finalCapMeleeOthers base 2/4 with correction and crit', function()
        -- no correction: base 4
        assert(almost(p.finalCapMeleeOthersFromParams({
            applyLevelCorrection = false, isCritical = true,
            damageLimit = 0, damageLimitP = 0,
        }), 4))
        -- correction + crit: base 2 + critBonus 1 = 3
        assert(almost(p.finalCapMeleeOthersFromParams({
            applyLevelCorrection = true, isCritical = true,
            damageLimit = 0, damageLimitP = 0,
        }), 3))
        -- correction no crit: base 2
        assert(almost(p.finalCapMeleeOthersFromParams({
            applyLevelCorrection = true, isCritical = false,
            damageLimit = 0, damageLimitP = 0,
        }), 2))
    end)

    it('finalCapRangedOthers base 3/4', function()
        assert(almost(p.finalCapRangedOthersFromParams({
            applyLevelCorrection = false, damageLimit = 0, damageLimitP = 0,
        }), 4))
        assert(almost(p.finalCapRangedOthersFromParams({
            applyLevelCorrection = true, damageLimit = 0, damageLimitP = 0,
        }), 3))
        -- (3 + 1) * 1.1 = 4.4
        assert(almost(p.finalCapRangedOthersFromParams({
            applyLevelCorrection = true, damageLimit = 100, damageLimitP = 10,
        }), 4.4))
    end)
end)

describe('critDamageMultFromParams', function()
    it('clamps increase-def to 0..100 and scales', function()
        assert(almost(p.critDamageMultFromParams({
            critDmgIncrease = 0, critDefBonus = 0,
        }), 1.0))
        assert(almost(p.critDamageMultFromParams({
            critDmgIncrease = 25, critDefBonus = 0,
        }), 1.25))
        assert(almost(p.critDamageMultFromParams({
            critDmgIncrease = 10, critDefBonus = 30,
        }), 1.0)) -- clamp at 0
        assert(almost(p.critDamageMultFromParams({
            critDmgIncrease = 200, critDefBonus = 0,
        }), 2.0)) -- clamp at 100
    end)
end)
