-----------------------------------
-- Pure system tests for damage addition dual-wire helpers (slice 6697).
-- Calls production xi.combat.damage pure exports.
-- Goldens match internal/damageadditions (6084 / 6091).
-----------------------------------

require('scripts/globals/combat/damage_additions')

local dmg = xi.combat.damage

describe('damage additions constants', function()
    it('pins base Souleater fraction and DRK job', function()
        assert(math.abs(dmg.souleaterBaseFraction - 0.1) < 1e-12)
        assert(xi.job.DRK == 8)
    end)
end)

describe('souleaterBonusFromParams', function()
    it('returns 0 without effect', function()
        assert(dmg.souleaterBonusFromParams({ hasEffect = false, hp = 1000 }) == 0)
    end)

    it('floors HP * (0.1 + mods/100)', function()
        assert(dmg.souleaterBonusFromParams({ hasEffect = true, hp = 1000 }) == 100)
        assert(dmg.souleaterBonusFromParams({
            hasEffect = true, hp = 1000, souleaterEffect = 5, souleaterEffectII = 2,
        }) == 170)
        assert(dmg.souleaterBonusFromParams({ hasEffect = true, hp = 999 }) == 99)
    end)
end)

describe('souleaterSelfDamageFromParams', function()
    it('returns 0 for non-positive bonus', function()
        assert(dmg.souleaterSelfDamageFromParams({ bonusDamage = 0, stalwartSoulPercent = 10 }) == 0)
        assert(dmg.souleaterSelfDamageFromParams({ bonusDamage = -5 }) == 0)
    end)

    it('scales by (1 - stalwart/100)', function()
        assert(dmg.souleaterSelfDamageFromParams({ bonusDamage = 100, stalwartSoulPercent = 0 }) == 100)
        assert(dmg.souleaterSelfDamageFromParams({ bonusDamage = 100, stalwartSoulPercent = 20 }) == 80)
    end)
end)

describe('souleaterAdditionFromParams', function()
    it('returns full bonus for DRK and half for others', function()
        assert(dmg.souleaterAdditionFromParams({
            hasEffect = false, hp = 1000, mainJob = xi.job.DRK,
        }) == 0)
        assert(dmg.souleaterAdditionFromParams({
            hasEffect = true, hp = 1000, mainJob = xi.job.DRK,
        }) == 100)
        assert(dmg.souleaterAdditionFromParams({
            hasEffect = true, hp = 1000, souleaterEffect = 5, souleaterEffectII = 2,
            mainJob = xi.job.DRK,
        }) == 170)
        assert(dmg.souleaterAdditionFromParams({
            hasEffect = true, hp = 1000, mainJob = xi.job.WAR,
        }) == 50)
        assert(dmg.souleaterAdditionFromParams({
            hasEffect = true, hp = 0, mainJob = xi.job.WAR,
        }) == 0)
        assert(dmg.souleaterAdditionFromParams({
            hasEffect = true, hp = 999, mainJob = xi.job.WAR,
        }) == 49)
    end)
end)

describe('consumeManaAdditionFromParams', function()
    it('returns floor(mp/10) with effect', function()
        assert(dmg.consumeManaAdditionFromParams({ hasEffect = false, mp = 500 }) == 0)
        assert(dmg.consumeManaAdditionFromParams({ hasEffect = true, mp = 500 }) == 50)
        assert(dmg.consumeManaAdditionFromParams({ hasEffect = true, mp = 509 }) == 50)
        assert(dmg.consumeManaAdditionFromParams({ hasEffect = true, mp = 0 }) == 0)
    end)
end)
