-----------------------------------
-- Pure system tests for Beastmaster dual-wire (slice 6737).
-- Calls production xi.job_utils.beastmaster pure exports.
-- Goldens match internal/beastmaster (0886).
-----------------------------------

require('scripts/globals/job_utils/beastmaster')

local b = xi.job_utils.beastmaster

local function almost(a, c)
    return math.abs(a - c) < 1e-6
end

describe('Beastmaster pure pins', function()
    it('bases and pet food', function()
        assert(b.charmChanceBase == 50 and b.charmChanceCap == 95)
        assert(almost(b.charmDurationCHRScale, 1.25) and b.charmDurationCHRAddend == 150)
        assert(almost(b.charmTimePerMod, 0.05))
        assert(b.rewardRegenDuration == 180 and b.rewardRegenTick == 3)
        assert(b.stayTickMax == 10 and b.stayTickMin == 5)
        assert(b.killerInstinctBaseDuration == 180 and b.killerInstinctMeritUnit == 10)
        assert(b.spurBasePower == 20 and b.spurJPAttackPerLevel == 3 and b.spurDuration == 90)
        assert(b.feralHowlBaseDuration == 10 and b.feralHowlMeritUnit == 5)
        assert(b.oneHourRecastSecondsPerMod == 60)
        assert(b.msgCannotCharm == 210 and b.msgShouldBeAbleCharm == 214)

        local alpha = b.petFoodData[xi.item.PET_FOOD_ALPHA_BISCUIT]
        assert(alpha and alpha.minHealing == 50 and alpha.regen == 1)
        local theta = b.petFoodData[xi.item.PET_FOOD_THETA_BISCUIT]
        assert(theta and theta.minHealing == 1600 and theta.regen == 20)
    end)
end)

describe('Charm chance duration gauge', function()
    it('chance duration message', function()
        assert(b.charmChanceFromParams({ eligible = false }) == 0)

        local ch = b.charmChanceFromParams({
            eligible = true, charmerBSTLevel = 75, targetLevel = 75,
            charmRes = 0, lightResRank = 0, includeMods = false,
            charmChanceMod = 0, charmerCHR = 100, targetCHR = 100,
        })
        assert(almost(ch, 50))

        -- clamp high
        ch = b.charmChanceFromParams({
            eligible = true, charmerBSTLevel = 99, targetLevel = 1,
            charmRes = 0, lightResRank = -3, includeMods = true,
            charmChanceMod = 50, charmerCHR = 200, targetCHR = 0,
        })
        assert(almost(ch, 95))

        -- dLvl mid ladder (target 51..70 → −5 per level)
        ch = b.charmChanceFromParams({
            eligible = true, charmerBSTLevel = 55, targetLevel = 60,
            charmRes = 0, lightResRank = 0, includeMods = false,
            charmChanceMod = 0, charmerCHR = 100, targetCHR = 100,
        })
        -- 50 - 5*5 = 25
        assert(almost(ch, 25))

        -- dLvl high ladder (target ≥71 → −10 per level)
        ch = b.charmChanceFromParams({
            eligible = true, charmerBSTLevel = 70, targetLevel = 75,
            charmRes = 0, lightResRank = 0, includeMods = false,
            charmChanceMod = 0, charmerCHR = 100, targetCHR = 100,
        })
        -- 50 - 10*5 = 0
        assert(almost(ch, 0))

        assert(almost(b.dLvlCharmMultiplierFromParams(-7), 1 / 24))
        assert(almost(b.dLvlCharmMultiplierFromParams(9), 6))
        assert(almost(b.dLvlCharmMultiplierFromParams(0), 0.9997336))

        -- duration: floor(1.25*100+150)=275 at dLvl 0 ~ *0.9997336
        local dur = b.charmDurationFromParams({
            charmerCHR = 100, charmerLevel = 75, targetLevel = 75, charmTimeMod = 0,
        })
        assert(dur == math.floor(275 * 0.9997336))

        assert(b.gaugeMessageFromParams(80) == 214)
        assert(b.gaugeMessageFromParams(50) == 213)
        assert(b.gaugeMessageFromParams(25) == 212)
        assert(b.gaugeMessageFromParams(1) == 211)
        assert(b.gaugeMessageFromParams(0) == 210)
    end)
end)

describe('jug reward stay killer spur feral recast', function()
    it('products', function()
        assert(b.validJugPetIDFromParams({
            ammoSubSkill = 21, ammoSkill = 0, ammoPresent = true,
            ammoReqLevel = 1, playerMainLevel = 75,
        }) == 21)
        assert(b.validJugPetIDFromParams({
            ammoSubSkill = 21, ammoSkill = 1, ammoPresent = true,
            ammoReqLevel = 1, playerMainLevel = 75,
        }) == nil)
        assert(b.validJugPetIDFromParams({
            ammoSubSkill = 10, ammoSkill = 0, ammoPresent = true,
            ammoReqLevel = 1, playerMainLevel = 75,
        }) == nil)

        local heal, ok = b.rewardHealingFromParams({
            foodItemId = xi.item.PET_FOOD_ALPHA_BISCUIT,
            playerMND = 20, rewardHPBonus = 0, petMissingHP = 9999,
        })
        -- 50 + 2*(20-10) = 70
        assert(ok and heal == 70)

        heal, ok = b.rewardHealingFromParams({
            foodItemId = xi.item.PET_FOOD_ALPHA_BISCUIT,
            playerMND = 20, rewardHPBonus = 50, petMissingHP = 9999,
        })
        -- 70 + floor(70*50/100) = 70+35 = 105
        assert(ok and heal == 105)

        heal, ok = b.rewardHealingFromParams({
            foodItemId = xi.item.PET_FOOD_ALPHA_BISCUIT,
            playerMND = 20, rewardHPBonus = 0, petMissingHP = 40,
        })
        assert(ok and heal == 40)

        local regen
        regen, ok = b.rewardRegenFromParams(xi.item.PET_FOOD_THETA_BISCUIT)
        assert(ok and regen == 20)

        assert(b.stayHealingTickFromParams(0) == 10)
        assert(b.stayHealingTickFromParams(20) == 9)
        assert(b.stayHealingTickFromParams(99) == 5)

        assert(b.killerInstinctDurationFromParams(10) == 180)
        assert(b.killerInstinctDurationFromParams(30) == 200)

        local power, sub = b.spurPowersFromParams({ enhancesSpur = 5, spurJP = 4 })
        assert(power == 25 and sub == 12)

        assert(b.feralHowlDurationFromParams({ meritValue = 25, feralHowlDurationMod = 0 }) == 10)
        assert(b.feralHowlDurationFromParams({ meritValue = 25, feralHowlDurationMod = 1 }) == 15)

        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(b.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(b.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 5 }) == 0)
    end)
end)
