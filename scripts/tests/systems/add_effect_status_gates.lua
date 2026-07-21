-----------------------------------
-- Pure system tests for executeAddEffect enhancement/enfeeble/dispel gates
-- and enfeeble duration inject (slice 6094).
-- Mirrors scripts/globals/combat/action_additional_effect_status.lua
-----------------------------------

describe('add effect status gates pure injects', function()
    local EFFECT_NONE = 255 -- xi.effect.NONE

    local function enhancementApplies(p)
        if p.hasEnspell then
            return false
        end
        if p.effectId == EFFECT_NONE then
            return false
        end
        if p.procMiss then
            return false
        end
        if p.nullified then
            return false
        end
        return p.addStatusOK == true
    end

    local function enfeeblementApplies(p)
        if p.hasEnspell then
            return false
        end
        if p.effectId == EFFECT_NONE then
            return false
        end
        if p.procMiss then
            return false
        end
        if p.immune then
            return false
        end
        if p.traitResisted then
            return false
        end
        if p.nullified then
            return false
        end
        if p.resistRateFail then
            return false
        end
        return p.addStatusOK == true
    end

    local function dispelApplies(p)
        if p.hasEnspell then
            return false
        end
        if p.effectId ~= EFFECT_NONE then
            return false
        end
        if p.procMiss then
            return false
        end
        if not p.hasDispelable then
            return false
        end
        if p.resistRateFail then
            return false
        end
        return true
    end

    local function enfeebleDuration(baseDuration, resistanceRate)
        return math.floor(baseDuration * resistanceRate)
    end

    it('enhancement gate order', function()
        assert(not enhancementApplies({ effectId = 1, hasEnspell = true, addStatusOK = true }))
        assert(not enhancementApplies({ effectId = EFFECT_NONE, addStatusOK = true }))
        assert(not enhancementApplies({ effectId = 1, procMiss = true, addStatusOK = true }))
        assert(not enhancementApplies({ effectId = 1, nullified = true, addStatusOK = true }))
        assert(not enhancementApplies({ effectId = 1, addStatusOK = false }))
        assert(enhancementApplies({ effectId = 1, addStatusOK = true }))
    end)

    it('enfeeblement gate order', function()
        assert(not enfeeblementApplies({ effectId = 2, immune = true, addStatusOK = true }))
        assert(not enfeeblementApplies({ effectId = 2, traitResisted = true, addStatusOK = true }))
        assert(not enfeeblementApplies({ effectId = 2, resistRateFail = true, addStatusOK = true }))
        assert(enfeeblementApplies({ effectId = 2, addStatusOK = true }))
    end)

    it('dispel gate order', function()
        assert(not dispelApplies({ effectId = EFFECT_NONE, hasEnspell = true, hasDispelable = true }))
        assert(not dispelApplies({ effectId = 5, hasDispelable = true })) -- effectId must be NONE
        assert(not dispelApplies({ effectId = EFFECT_NONE, procMiss = true, hasDispelable = true }))
        assert(not dispelApplies({ effectId = EFFECT_NONE, hasDispelable = false }))
        assert(not dispelApplies({ effectId = EFFECT_NONE, hasDispelable = true, resistRateFail = true }))
        assert(dispelApplies({ effectId = EFFECT_NONE, hasDispelable = true }))
    end)

    it('enfeeble duration floors resistance product', function()
        assert(enfeebleDuration(60, 1) == 60)
        assert(enfeebleDuration(60, 0.5) == 30)
        assert(enfeebleDuration(60, 0.125) == 7)
        assert(enfeebleDuration(100, 0.33) == 33)
    end)
end)
