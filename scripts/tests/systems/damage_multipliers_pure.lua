-----------------------------------
-- Pure system tests for damage multiplier dual-wire helpers (slice 6691).
-- Calls production xi.combat.damage pure exports.
-- Goldens match internal/dmgmultiplier (0850 / 6075).
-----------------------------------

require('scripts/globals/combat/damage_multipliers')

local dmg = xi.combat.damage
local dt = xi.damageType
local el = xi.element

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('damage multiplier constants', function()
    it('pins SDT and taken clamps', function()
        assert(dmg.sdtMin == 0)
        assert(dmg.sdtMax == 3)
        assert(dmg.combinedTakenMin == -0.5)
        assert(dmg.combinedTakenMax == 0.5)
        assert(dmg.typeIIMultMin == 0.125)
        assert(dmg.typeIIMultMax == 1.875)
        assert(dmg.uncappedMultMin == 0)
        assert(dmg.uncappedMultMax == 2)
        assert(dmg.steamJacketLocalVar == '[steamJacket]Element')
    end)
end)

describe('physicalElementSDTFromParams', function()
    it('returns 1 outside PIERCING..H2H', function()
        assert(dmg.physicalElementSDTFromParams({ physicalElement = 0, sdtMod = -5000 }) == 1)
        assert(dmg.physicalElementSDTFromParams({ physicalElement = 5, sdtMod = -5000 }) == 1)
    end)

    it('scales and clamps physical SDT', function()
        assert(dmg.physicalElementSDTFromParams({ physicalElement = dt.PIERCING, sdtMod = 0 }) == 1)
        assert(almost(dmg.physicalElementSDTFromParams({
            physicalElement = dt.PIERCING, sdtMod = -2500,
        }), 0.75))
        assert(almost(dmg.physicalElementSDTFromParams({
            physicalElement = dt.SLASHING, sdtMod = 2500,
        }), 1.25))
        assert(dmg.physicalElementSDTFromParams({
            physicalElement = dt.BLUNT, sdtMod = -20000,
        }) == 0)
        assert(dmg.physicalElementSDTFromParams({
            physicalElement = dt.HAND_TO_HAND, sdtMod = 50000,
        }) == 3)
        assert(dmg.physicalElementSDTFromParams({
            physicalElement = dt.PIERCING, sdtMod = -10000,
        }) == 0)
        assert(dmg.physicalElementSDTFromParams({
            physicalElement = dt.PIERCING, sdtMod = 20000,
        }) == 3)
    end)
end)

describe('magicalElementSDTFromParams', function()
    it('returns 1 outside FIRE..DARK', function()
        assert(dmg.magicalElementSDTFromParams({ magicalElement = 0, sdtMod = -5000 }) == 1)
        assert(dmg.magicalElementSDTFromParams({ magicalElement = 9, sdtMod = -5000 }) == 1)
    end)

    it('scales and clamps magic SDT', function()
        for e = el.FIRE, el.DARK do
            assert(dmg.magicalElementSDTFromParams({ magicalElement = e, sdtMod = 0 }) == 1)
        end
        assert(almost(dmg.magicalElementSDTFromParams({
            magicalElement = el.FIRE, sdtMod = -2500,
        }), 0.75))
        assert(dmg.magicalElementSDTFromParams({
            magicalElement = el.DARK, sdtMod = 10000,
        }) == 2)
        assert(dmg.magicalElementSDTFromParams({
            magicalElement = el.LIGHT, sdtMod = -15000,
        }) == 0)
        assert(dmg.magicalElementSDTFromParams({
            magicalElement = el.THUNDER, sdtMod = 30000,
        }) == 3)
    end)
end)

