-----------------------------------
-- Pure system tests for Ranger dual-wire (slice 6746).
-- Calls production xi.job_utils.ranger pure exports.
-- Goldens match internal/ranger (0902).
-----------------------------------

require('scripts/globals/job_utils/ranger')

local r = xi.job_utils.ranger

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Ranger pure pins', function()
    it('bases and fixed params', function()
        assert(r.sharpshotBasePower == 40 and r.sharpshotDuration == 60)
        assert(r.velocityShotPower == 1 and r.velocityShotDuration == 7200)
        assert(r.barrageDuration == 60)
        assert(r.unlimitedShotPower == 1 and r.unlimitedShotDuration == 60)
        assert(r.doubleShotPower == 40 and r.doubleShotDuration == 90)
        assert(r.decoyShotPower == 11 and r.decoyShotDuration == 30 and r.decoyShotTick == 1)
        assert(r.overkillPower == 11 and r.overkillDuration == 60 and r.overkillTick == 1)
        assert(r.camouflageBaseRollMin == 30 and r.camouflageBaseRollMax == 300)
        assert(r.shadowbindBaseDuration == 30)
        assert(r.scavengeArrowsUsedMod == 10000 and r.scavengeArrowsMax == 99)
        assert(r.msgNoRangedWeapon == 216 and r.msgCannotAttackTarget == 446)
        assert(r.msgScavengeFindNothing == 139 and r.msgScavengeFindItem == 140)
        assert(r.msgScavengeFindItems == 674 and r.msgIsEffect == 277 and r.msgJAMiss == 158)
        assert(r.oneHourRecastSecondsPerMod == 60)

        local p = r.velocityShotFromParams()
        assert(p.power == 1 and p.duration == 7200)
        p = r.sharpshotFromParams({ sharpshotMod = 5 })
        assert(p.power == 45 and p.duration == 60)
        p = r.doubleShotFromParams()
        assert(p.power == 40 and p.duration == 90)
        p = r.decoyShotFromParams()
        assert(p.power == 11 and p.duration == 30 and p.tick == 1)
        p = r.overkillFromParams()
        assert(p.power == 11 and p.duration == 60 and p.tick == 1)
    end)
end)

describe('oneHour scavenge camouflage shadowbind', function()
    it('products', function()
        assert(r.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(r.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(r.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)

        assert(r.sharpshotPowerFromParams(0) == 40)
        assert(r.sharpshotPowerFromParams(10) == 50)

        local id, low = r.decodeArrowsUsedFromParams(12345)
        assert(id == 1 and low == 2345)
        id, low = r.decodeArrowsUsedFromParams(0)
        assert(id == 0 and low == 0)

        -- floor(100 * (75/200 + 0)) = floor(37.5) = 37
        assert(r.scavengeArrowsToReturnFromParams({
            arrowsUsedLow = 100, mainLvl = 75, scavengeMod = 0, scavengeMerit = 0,
        }) == 37)
        -- cap 99
        assert(r.scavengeArrowsToReturnFromParams({
            arrowsUsedLow = 10000, mainLvl = 99, scavengeMod = 100, scavengeMerit = 0,
        }) == 99)
        assert(r.scavengeArrowsToReturnFromParams({
            arrowsUsedLow = 0, mainLvl = 99, scavengeMod = 0, scavengeMerit = 0,
        }) == 0)

        assert(r.scavengeMessageFromParams(0) == 139)
        assert(r.scavengeMessageFromParams(1) == 140)
        assert(r.scavengeMessageFromParams(5) == 674)

        -- base 100, mod 0, mult 1 → 100
        assert(r.camouflageDurationFromParams({
            baseRoll = 100, camouflageDurationMod = 0, sneakInvisMultiplier = 1,
        }) == 100)
        -- base 100, mod 50 → 100 * 1.5 = 150; mult 2 → 300
        assert(r.camouflageDurationFromParams({
            baseRoll = 100, camouflageDurationMod = 50, sneakInvisMultiplier = 2,
        }) == 300)

        assert(r.shadowbindDurationFromParams({ shadowBindExt = 5, jpDuration = 3 }) == 38)
        assert(r.shadowbindSuccessFromParams({ bindMeva = 50, roll0to99 = 50, alreadyBound = false }))
        assert(not r.shadowbindSuccessFromParams({ bindMeva = 50, roll0to99 = 49, alreadyBound = false }))
        assert(not r.shadowbindSuccessFromParams({ bindMeva = 0, roll0to99 = 99, alreadyBound = true }))
        assert(r.shadowbindMsgFromParams(true) == 277)
        assert(r.shadowbindMsgFromParams(false) == 158)
    end)
end)

describe('check gates and anim offsets', function()
    it('gates anims', function()
        local msg, ok = r.checkEagleEyeShotFromParams({
            hasRangedWeapon = true, skillType = 25, hasAmmoWeapon = true,
        })
        assert(ok and msg == 0)
        msg, ok = r.checkEagleEyeShotFromParams({
            hasRangedWeapon = true, skillType = 27, hasAmmoWeapon = false,
        })
        assert(ok and msg == 0) -- throwing
        msg, ok = r.checkEagleEyeShotFromParams({
            hasRangedWeapon = false, skillType = 25, hasAmmoWeapon = true,
        })
        assert(not ok and msg == 216)

        msg, ok = r.checkShadowbindFromParams({ rangedSkill = 25, ammoSkill = 25 })
        assert(ok and msg == 0)
        msg, ok = r.checkShadowbindFromParams({ rangedSkill = 26, ammoSkill = 26 })
        assert(ok and msg == 0)
        msg, ok = r.checkShadowbindFromParams({ rangedSkill = 25, ammoSkill = 26 })
        assert(not ok and msg == 216)

        msg, ok = r.checkBountyShotFromParams({
            isMob = false, rangedSkill = 25, ammoSkill = 25,
        })
        assert(not ok and msg == 446)
        msg, ok = r.checkBountyShotFromParams({
            isMob = true, rangedSkill = 25, ammoSkill = 25,
        })
        assert(ok and msg == 0)
        msg, ok = r.checkBountyShotFromParams({
            isMob = true, rangedSkill = 25, ammoSkill = 26,
        })
        assert(not ok and msg == 216)

        assert(r.marksmanAnimOffsetFromParams(26) == 1)
        assert(r.marksmanAnimOffsetFromParams(25) == 0)
        assert(r.bountyShotAnimOffsetFromParams(25) == -1)
        assert(r.bountyShotAnimOffsetFromParams(26) == 0)

        assert(r.isRangedCombatSkillFromParams(25))
        assert(r.isRangedCombatSkillFromParams(26))
        assert(r.isRangedCombatSkillFromParams(27))
        assert(not r.isRangedCombatSkillFromParams(1))
        assert(r.hasMatchingRangedPairFromParams({ rangedSkill = 25, ammoSkill = 25 }))
        assert(not r.hasMatchingRangedPairFromParams({ rangedSkill = 25, ammoSkill = 26 }))
    end)
end)
