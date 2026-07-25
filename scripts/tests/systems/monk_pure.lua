-----------------------------------
-- Pure system tests for Monk dual-wire (slice 6733).
-- Calls production xi.job_utils.monk pure exports.
-- Goldens match internal/monk (0896).
-----------------------------------

require('scripts/globals/job_utils/monk')

local m = xi.job_utils.monk

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Monk pure pins', function()
    it('bases and fixed powers', function()
        assert(almost(m.boostBasePower, 12.5))
        assert(almost(m.boostEffectScale, 0.10))
        assert(m.boostDuration == 180)
        assert(m.chakraVITScale == 2)
        assert(almost(m.chakraLevelHPFactor, 0.002))
        assert(m.invigorateRegenPower == 10 and m.invigorateRegenTier == 1)
        assert(m.chiBlastPenancePower == 25)
        assert(m.counterstanceBasePower == 45 and m.counterstanceDuration == 300)
        assert(m.dodgeDuration == 30 and m.focusDuration == 30)
        assert(m.footworkBasePower == 20 and m.footworkBaseSubPower == 25)
        assert(m.footworkDuration == 60)
        assert(m.formlessStrikesPower == 1 and m.formlessStrikesDuration == 180)
        assert(m.hundredFistsPower == 1 and m.hundredFistsDuration == 45)
        assert(m.impetusPower == 0 and m.impetusDuration == 180)
        assert(m.innerStrengthPower == 2 and m.innerStrengthDuration == 30)
        assert(m.mantraDuration == 180)
        assert(m.perfectCounterPower == 2 and m.perfectCounterDuration == 30)
        assert(m.chakraBitParalysis == 1 and m.chakraBitDisease == 2 and m.chakraBitPlague == 4)
    end)
end)

describe('boostPower and stack', function()
    it('12.5 + 0.10*mod', function()
        assert(almost(m.boostPowerFromParams({}), 12.5))
        assert(almost(m.boostPowerFromParams({ boostEffectMod = 10 }), 13.5))
        assert(almost(m.boostPowerFromParams({ boostEffectMod = 25 }), 15.0))
        assert(almost(m.boostStackedPower(12.5, 12.5), 25.0))
    end)
end)

describe('chakra recovery products', function()
    it('max recovery and missing-HP clamp', function()
        -- monk 99, VIT 100, maxHP 2000 → hpMod 400; (200+400)=600
        assert(almost(m.chakraMaxRecoveryFromParams({
            monkLevel = 99, vit = 100, maxHP = 2000,
        }), 600))
        -- mult 50 → 900
        assert(almost(m.chakraMaxRecoveryFromParams({
            monkLevel = 99, vit = 100, maxHP = 2000, chakraMultMod = 50,
        }), 900))
        -- jp 30 → 630
        assert(almost(m.chakraMaxRecoveryFromParams({
            monkLevel = 99, vit = 100, maxHP = 2000, chakraJP = 30,
        }), 630))
        -- level 0 → 204
        assert(almost(m.chakraMaxRecoveryFromParams({
            monkLevel = 0, vit = 100, maxHP = 2000,
        }), 204))

        assert(almost(m.chakraRecoveryFromParams({
            currentHP = 1900, maxHP = 2000, maxRecovery = 600,
        }), 100))
        assert(almost(m.chakraRecoveryFromParams({
            currentHP = 1200, maxHP = 2000, maxRecovery = 600,
        }), 600))
        assert(almost(m.chakraRecoveryFromParams({
            currentHP = 2000, maxHP = 2000, maxRecovery = 600,
        }), 0))
    end)
end)

describe('evaluateChakraClears', function()
    it('always poison/blind; optional bits', function()
        local c0 = m.evaluateChakraClears(0)
        assert(c0.poison and c0.blindness)
        assert(not c0.paralysis and not c0.disease and not c0.plague)

        local cAll = m.evaluateChakraClears(1 + 2 + 4)
        assert(cAll.paralysis and cAll.disease and cAll.plague)

        local cPara = m.evaluateChakraClears(1)
        assert(cPara.paralysis and not cPara.disease and not cPara.plague)

        local cPlague = m.evaluateChakraClears(4)
        assert(not cPlague.paralysis and not cPlague.disease and cPlague.plague)
    end)
end)

describe('invigorate penance chiBlast', function()
    it('gates multiplier damage', function()
        assert(not m.invigorateApplies(0) and m.invigorateApplies(20))
        assert(m.invigorateDuration(40) == 40)
        assert(not m.penanceApplies(0) and m.penanceApplies(20))
        assert(m.penanceDuration(100) == 100)

        assert(almost(m.chiBlastBoostMultiplierFromParams({ hasBoost = false }), 1.0))
        assert(almost(m.chiBlastBoostMultiplierFromParams({
            hasBoost = true, boostPower = 12.5,
        }), 0.5))
        assert(almost(m.chiBlastBoostMultiplierFromParams({
            hasBoost = true, boostPower = 25,
        }), 1.0))

        assert(almost(m.chiBlastDamageFromParams({
            mnd = 100, rollUnit = 0, multiplier = 1,
        }), 50))
        assert(almost(m.chiBlastDamageFromParams({
            mnd = 100, rollUnit = 1, multiplier = 1,
        }), 100))
        assert(almost(m.chiBlastDamageFromParams({
            mnd = 100, rollUnit = 0, multiplier = 0.5,
        }), 25))
        assert(almost(m.chiBlastDamageFromParams({
            mnd = 99, rollUnit = 0, multiplier = 1,
        }), 49))
    end)
end)

describe('stance powers oneHour activeJobLevel', function()
    it('products and helpers', function()
        assert(m.counterstancePowerFromParams({}) == 45)
        assert(m.counterstancePowerFromParams({ counterstanceEffectMod = 10 }) == 55)
        assert(m.dodgePowerFromParams({ dodgeEffectMod = 20, dodgeJP = 5 }) == 25)
        assert(m.focusPowerFromParams({ focusEffectMod = 10, focusJP = 3 }) == 13)
        assert(m.footworkPowerFromParams({ weaponDmg = 50 }) == 70)
        assert(m.footworkSubPowerFromParams({ footworkAttBonusMod = 13 }) == 38)
        assert(m.impetusSubPowerFromParams({ augmentsImpetusMod = 7 }) == 7)
        assert(m.mantraPowerFromParams({ mantraMerit = 15 }) == 15)

        assert(m.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 10 }) == 3000)
        assert(m.oneHourRecastFromParams({ abilityRecast = 100, oneHourRecastMod = 10 }) == 0)
        assert(m.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 0 }) == 3600)

        assert(m.activeJobLevel(xi.job.MNK, 1, xi.job.MNK, 99, 49) == 99)
        assert(m.activeJobLevel(1, xi.job.MNK, xi.job.MNK, 99, 49) == 49)
        assert(m.activeJobLevel(1, 5, xi.job.MNK, 99, 49) == 0)
    end)
end)