describe('damageAdjustmentFromParams', function()
    it('returns 1 with no mods', function()
        assert(dmg.damageAdjustmentFromParams({}) == 1)
        assert(dmg.damageAdjustmentFromParams({ isPhysical = true }) == 1)
        assert(dmg.damageAdjustmentFromParams({
            isPhysical = true, isMagical = true, isRanged = true, isBreath = true,
        }) == 1)
    end)

    it('applies shared DMG and type-I ±50% cap', function()
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = -2500,
        }), 0.75))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = 2500,
        }), 1.25))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = -10000,
        }), 0.5))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = 10000,
        }), 1.5))
    end)

    it('applies type-I, type-II, and uncapped ladders', function()
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmgPhys = -2000,
        }), 0.8))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = -3000, dmgPhys = -3000,
        }), 0.5))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = -5000, dmgPhys = -5000, dmgPhysII = -2500,
        }), 0.25))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = -5000, dmgPhys = -5000, dmgPhysII = -5000,
        }), 0.125))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = 5000, dmgPhys = 5000, dmgPhysII = 5000,
        }), 1.875))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, uDmgPhys = -2500,
        }), 0.75))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = -5000, dmgPhys = -5000, dmgPhysII = -5000, uDmgPhys = -2000,
        }), 0))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, dmg = 5000, dmgPhys = 5000, dmgPhysII = 5000, uDmgPhys = 2000,
        }), 2))
    end)

    it('gates type mods by category flags', function()
        assert(almost(dmg.damageAdjustmentFromParams({
            isMagical = true, dmgPhys = -5000, dmgPhysII = -5000, uDmgPhys = -5000, dmgMagic = -2000,
        }), 0.8))
        assert(almost(dmg.damageAdjustmentFromParams({
            isRanged = true, dmgRange = -2500, dmgPhysII = -9000,
        }), 0.75))
        assert(almost(dmg.damageAdjustmentFromParams({
            isBreath = true, dmg = -3000, dmgBreath = -3000,
        }), 0.5))
        assert(almost(dmg.damageAdjustmentFromParams({
            isPhysical = true, isMagical = true, dmgPhys = -2000, dmgMagic = -2000,
        }), 0.6))
    end)
end)

describe('scarletDeliriumMultiplierFromParams', function()
    it('returns 1 without effect; scales power/1000 with effect', function()
        assert(dmg.scarletDeliriumMultiplierFromParams({ hasEffect = false, power = 500 }) == 1)
        assert(dmg.scarletDeliriumMultiplierFromParams({ hasEffect = true, power = 0 }) == 1)
        assert(almost(dmg.scarletDeliriumMultiplierFromParams({ hasEffect = true, power = 500 }), 1.5))
        assert(almost(dmg.scarletDeliriumMultiplierFromParams({ hasEffect = true, power = 1000 }), 2))
    end)
end)

describe('steamJacketMultiplierFromParams', function()
    it('returns 1 for non-elemental, no mod, or mismatched track', function()
        assert(dmg.steamJacketMultiplierFromParams({
            magicalElement = 0, reductionMod = 25, priorTracked = el.FIRE,
        }) == 1)
        assert(dmg.steamJacketMultiplierFromParams({
            magicalElement = el.FIRE, reductionMod = 0, priorTracked = el.FIRE,
        }) == 1)
        assert(dmg.steamJacketMultiplierFromParams({
            magicalElement = el.FIRE, reductionMod = -5, priorTracked = el.FIRE,
        }) == 1)
        assert(dmg.steamJacketMultiplierFromParams({
            magicalElement = el.FIRE, reductionMod = 25, priorTracked = el.ICE,
        }) == 1)
        assert(dmg.steamJacketMultiplierFromParams({
            magicalElement = el.FIRE, reductionMod = 25, priorTracked = 0,
        }) == 1)
    end)

    it('reduces when prior tracked matches element', function()
        assert(almost(dmg.steamJacketMultiplierFromParams({
            magicalElement = el.THUNDER, reductionMod = 25, priorTracked = el.THUNDER,
        }), 0.75))
        assert(dmg.steamJacketMultiplierFromParams({
            magicalElement = el.DARK, reductionMod = 100, priorTracked = el.DARK,
        }) == 0)
    end)
end)
