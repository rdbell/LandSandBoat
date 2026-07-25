-----------------------------------
-- Pure system tests for Dark Knight dual-wire (slice 6732).
-- Calls production xi.job_utils.dark_knight pure exports.
-- Goldens match internal/darkknight (0893).
-----------------------------------

require('scripts/globals/job_utils/dark_knight')

local d = xi.job_utils.dark_knight

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Dark Knight pure pins', function()
    it('bases and messages', function()
        assert(d.arcaneCircleMainPower == 15 and d.arcaneCircleSubPower == 5)
        assert(d.arcaneCircleBaseDuration == 180)
        assert(d.arcaneCrestPower == 20 and d.arcaneCrestBaseDuration == 180)
        assert(d.bloodWeaponPower == 1 and d.bloodWeaponBaseDuration == 30)
        assert(d.darkSealMeritOffset == 10 and d.darkSealDuration == 60)
        assert(d.diabolicEyeBasePower == 15 and d.diabolicEyeMeritPowerScale == 5)
        assert(d.netherVoidBasePower == 50 and d.netherVoidJPScale == 2)
        assert(d.scarletDeliriumBaseDuration == 90)
        assert(d.souleaterPower == 1 and d.souleaterBaseDuration == 60)
        assert(d.weaponBashLevelOffset == 11 and d.weaponBashJPScale == 10)
        assert(d.ecosystemArcana == 3)
        assert(d.msgCannotOnThatTarg == 155 and d.msgNeeds2HWeapon == 307)
        assert(d.lastResortDuration == 180)
        assert(d.consumeManaPower == 1 and d.consumeManaDuration == 60)
        assert(d.soulEnslavementDuration == 30)
    end)
end)

describe('checks and oneHourRecast', function()
    it('gates and recast', function()
        assert(d.checkArcaneCrestFromParams({ targetEcosystem = d.ecosystemArcana }) == 0)
        assert(d.checkArcaneCrestFromParams({ targetEcosystem = 1 }) == 155)
        assert(d.checkWeaponBashFromParams({ isTwoHanded = true }) == 0)
        assert(d.checkWeaponBashFromParams({ isTwoHanded = false }) == 307)

        assert(d.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)
        assert(d.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(d.oneHourRecastFromParams({ abilityRecast = 60, oneHourRecastMod = 5 }) == 0)

        assert(d.activeJobLevel(xi.job.DRK, 1, xi.job.DRK, 99, 49) == 99)
        assert(d.activeJobLevel(1, xi.job.DRK, xi.job.DRK, 99, 49) == 49)
        assert(d.activeJobLevel(1, 2, xi.job.DRK, 99, 49) == 0)
    end)
end)

describe('Arcane Circle Crest BloodWeapon', function()
    it('power and duration', function()
        assert(d.arcaneCirclePowerFromParams({ mainJobIsDRK = true }) == 15)
        assert(d.arcaneCirclePowerFromParams({ mainJobIsDRK = false }) == 5)
        assert(d.arcaneCirclePowerFromParams({
            mainJobIsDRK = true, arcaneCirclePotencyMod = 3,
        }) == 18)
        assert(d.arcaneCircleDurationFromParams({}) == 180)
        assert(d.arcaneCircleDurationFromParams({ arcaneCircleDurationMod = 45 }) == 225)

        assert(d.arcaneCrestDurationFromParams({}) == 180)
        assert(d.arcaneCrestDurationFromParams({ arcaneCrestJP = 12 }) == 192)
        assert(d.bloodWeaponDurationFromParams({}) == 30)
        assert(d.bloodWeaponDurationFromParams({ enhancesBloodWeaponMod = 20 }) == 50)
    end)
end)

describe('DarkSeal DiabolicEye NetherVoid Scarlet Souleater', function()
    it('merit JP products', function()
        assert(d.darkSealPowerFromParams({ darkSealMerit = 10 }) == 0)
        assert(d.darkSealPowerFromParams({ darkSealMerit = 50 }) == 40)
        assert(almost(d.darkSealSubPowerFromParams({
            darkSealMerit = 50, enhancesDarkSealMod = 10,
        }), 50))
        assert(almost(d.darkSealSubPowerFromParams({
            darkSealMerit = 35, enhancesDarkSealMod = 3,
        }), 10.5))

        assert(d.diabolicEyePowerFromParams({ diabolicEyeMerit = 0 }) == 15)
        assert(d.diabolicEyePowerFromParams({ diabolicEyeMerit = 5 }) == 40)
        assert(d.diabolicEyeDurationFromParams({
            diabolicEyeMerit = 5, enhancesDiabolicEyeMod = 10,
        }) == 230)
        assert(d.diabolicEyeDurationFromParams({
            diabolicEyeMerit = 0, enhancesDiabolicEyeMod = 99,
        }) == 180)

        assert(d.netherVoidPowerFromParams({}) == 50)
        assert(d.netherVoidPowerFromParams({
            enhancesNetherVoidMod = 15, netherVoidJP = 10,
        }) == 85)

        assert(d.scarletDeliriumDurationFromParams({}) == 90)
        assert(d.scarletDeliriumDurationFromParams({ scarletDeliriumJP = 20 }) == 110)

        assert(d.souleaterDurationFromParams({}) == 60)
        assert(d.souleaterDurationFromParams({ souleaterJP = 15 }) == 75)
        assert(almost(d.souleaterSubPowerFromParams({
            enhancesMutedSoulMod = 3, mutedSoulMerit = 50,
        }), 15))
        assert(almost(d.souleaterSubPowerFromParams({
            enhancesMutedSoulMod = 3, mutedSoulMerit = 25,
        }), 7.5))
    end)
end)

describe('weaponBashDamage and animation', function()
    it('damage formula and animation map', function()
        assert(d.weaponBashDamageFromParams({ darkKnightLvl = 99 }) == 27)
        assert(d.weaponBashDamageFromParams({ darkKnightLvl = 1 }) == 3)
        assert(d.weaponBashDamageFromParams({
            darkKnightLvl = 75, weaponBashMod = 10, weaponBashJP = 2,
        }) == 51)
        assert(d.weaponBashDamageFromParams({ darkKnightLvl = 49 }) == 15)

        assert(d.weaponBashAnimation(xi.skill.GREAT_SWORD) == 201)
        assert(d.weaponBashAnimation(xi.skill.GREAT_KATANA) == 201)
        assert(d.weaponBashAnimation(xi.skill.GREAT_AXE) == 202)
        assert(d.weaponBashAnimation(xi.skill.SCYTHE) == 202)
        assert(d.weaponBashAnimation(xi.skill.STAFF) == 202)
        assert(d.weaponBashAnimation(xi.skill.POLEARM) == 203)
        assert(d.weaponBashAnimation(1) == 0)
    end)
end)
