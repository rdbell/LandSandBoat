-----------------------------------
-- Pure system tests for Summoner dual-wire (slice 6744).
-- Calls production xi.job_utils.summoner pure exports.
-- Goldens match internal/summoner (0903).
-----------------------------------

require('scripts/globals/job_utils/summoner')

local s = xi.job_utils.summoner

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('Summoner pure pins', function()
    it('bases', function()
        assert(s.baseMPCostMissing == 9999)
        assert(almost(s.apogeeMPCostMultiplier, 1.5))
        assert(s.bloodBoonConserveMin == 8 and s.bloodBoonConserveMax == 15)
        assert(s.bloodBoonConserveDiv == 16)
        assert(s.manaCedeMPCost == 100 and s.manaCedeBonusTPBase == 1000)
        assert(s.manaCedeJPPerLevel == 50 and s.manaCedeTPMin == 1000 and s.manaCedeTPMax == 3000)
        assert(s.soothingRubyPowerMin == 1 and s.soothingRubyPowerMax == 6)
        assert(s.baseMPCostPactCount == 100)
    end)
end)

describe('BaseMPCost BloodBoon', function()
    it('catalog astral apogee blood boon', function()
        -- catalog cost without AF/Apogee
        assert(almost(s.baseMPCostFromParams({
            catalogCost = 6, isAstralFlow = false, hasApogee = false, mainLvl = 75,
        }), 6))

        -- missing catalog
        assert(almost(s.baseMPCostFromParams({
            catalogCost = nil, isAstralFlow = false, hasApogee = false, mainLvl = 75,
        }), 9999))

        -- missing ignores Apogee (LSB returns 9999 before Apogee multiply)
        assert(almost(s.baseMPCostFromParams({
            catalogCost = nil, isAstralFlow = false, hasApogee = true, mainLvl = 75,
        }), 9999))

        -- Astral Flow: mainLvl * 2
        assert(almost(s.baseMPCostFromParams({
            isAstralFlow = true, hasApogee = false, mainLvl = 75, catalogCost = 999,
        }), 150))

        -- Apogee on catalog
        assert(almost(s.baseMPCostFromParams({
            catalogCost = 100, isAstralFlow = false, hasApogee = true, mainLvl = 75,
        }), 150))

        -- Apogee on Astral Flow
        assert(almost(s.baseMPCostFromParams({
            isAstralFlow = true, hasApogee = true, mainLvl = 50,
        }), 100 * 1.5))

        -- Blood Boon: AF skip
        assert(almost(s.bloodBoonMPCostFromParams({
            base = 100, bloodBoonRate = 100, roll1to100 = 1,
            conserveRoll8to15 = 8, isAstralFlow = true,
        }), 100))

        -- no proc (roll > rate)
        assert(almost(s.bloodBoonMPCostFromParams({
            base = 100, bloodBoonRate = 50, roll1to100 = 51,
            conserveRoll8to15 = 8, isAstralFlow = false,
        }), 100))

        -- proc: 100 * 8 / 16 = 50
        assert(almost(s.bloodBoonMPCostFromParams({
            base = 100, bloodBoonRate = 50, roll1to100 = 50,
            conserveRoll8to15 = 8, isAstralFlow = false,
        }), 50))

        -- proc: 100 * 15 / 16
        assert(almost(s.bloodBoonMPCostFromParams({
            base = 100, bloodBoonRate = 100, roll1to100 = 1,
            conserveRoll8to15 = 15, isAstralFlow = false,
        }), 100 * 15 / 16))
    end)
end)

describe('ManaCede SoothingRuby', function()
    it('tp power erase', function()
        -- avatar 0, jp 0, enh 0 → 0 + 1000*1 = 1000 (min clamp floor)
        assert(s.manaCedeTPFromParams({
            avatarTP = 0, manaCedeJP = 0, enhancesManaCede = 0,
        }) == 1000)

        -- avatar 500, jp 0, enh 0 → 500+1000 = 1500
        assert(s.manaCedeTPFromParams({
            avatarTP = 500, manaCedeJP = 0, enhancesManaCede = 0,
        }) == 1500)

        -- min clamp: avatar 0, jp 0, enh -100 → 0 + 1000*0 = 0 → min 1000
        assert(s.manaCedeTPFromParams({
            avatarTP = 0, manaCedeJP = 0, enhancesManaCede = -100,
        }) == 1000)

        -- max clamp: avatar 3000, jp 10, enh 100 → huge → 3000
        assert(s.manaCedeTPFromParams({
            avatarTP = 3000, manaCedeJP = 10, enhancesManaCede = 100,
        }) == 3000)

        -- JP 2 → bonus 1000+100=1100; avatar 0 → 1100
        assert(s.manaCedeTPFromParams({
            avatarTP = 0, manaCedeJP = 2, enhancesManaCede = 0,
        }) == 1100)

        -- Soothing Ruby power: skill 0 → floor(99/100)=0 → clamp min 1
        assert(s.soothingRubyPowerFromParams(0) == 1)
        -- skill 1 → floor(100/100)=1
        assert(s.soothingRubyPowerFromParams(1) == 1)
        -- skill 401 → floor(500/100)=5
        assert(s.soothingRubyPowerFromParams(401) == 5)
        -- skill 999 → floor(1098/100)=10 → clamp max 6
        assert(s.soothingRubyPowerFromParams(999) == 6)

        assert(s.soothingRubyEffectsErasedFromParams({ power = 3, erasableCount = 5 }) == 3)
        assert(s.soothingRubyEffectsErasedFromParams({ power = 5, erasableCount = 2 }) == 2)
        assert(s.soothingRubyEffectsErasedFromParams({ power = 0, erasableCount = 3 }) == 0)
    end)
end)
