-----------------------------------
-- Pure system tests for shadow mitigation dual-wire helpers (slice 6706).
-- Calls production utils pure exports (not local copies).
-- Goldens match internal/shadowabsorb (0877 / 6040).
-----------------------------------

require('scripts/utils/combat_utils')

describe('Shadow pure constants and copyImageIcon', function()
    it('pins chance thresholds', function()
        assert(utils.shadowDefaultProcChance == 50)
        assert(utils.shadowBlinkFailThreshold == 20)
        assert(utils.shadowTakeShadowsBlinkProc == 80)
    end)

    it('maps remaining to Copy Image icons', function()
        assert(utils.copyImageIcon(0) == 0)
        assert(utils.copyImageIcon(-1) == 0)
        assert(utils.copyImageIcon(1) == xi.effect.COPY_IMAGE)
        assert(utils.copyImageIcon(2) == xi.effect.COPY_IMAGE_2)
        assert(utils.copyImageIcon(3) == xi.effect.COPY_IMAGE_3)
        assert(utils.copyImageIcon(4) == xi.effect.COPY_IMAGE_4)
        assert(utils.copyImageIcon(5) == xi.effect.COPY_IMAGE_4)
    end)
end)

describe('takeShadowsDamage', function()
    it('zeros full block and truncates partial', function()
        assert(utils.takeShadowsDamage(100, 2, 2) == 0)
        assert(utils.takeShadowsDamage(100, 2, 1) == 50)
        assert(utils.takeShadowsDamage(100, 3, 1) == 66)
        assert(utils.takeShadowsDamage(100, 0, 0) == 100) -- remove<=0 → 1; used 0 → full damage
    end)
end)

describe('attemptShadowMitigationFromParams', function()
    it('gates non-NIN / no Utsusemi / zero attempts', function()
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 4, isNIN = true, hasUtsusemi = true, rolls = { 1, 1, 1, 1 },
        }) == 3) -- cap attempted-1
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 0, isNIN = true, hasUtsusemi = true, rolls = { 1 },
        }) == 0)
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 4, isNIN = false, hasUtsusemi = true, rolls = { 1, 1, 1, 1 },
        }) == 0)
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 4, isNIN = true, hasUtsusemi = false, rolls = { 1, 1, 1, 1 },
        }) == 0)
    end)

    it('counts rolls and caps at attempted-1', function()
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 4, isNIN = true, hasUtsusemi = true,
            rolls = { 50, 51, 1, 100 },
        }) == 2)
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 3, isNIN = true, hasUtsusemi = true, rolls = { 1 },
        }) == 1) -- missing rolls fail
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 2, isNIN = true, hasUtsusemi = true,
            procChance = 10, rolls = { 10, 11 },
        }) == 1)
        assert(utils.attemptShadowMitigationFromParams({
            attemptedRemovals = 1, isNIN = true, hasUtsusemi = true, rolls = { 1 },
        }) == 0)
    end)
end)

describe('takeShadowsFromParams', function()
    it('returns full damage with no shadows', function()
        local res = utils.takeShadowsFromParams({ damage = 100, shadowsToRemove = 2 })
        assert(res.damage == 100 and res.used == 0 and res.modID == 0)
    end)

    it('utsusemi full absorb and icon', function()
        local res = utils.takeShadowsFromParams({ damage = 100, utsusemi = 3, shadowsToRemove = 2 })
        assert(res.damage == 0 and res.used == 2 and res.remaining == 1)
        assert(res.usedUtsusemi and res.modID == xi.mod.UTSUSEMI)
        assert(res.setIcon and res.icon == xi.effect.COPY_IMAGE)
        assert(not res.delCopyImage and not res.delBlink)
    end)

    it('utsusemi partial truncates damage and deletes', function()
        local res = utils.takeShadowsFromParams({ damage = 100, utsusemi = 1, shadowsToRemove = 3 })
        assert(res.damage == 66 and res.used == 1 and res.remaining == 0)
        assert(res.delCopyImage and res.delBlink)
    end)

    it('prefers utsusemi over blink', function()
        local res = utils.takeShadowsFromParams({
            damage = 50, utsusemi = 2, blink = 5, shadowsToRemove = 1,
        })
        assert(res.usedUtsusemi and not res.usedBlink and res.remaining == 1)
    end)

    it('blink rolls full partial and miss', function()
        local res = utils.takeShadowsFromParams({
            damage = 100, blink = 3, shadowsToRemove = 2, blinkRolls = { 1, 1 },
        })
        assert(res.damage == 0 and res.used == 2 and res.remaining == 1)

        res = utils.takeShadowsFromParams({
            damage = 100, blink = 3, shadowsToRemove = 2, blinkRolls = { 1, 100 },
        })
        assert(res.damage == 50 and res.used == 1 and res.remaining == 2)

        res = utils.takeShadowsFromParams({
            damage = 100, blink = 3, shadowsToRemove = 2, blinkRolls = { 100, 100 },
        })
        assert(res.damage == 100 and res.used == 0 and res.remaining == 3)
    end)

    it('defaults shadowsToRemove to 1', function()
        local res = utils.takeShadowsFromParams({ damage = 100, utsusemi = 2, shadowsToRemove = 0 })
        assert(res.damage == 0 and res.used == 1 and res.remaining == 1)
    end)
end)

describe('shadowAbsorbFromParams', function()
    it('misses with no shadows', function()
        local res = utils.shadowAbsorbFromParams({ utsusemi = 0, blink = 0, shadowsToRemove = 1 })
        assert(not res.absorbHit and res.consumed == 0)
    end)

    it('utsusemi full block and icon ladder', function()
        local res = utils.shadowAbsorbFromParams({
            utsusemi = 3, shadowsToRemove = 2, hasCopyImageEffect = true,
        })
        assert(res.absorbHit and res.consumed == 2 and res.remaining == 1)
        assert(res.usedUtsusemi and res.setIcon and res.icon == xi.effect.COPY_IMAGE)

        res = utils.shadowAbsorbFromParams({
            utsusemi = 5, shadowsToRemove = 1, hasCopyImageEffect = true,
        })
        assert(res.remaining == 4 and res.icon == xi.effect.COPY_IMAGE_4)
    end)

    it('utsusemi partial does not absorb hit', function()
        local res = utils.shadowAbsorbFromParams({
            utsusemi = 1, shadowsToRemove = 3, hasCopyImageEffect = true,
        })
        assert(not res.absorbHit and res.consumed == 1 and res.remaining == 0)
        assert(res.delCopyImage)
    end)

    it('utsusemi precedes blink', function()
        local res = utils.shadowAbsorbFromParams({
            utsusemi = 2, blink = 9, shadowsToRemove = 1, hasCopyImageEffect = true,
        })
        assert(res.usedUtsusemi and not res.usedBlink and res.remaining == 1)
    end)

    it('blink fail roll and success', function()
        local res = utils.shadowAbsorbFromParams({
            blink = 2, shadowsToRemove = 1, blinkFailRoll = 20,
        })
        assert(not res.absorbHit and res.consumed == 0 and not res.usedBlink)

        res = utils.shadowAbsorbFromParams({
            blink = 2, shadowsToRemove = 1, blinkFailRoll = 21,
        })
        assert(res.absorbHit and res.consumed == 1 and res.remaining == 1 and res.usedBlink)

        res = utils.shadowAbsorbFromParams({
            blink = 1, shadowsToRemove = 1, blinkFailRoll = 100,
        })
        assert(res.absorbHit and res.remaining == 0 and res.delBlink)
    end)
end)
