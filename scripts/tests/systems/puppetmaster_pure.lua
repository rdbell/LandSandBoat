-----------------------------------
-- Pure system tests for Puppetmaster dual-wire (slice 6747).
-- Calls production xi.job_utils.puppetmaster pure exports.
-- Goldens match internal/puppetmaster (0904).
-----------------------------------

require('scripts/globals/job_utils/puppetmaster')

local p = xi.job_utils.puppetmaster

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Puppetmaster pure pins', function()
    it('bases and oil catalog', function()
        assert(p.overdriveBaseDuration == 180)
        assert(p.repairRegenTick == 3)
        assert(p.roleReversalMeritBase == 5 and p.ventriloquyMeritBase == 5)
        assert(p.oneHourRecastSecondsPerMod == 60 and p.minSpawnHP == 1)
        assert(p.msgRequiresAPet == 215 and p.msgNoEffectOnPet == 336)
        assert(p.msgAlreadyHasAPet == 315 and p.msgUnableToUseJA == 87)

        local oil = p.oilData[xi.item.CAN_OF_AUTOMATON_OIL]
        assert(oil and almost(oil.initialHealPercent, 0.1) and oil.statusesRemoved == 1)
        assert(oil.regen == 20 and oil.duration == 15)
        oil = p.oilData[xi.item.CAN_OF_AUTOMATON_OIL_P3]
        assert(oil and almost(oil.initialHealPercent, 0.4) and oil.statusesRemoved == 4)
        assert(oil.regen == 80 and oil.duration == 60)
    end)
end)

describe('checks and repair overdrive', function()
    it('gates repair products', function()
        local msg, ok = p.checkAutomatonPetFromParams({ hasPet = false, isAutomaton = false })
        assert(not ok and msg == 215)
        msg, ok = p.checkAutomatonPetFromParams({ hasPet = true, isAutomaton = false })
        assert(not ok and msg == 336)
        msg, ok = p.checkAutomatonPetFromParams({ hasPet = true, isAutomaton = true })
        assert(ok and msg == 0)

        msg, ok = p.checkNoPetFromParams({ hasPet = true })
        assert(not ok and msg == 315)
        msg, ok = p.checkNoPetFromParams({ hasPet = false })
        assert(ok and msg == 0)

        msg, ok = p.checkRepairOilFromParams(xi.item.CAN_OF_AUTOMATON_OIL_P2)
        assert(ok and msg == 0)
        msg, ok = p.checkRepairOilFromParams(12345)
        assert(not ok and msg == 87)

        assert(almost(p.repairInitialHealFromParams({
            petMaxHP = 1000, initialHealPercent = 0.1, repairMerit = 0,
        }), 100))
        assert(almost(p.repairInitialHealFromParams({
            petMaxHP = 1000, initialHealPercent = 0.1, repairMerit = 10,
        }), 110))

        assert(almost(p.repairRegenAmountFromParams({
            baseRegen = 20, repairMerit = 0, repairPotencyMod = 0,
        }), 20))
        assert(almost(p.repairRegenAmountFromParams({
            baseRegen = 20, repairMerit = 10, repairPotencyMod = 20,
        }), 26))

        local heal, regen, dur, rok = p.repairProductsFromParams({
            oilItemId = xi.item.CAN_OF_AUTOMATON_OIL,
            petMaxHP = 1000, repairMerit = 10, repairPotencyMod = 20,
        })
        assert(rok and almost(heal, 110) and almost(regen, 26) and dur == 15)

        heal, regen, dur, rok = p.repairProductsFromParams({
            oilItemId = xi.item.CAN_OF_AUTOMATON_OIL_P3,
            petMaxHP = 2500, repairMerit = 0, repairPotencyMod = 0,
        })
        assert(rok and almost(heal, 1000) and almost(regen, 80) and dur == 60)

        _, _, _, rok = p.repairProductsFromParams({ oilItemId = 1, petMaxHP = 1000 })
        assert(not rok)

        assert(p.overdriveDurationFromParams(0) == 180)
        assert(p.overdriveDurationFromParams(30) == 210)
        assert(p.oneHourRecastFromParams({ abilityRecast = 3600, oneHourRecastMod = 15 }) == 2700)
        assert(p.oneHourRecastFromParams({ abilityRecast = 30, oneHourRecastMod = 1 }) == 0)
    end)
end)

describe('deus role reversal ventriloquy', function()
    it('spawn swap enmity', function()
        assert(almost(p.deusExAutomataHPPercentFromParams(75), 0.25))
        assert(almost(p.deusExAutomataHPPercentFromParams(99), 0.33))
        assert(almost(p.deusExAutomataHPPercentFromParams(2), 0))
        assert(almost(p.deusExAutomataHPPercentFromParams(3), 0.01))

        assert(almost(p.deusExAutomataSpawnHPFromParams({ maxHP = 1000, percent = 0.25 }), 250))
        assert(almost(p.deusExAutomataSpawnHPFromParams({ maxHP = 10, percent = 0 }), 1))
        assert(almost(p.deusExAutomataSpawnMPFromParams({ maxMP = 1000, percent = 0.25 }), 250))
        assert(almost(p.deusExAutomataSpawnMPFromParams({ maxMP = 100, percent = 0 }), 0))

        assert(almost(p.roleReversalHPFromParams({ sourceHP = 500, roleReversalMerit = 5 }), 500))
        assert(almost(p.roleReversalHPFromParams({ sourceHP = 500, roleReversalMerit = 15 }), 550))
        assert(almost(p.roleReversalHPFromParams({ sourceHP = 1000, roleReversalMerit = 0 }), 950))
        assert(almost(p.roleReversalHPFromParams({ sourceHP = 0, roleReversalMerit = 5 }), 1))

        assert(almost(p.ventriloquyMeritBonusFromParams(5), 0))
        assert(almost(p.ventriloquyMeritBonusFromParams(15), 0.10))

        local pb, petb = p.ventriloquyEnmityBonusesFromParams({
            ventriloquyMerit = 15,
            targetFocusIsPlayer = true,
            targetFocusIsPet = false,
            playerTotal = 100, petTotal = 200,
        })
        assert(almost(pb, 1.10) and almost(petb, 0.90))

        pb, petb = p.ventriloquyEnmityBonusesFromParams({
            ventriloquyMerit = 15,
            targetFocusIsPlayer = false,
            targetFocusIsPet = true,
            playerTotal = 100, petTotal = 200,
        })
        assert(almost(pb, 0.90) and almost(petb, 1.10))

        local nPCE, nPVE, nPetCE, nPetVE = p.ventriloquySwappedEnmityFromParams({
            playerCE = 100, playerVE = 200, petCE = 300, petVE = 400,
            playerBonus = 1.1, petBonus = 0.9,
        })
        assert(almost(nPCE, 270) and almost(nPVE, 360))
        assert(almost(nPetCE, 110) and almost(nPetVE, 220))
    end)
end)
