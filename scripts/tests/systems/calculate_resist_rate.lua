-----------------------------------
-- Pure system tests for calculateResistRate top-level composition (slice 6761 / 6088).
-- Calls production xi.combat.magicHitRate pure exports (not local copies).
-----------------------------------

require('scripts/globals/combat/magic_hit_rate')

describe('calculate resist rate evaluateResistRateFromParams', function()
    it('rank 11 auto-resist', function()
        local nuke = xi.combat.magicHitRate.evaluateResistRateFromParams({
            resistanceRank = 11, effectId = 0,
        })
        assert(nuke.autoResist == true)
        assert(nuke.factor == 0.25)

        local status = xi.combat.magicHitRate.evaluateResistRateFromParams({
            resistanceRank = 11, effectId = 1,
        })
        assert(status.autoResist == true)
        assert(status.factor == 0)
    end)

    it('rank 10 skips MACC/MEVA assembly and uses floor MHR', function()
        local res = xi.combat.magicHitRate.evaluateResistRateFromParams({
            resistanceRank = 10,
            resistRolls    = { true, true, false },
            isPC           = false,
        })
        assert(res.assemblySkipped == true)
        assert(res.magicHitRate == 0.05)
        assert(res.resistTier == 2)
        assert(res.factor == 0.25)
    end)

    it('normal assembly macc==meva → 0.50 MHR, no resists → factor 1', function()
        local res = xi.combat.magicHitRate.evaluateResistRateFromParams({
            resistanceRank     = 0,
            actorMagicAccuracy = 300,
            targetMagicEvasion = 300,
            resistRolls        = { false, true, true },
        })
        assert(res.magicHitRate == 0.50)
        assert(res.factor == 1)
        assert(res.resistTier == 0)
        assert(res.autoResist == false)
        assert(res.assemblySkipped == false)
    end)

    it('PC weak element caps max tiers at 1', function()
        local res = xi.combat.magicHitRate.evaluateResistRateFromParams({
            resistanceRank     = 0,
            actorMagicAccuracy = 300,
            targetMagicEvasion = 300,
            isPC               = true,
            elementalMeva      = -5,
            resistRolls        = { true, true, true },
        })
        assert(res.maxTiers == 1)
        assert(res.resistTier == 1)
        assert(res.factor == 0.5)
    end)
end)

describe('calculate resist rate calculateResistRateFromParams', function()
    it('rank 11 auto-resist before shield/element gates', function()
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            magicalElement = xi.element.FIRE, baseRank = 11, effectId = 2,
        }) == 0)
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            magicalElement = xi.element.FIRE, baseRank = 11, effectId = 0,
        }) == 0.25)
    end)

    it('magic shield and non-elemental after rank gate', function()
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            hasMagicShield = true, magicalElement = xi.element.FIRE, baseRank = 0,
        }) == 0)
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            magicalElement = xi.element.NONE, baseRank = 0,
        }) == 1)
    end)

    it('tier composition after injects', function()
        -- PC target always rank 0; two resists → 0.25
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            magicalElement     = xi.element.FIRE,
            targetIsPC         = true,
            baseRank           = 5,
            actorMagicAccuracy = 100,
            targetMagicEvasion = 100,
            resistRolls        = { true, true, false },
        }) == 0.25)

        -- Rank 10 uses floor MHR path; three resists non-PC → 0.125
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            magicalElement = xi.element.FIRE, baseRank = 10, effectId = 0,
            resistRolls    = { true, true, true },
        }) == 0.125)
    end)

    it('PC weak element elementalMeva screen on full product', function()
        assert(xi.combat.magicHitRate.calculateResistRateFromParams({
            magicalElement     = xi.element.FIRE,
            targetIsPC         = true,
            isPC               = true,
            elementalMeva      = -10,
            actorMagicAccuracy = 300,
            targetMagicEvasion = 300,
            resistRolls        = { true, true, true },
        }) == 0.5)
    end)
end)
