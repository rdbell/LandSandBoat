-----------------------------------
-- Pure system tests for pDIF level-correction dual-wire (slice 6757).
-- Calls production xi.combat.physical pure exports.
-- Goldens match internal/pdif.LevelDifFactor / ClampRangedCRatio /
-- RangedCRatioCaps / ApplyLevelDifToCaps (and internal/wsformula twins).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local p = xi.combat.physical

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('pDIF levelDif pins', function()
    it('slopes and ranged clamp bounds', function()
        assert(almost(p.meleeLevelCorrectionPerLevel, 3 / 64))
        assert(almost(p.rangedLevelCorrectionPerLevel, 3 / 128))
        assert(almost(p.rangedLevelCorrectionPerLevel * 2, p.meleeLevelCorrectionPerLevel))
        assert(p.rangedCRatioMin == 0 and p.rangedCRatioMax == 10)
    end)
end)

describe('levelDifFactorFromParams', function()
    it('returns zero when correction disabled', function()
        assert(p.levelDifFactorFromParams({
            actorLevel = 99, targetLevel = 1, applyLevelCorrection = false,
            actorIsPC = false, ranged = false,
        }) == 0)
    end)

    it('melee: PC clamps positive, keeps negative', function()
        assert(p.levelDifFactorFromParams({
            actorLevel = 60, targetLevel = 50, applyLevelCorrection = true,
            actorIsPC = true, ranged = false,
        }) == 0)
        local want = -10 * p.meleeLevelCorrectionPerLevel
        assert(almost(p.levelDifFactorFromParams({
            actorLevel = 50, targetLevel = 60, applyLevelCorrection = true,
            actorIsPC = true, ranged = false,
        }), want))
    end)

    it('melee: non-PC clamps negative, keeps positive', function()
        local want = 10 * p.meleeLevelCorrectionPerLevel
        assert(almost(p.levelDifFactorFromParams({
            actorLevel = 60, targetLevel = 50, applyLevelCorrection = true,
            actorIsPC = false, ranged = false,
        }), want))
        assert(p.levelDifFactorFromParams({
            actorLevel = 50, targetLevel = 60, applyLevelCorrection = true,
            actorIsPC = false, ranged = false,
        }) == 0)
    end)

    it('ranged uses half slope', function()
        local want = -10 * p.rangedLevelCorrectionPerLevel
        assert(almost(p.levelDifFactorFromParams({
            actorLevel = 50, targetLevel = 60, applyLevelCorrection = true,
            actorIsPC = true, ranged = true,
        }), want))
    end)
end)

describe('clampRangedCRatio and caps', function()
    it('clamps base ratio to [0, 10]', function()
        assert(p.clampRangedCRatioFromParams({ baseRatio = -1 }) == 0)
        assert(p.clampRangedCRatioFromParams({ baseRatio = 0 }) == 0)
        assert(p.clampRangedCRatioFromParams({ baseRatio = 5 }) == 5)
        assert(p.clampRangedCRatioFromParams({ baseRatio = 10 }) == 10)
        assert(p.clampRangedCRatioFromParams({ baseRatio = 11 }) == 10)
    end)

    it('ranged cap bands', function()
        local lo, hi = p.rangedCRatioCapsFromParams({ cRatio = 0.45, pDifFinalCap = 3 })
        assert(almost(lo, 0.45) and almost(hi, 0.45 * 10 / 9))

        lo, hi = p.rangedCRatioCapsFromParams({ cRatio = 1.0, pDifFinalCap = 3 })
        assert(lo == 1 and hi == 1)

        lo, hi = p.rangedCRatioCapsFromParams({ cRatio = 2.0, pDifFinalCap = 3 })
        assert(almost(lo, math.min(2 * 20 / 19 - 3 / 19, 3)))
        assert(almost(hi, math.min(2, 3)))
    end)

    it('applyLevelDif adds factor to both caps', function()
        local lo, hi = p.applyLevelDifToCapsFromParams({
            lowerCap = 1.0, upperCap = 2.0, levelDifFactor = -0.1,
        })
        assert(almost(lo, 0.9) and almost(hi, 1.9))
    end)
end)
