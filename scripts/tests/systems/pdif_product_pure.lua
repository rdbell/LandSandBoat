-----------------------------------
-- Pure system tests for MeleePDIF / RangedPDIF full product dual-wire (slice 6762).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/pdif MeleePDIF / RangedPDIF.
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('meleePDIFFromParams', function()
    it('spikes to 1.0 when spike roll lands', function()
        -- PC wRatio ~1.0 has positive spike chance; roll 1 always spikes if sRatio>0.
        assert(p.meleePDIFFromParams({
            actorAttack   = 100,
            targetDefense = 100,
            actorIsPC     = true,
            weaponCap     = 3.25,
            spikeRoll     = 1,
        }) == 1.0)
    end)

    it('mid path uses ratio roll and melee random', function()
        local base =
        {
            actorAttack    = 100,
            targetDefense  = 100,
            actorIsPC      = true,
            weaponCap      = 3.25,
            spikeRoll      = 10000, -- never spikes when sRatio < 1
            upperMaxCoin   = 1,     -- upperMax 0
            ratioRollValid = true,
            ratioRoll      = 1500,  -- 1.5
            meleeRandStep  = 0,
        }
        assert(p.meleePDIFFromParams(base) == 1.5)

        base.meleeRandStep = 5
        assert(almost(p.meleePDIFFromParams(base), 1.5 * 1.05))
    end)

    it('applies crit damage mult after random factor', function()
        local got = p.meleePDIFFromParams({
            actorAttack     = 100,
            targetDefense   = 100,
            actorIsPC       = true,
            weaponCap       = 3.25,
            isCritical      = true,
            spikeRoll       = 10000,
            upperMaxCoin    = 1,
            ratioRollValid  = true,
            ratioRoll       = 1000,
            meleeRandStep   = 0,
            critDmgIncrease = 20,
            critDefBonus    = 0,
        })
        -- 1.0 * 1.0 * (100+20)/100 = 1.2
        assert(almost(got, 1.2))
    end)
end)

describe('rangedPDIFFromParams', function()
    it('applies 1.25 crit mult then gear crit dmg', function()
        local got = p.rangedPDIFFromParams({
            actorAttack     = 100,
            targetDefense   = 100,
            actorIsPC       = true,
            weaponCap       = 3.25,
            isCritical      = true,
            critDmgIncrease = 0,
            critDefBonus    = 0,
            ratioRollValid  = true,
            ratioRoll       = 1000, -- 1.0
        })
        -- 1.0 * 1.25 * 1.0 = 1.25
        assert(almost(got, 1.25))
        assert(almost(p.rangedCritMult, 1.25))
    end)

    it('clamps negative ratio rolls to 0', function()
        assert(p.rangedPDIFFromParams({
            actorAttack    = 10,
            targetDefense  = 100,
            actorIsPC      = true,
            weaponCap      = 3.25,
            ratioRollValid = true,
            ratioRoll      = -50,
        }) == 0)
    end)
end)

describe('meleePDIFBoundsFromParams and rangedPDIFBoundsFromParams', function()
    it('melee spike short-circuits bounds', function()
        local lower, upper, spiked, spikePdif = p.meleePDIFBoundsFromParams({
            actorAttack   = 100,
            targetDefense = 100,
            actorIsPC     = true,
            weaponCap     = 3.25,
            spikeRoll     = 1,
        })
        assert(spiked == true)
        assert(spikePdif == 1.0)
        assert(lower == 0)
        assert(upper == 0)
    end)

    it('ranged bounds are finite for equal att/def', function()
        local lower, upper = p.rangedPDIFBoundsFromParams({
            actorAttack   = 100,
            targetDefense = 100,
            actorIsPC     = true,
            weaponCap     = 3.25,
        })
        -- cRatio 1.0 → caps (1, 1)
        assert(lower == 1)
        assert(upper == 1)
    end)
end)
