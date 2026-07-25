-----------------------------------
-- Pure system tests for singleHitOutcomeFromParams dual-wire (slice 6766).
-- Calls production xi.weaponskills pure exports.
-- Goldens match internal/wsformula SingleHitOutcome.
-----------------------------------

require('scripts/globals/weaponskills')

local ws = xi.weaponskills

describe('singleHitOutcomeFromParams', function()
    it('miss keeps counters and zero damage', function()
        local dmg, hits, guarded, pdif, crit, shadows = ws.singleHitOutcomeFromParams({
            missed = true, hitsLanded = 2, guardedHits = 1, pdif = 1.5, criticalHit = true,
        })
        assert(dmg == 0)
        assert(hits == 2)
        assert(guarded == 1)
        assert(pdif == 1.5)
        assert(crit == true)
        assert(shadows == 0)
    end)

    it('parry is same as miss for counters', function()
        local dmg, hits = ws.singleHitOutcomeFromParams({
            parried = true, hitsLanded = 5,
        })
        assert(dmg == 0)
        assert(hits == 5)
    end)

    it('shadow absorb increments shadowsDelta', function()
        local dmg, hits, guarded, pdif, crit, shadows = ws.singleHitOutcomeFromParams({
            shadowAbsorbed = true, hitsLanded = 0,
        })
        assert(dmg == 0)
        assert(shadows == 1)
        assert(hits == 0)
    end)

    it('landed hit applies damage, crit sticky, and guard', function()
        local dmg, hits, guarded, pdif, crit = ws.singleHitOutcomeFromParams({
            critVaries  = true,
            critChance  = 0.1,
            critRate    = 0.2,
            dmg         = 100,
            ftp         = 1,
            pdif        = 2,
            hitsLanded  = 0,
            guardedHits = 0,
            isPhysical  = true,
            isGuarded   = true,
        })
        assert(dmg == 200)
        assert(hits == 1)
        assert(guarded == 1)
        assert(pdif == 1)
        assert(crit == true)
    end)

    it('block subtracts reduction from product', function()
        local dmg = ws.singleHitOutcomeFromParams({
            dmg = 100, ftp = 1, pdif = 1,
            blocked = true, blockReduction = 25,
            hitsLanded = 0, isPhysical = true,
        })
        assert(dmg == 75)
    end)
end)
