-----------------------------------
-- Pure system tests for Samurai dual-wire (slice 6734).
-- Calls production xi.job_utils.samurai pure exports.
-- Goldens match internal/samurai (0887).
-----------------------------------

require('scripts/globals/job_utils/samurai')

local s = xi.job_utils.samurai

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Samurai pure pins', function()
    it('bases and messages', function()
        assert(s.wardingCircleMainPower == 15 and s.wardingCircleSubPower == 5)
        assert(s.wardingCircleBaseDuration == 180)
        assert(s.hassoDuration == 300 and s.hassoLevelDivisor == 7)
        assert(s.meditateSubAmount == 12 and s.meditateMainBase == 20)
        assert(s.meditateJPPerLevel == 5 and s.meditateBaseDuration == 15 and s.meditateTick == 3)
        assert(s.shikikoyoKeepTP == 1000 and s.shikikoyoMeritBaseline == 12 and s.shikikoyoTPCap == 3000)
        assert(s.meikyoShisuiDuration == 30 and s.meikyoShisuiTPGrant == 3000)
        assert(s.yaegasumiPower == 12 and s.yaegasumiDuration == 45)
        assert(s.konzenMissInfo == 1 and s.konzenHitInfo == 5 and s.konzenHitReturn == 3)
        assert(s.bladeBashPlagueBase == 15 and s.bladeBashStunDuration == 6)
        assert(s.msgNeeds2HWeapon == 307 and s.msgRequiresCombat == 525)
        assert(s.msgCannotPerformTarg == 72 and s.msgNotEnoughTP == 192)
    end)
end)

describe('ability check pure gates', function()
    it('2H combat Shikikoyo', function()
        local msg, ok = s.checkTwoHandedWeaponFromParams({ isTwoHanded = true })
        assert(ok and msg == 0)
        msg, ok = s.checkTwoHandedWeaponFromParams({ isTwoHanded = false })
        assert(not ok and msg == 307)

        msg, ok = s.checkKonzenIttaiFromParams({ inCombat = true })
        assert(ok and msg == 0)
        msg, ok = s.checkKonzenIttaiFromParams({ inCombat = false })
        assert(not ok and msg == 525)

        msg, ok = s.checkShikikoyoFromParams({ selfTarget = true, playerTP = 3000 })
        assert(not ok and msg == 72)
        msg, ok = s.checkShikikoyoFromParams({ selfTarget = false, playerTP = 999 })
        assert(not ok and msg == 192)
        msg, ok = s.checkShikikoyoFromParams({ selfTarget = false, playerTP = 1000 })
        assert(ok and msg == 0)
    end)
end)

describe('recast pure', function()
    it('oneHour and thirdEye', function()
        assert(s.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(s.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(s.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)

        assert(s.thirdEyeRecastFromParams({ abilityRecast = 60, hasSeigan = false, isTwoHanded = true }) == 60)
        assert(s.thirdEyeRecastFromParams({ abilityRecast = 60, hasSeigan = true, isTwoHanded = false }) == 60)
        assert(s.thirdEyeRecastFromParams({ abilityRecast = 60, hasSeigan = true, isTwoHanded = true }) == 30)
        assert(s.thirdEyeRecastFromParams({ abilityRecast = 61, hasSeigan = true, isTwoHanded = true }) == 30)
    end)
end)

describe('WardingCircle Hasso Meditate ThirdEye', function()
    it('power duration products', function()
        assert(s.wardingCirclePowerFromParams({ mainJobSAM = true }) == 15)
        assert(s.wardingCirclePowerFromParams({ mainJobSAM = false }) == 5)
        assert(s.wardingCirclePowerFromParams({ mainJobSAM = true, potencyMod = 2 }) == 17)
        assert(s.wardingCircleDurationFromParams({}) == 180)
        assert(s.wardingCircleDurationFromParams({ durationMod = 30 }) == 210)

        assert(almost(s.hassoSTRBoostFromParams({
            mainJobSAM = true, mainLvl = 70,
        }), 10))
        assert(almost(s.hassoSTRBoostFromParams({
            mainJobSAM = true, mainLvl = 75, hassoJP = 5,
        }), 75 / 7 + 5))
        assert(almost(s.hassoSTRBoostFromParams({
            mainJobSAM = false, subJobSAM = true, subLvl = 35, hassoJP = 99,
        }), 5))
        assert(almost(s.hassoSTRBoostFromParams({
            mainJobSAM = false, subJobSAM = false, mainLvl = 99,
        }), 0))

        local med = s.meditateFromParams({ mainJobSAM = false, meditateJP = 10, durationMod = 5 })
        assert(med.amount == 12 and med.duration == 20 and med.tick == 3)
        med = s.meditateFromParams({ mainJobSAM = true, meditateJP = 4 })
        assert(med.amount == 40 and med.duration == 15 and med.tick == 3)

        assert(s.thirdEyeAppliesFromParams({ hasCopyImage = false, hasBlink = false }))
        assert(not s.thirdEyeAppliesFromParams({ hasCopyImage = true, hasBlink = false }))
        assert(not s.thirdEyeAppliesFromParams({ hasCopyImage = false, hasBlink = true }))
    end)
end)

describe('Shikikoyo Hamanoha BladeBash Konzen', function()
    it('transfer duration animations', function()
        assert(almost(s.shikikoyoTransferFromParams({
            playerTP = 3000, targetTP = 0, meritValue = 12,
        }), 2000))
        assert(almost(s.shikikoyoTransferFromParams({
            playerTP = 3000, targetTP = 0, meritValue = 17,
        }), 2100))
        assert(almost(s.shikikoyoTransferFromParams({
            playerTP = 3000, targetTP = 2500, meritValue = 12,
        }), 500))
        assert(almost(s.shikikoyoTransferFromParams({
            playerTP = 3000, targetTP = 3000, meritValue = 12,
        }), 0))
        assert(almost(s.shikikoyoTransferFromParams({
            playerTP = 1000, targetTP = 0, meritValue = 12,
        }), 0))
        assert(almost(s.shikikoyoTransferFromParams({
            playerTP = 2000, targetTP = 0, meritValue = 7,
        }), 950))

        assert(s.hamanohaDurationFromParams({}) == 180)
        assert(s.hamanohaDurationFromParams({ hamanohaJP = 20 }) == 200)
        assert(s.bladeBashPlagueDurationBase(0) == 15)
        assert(s.bladeBashPlagueDurationBase(15) == 30)

        assert(s.konzenAnimation(xi.skill.GREAT_KATANA) == 44)
        assert(s.konzenAnimation(xi.skill.POLEARM) == 42)
        assert(s.konzenAnimation(99) == 37)
        assert(s.bladeBashAnimation(xi.skill.GREAT_SWORD) == 201)
        assert(s.bladeBashAnimation(xi.skill.SCYTHE) == 202)
        assert(s.bladeBashAnimation(xi.skill.POLEARM) == 203)
        assert(s.bladeBashAnimation(1) == 0)

        local hit = s.konzenIttaiFromParams({
            weaponSkill = xi.skill.GREAT_KATANA, hasChainbound = false, hasSkillchain = false,
        })
        assert(hit.applied and hit.info == 5 and hit.returnValue == 3 and not hit.noEffectMsg)
        assert(hit.animation == 44)

        local miss = s.konzenIttaiFromParams({
            weaponSkill = xi.skill.POLEARM, hasChainbound = true, hasSkillchain = false,
        })
        assert(not miss.applied and miss.info == 1 and miss.returnValue == 0 and miss.noEffectMsg)
        assert(miss.animation == 42)
    end)
end)
