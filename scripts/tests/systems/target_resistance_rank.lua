-----------------------------------
-- Pure system tests for calculateTargetResistanceRank injects (slice 6085).
-----------------------------------

describe('target resistance rank pure injects', function()
    local function resistanceRankModID(effectId, statusAssociated, elemental)
        local mod = 0
        if effectId > 0 then
            mod = statusAssociated
        end
        if mod == 0 then
            mod = elemental
        end
        return mod
    end

    local function targetResistanceRank(p)
        if p.targetIsPC then
            return 0
        end
        local rank = p.baseRank
        if p.effectId and p.effectId > 0 then
            rank = rank - (p.immunobreakMod or 0)
        end
        return utils.clamp(rank, -3, 11)
    end

    it('resistance rank mod ID selection', function()
        assert(resistanceRankModID(0, 999, 192) == 192)
        assert(resistanceRankModID(2, 300, 192) == 300)
        assert(resistanceRankModID(2, 0, 192) == 192)
    end)

    it('target resistance rank PC and immunobreak', function()
        assert(targetResistanceRank({
            targetIsPC = true, baseRank = 5, effectId = 2, immunobreakMod = 1,
        }) == 0)
        assert(targetResistanceRank({
            baseRank = 3, effectId = 0, immunobreakMod = 10,
        }) == 3)
        assert(targetResistanceRank({
            baseRank = 5, effectId = 2, immunobreakMod = 2,
        }) == 3)
        assert(targetResistanceRank({ baseRank = 20 }) == 11)
        assert(targetResistanceRank({
            baseRank = -2, effectId = 1, immunobreakMod = 5,
        }) == -3)
    end)
end)
