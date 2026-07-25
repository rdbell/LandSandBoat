-----------------------------------
-- Pure system tests for checkAnticipated dual-wire (slice 6765 / 6101).
-- Calls production xi.combat.physicalHitRate pure exports.
-- Goldens match internal/physhitrate CheckAnticipated.
-----------------------------------

require('scripts/globals/combat/physical_hit_rate')

local phr = xi.combat.physicalHitRate

describe('thirdEyeRetentionScaledChance', function()
    it('fresh effect is 10000; 30s is 0', function()
        assert(phr.thirdEyeRetentionScaledChance(0, 0) == 10000)
        assert(phr.thirdEyeRetentionScaledChance(30000, 0) == 0)
        assert(phr.thirdEyeRetentionScaledChance(15000, 0) == 5000)
        assert(phr.thirdEyeRetentionScaledChance(15000, 50) == 7500)
        assert(phr.thirdEyeRetentionScaledChance(60000, 100) == 10000)
        assert(phr.thirdEyeRetentionScaledChance(-100, 0) == 10000)
    end)
end)

describe('canRetainThirdEye', function()
    it('non-PC always; PC only two-handed', function()
        assert(phr.canRetainThirdEye(false, false) == true)
        assert(phr.canRetainThirdEye(true, false) == false)
        assert(phr.canRetainThirdEye(true, true) == true)
    end)
end)

describe('checkAnticipatedFromParams', function()
    it('no Third Eye → not anticipated, no delete', function()
        local anticipated, shouldDelete = phr.checkAnticipatedFromParams({
            hasThirdEye = false, hasSeigan = true, isPC = true,
            isWeaponTwoHanded = true, roll1to10000 = 1,
        })
        assert(anticipated == false)
        assert(shouldDelete == false)
    end)

    it('TE without Seigan always anticipates and deletes', function()
        local anticipated, shouldDelete = phr.checkAnticipatedFromParams({
            hasThirdEye = true, hasSeigan = false, isPC = true,
            isWeaponTwoHanded = true, roll1to10000 = 1,
        })
        assert(anticipated == true)
        assert(shouldDelete == true)
    end)

    it('Seigan + retain chance keeps TE when roll succeeds', function()
        local anticipated, shouldDelete = phr.checkAnticipatedFromParams({
            hasThirdEye = true, hasSeigan = true, isPC = false,
            isWeaponTwoHanded = false, timeInEffectMs = 0,
            retentionRateMod = 0, roll1to10000 = 1,
        })
        assert(anticipated == true)
        assert(shouldDelete == false)
    end)

    it('Seigan + expired retention deletes TE', function()
        local anticipated, shouldDelete = phr.checkAnticipatedFromParams({
            hasThirdEye = true, hasSeigan = true, isPC = false,
            timeInEffectMs = 60000, retentionRateMod = 0, roll1to10000 = 1,
        })
        assert(anticipated == true)
        assert(shouldDelete == true)
    end)

    it('PC one-handed cannot retain even with Seigan', function()
        local anticipated, shouldDelete = phr.checkAnticipatedFromParams({
            hasThirdEye = true, hasSeigan = true, isPC = true,
            isWeaponTwoHanded = false, timeInEffectMs = 0, roll1to10000 = 1,
        })
        assert(anticipated == true)
        assert(shouldDelete == true)
    end)
end)
