-----------------------------------
-- Pure system tests for executeAddEffect* status full plans (slice 6105).
-- Source: scripts/globals/combat/action_additional_effect_status.lua ~104–227
-----------------------------------

describe('executeAddEffect status pure plans', function()
    local EFFECT_NONE = 255
    local EFFECT_POISON = 3
    local EFFECT_SLOW = 13
    local EFFECT_HASTE = 33

    local function enhancementApplies(p)
        if p.hasEnspell or p.effectId == EFFECT_NONE or p.procMiss or p.nullified then
            return false
        end
        return p.addStatusOK == true
    end

    local function enfeeblementApplies(p)
        if p.hasEnspell or p.effectId == EFFECT_NONE or p.procMiss then
            return false
        end
        if p.immune or p.traitResisted or p.nullified or p.resistRateFail then
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
        if p.procMiss or not p.hasDispelable or p.resistRateFail then
            return false
        end
        return true
    end

    local function enfeebleDuration(base, rate)
        return math.floor(base * rate)
    end

    local function executeEnhancement(p)
        if not enhancementApplies(p) then
            return { proc = false }
        end
        return {
            proc = true,
            animation = p.animation or 0,
            message = p.message or 0,
            effectId = p.effectId,
            applyDuration = p.duration or 0,
            applyPower = p.power or 0,
        }
    end

    local function executeEnfeeblement(p)
        if not enfeeblementApplies(p) then
            return { proc = false }
        end
        return {
            proc = true,
            animation = p.animation or 0,
            message = p.message or 0,
            effectId = p.effectId,
            applyDuration = enfeebleDuration(p.baseDuration or 0, p.resistanceRate or 1),
            applyPower = p.power or 0,
        }
    end

    local function executeDispel(p)
        if not dispelApplies(p) then
            return { proc = false }
        end
        if (p.dispelledEffect or 0) == 0 then
            return { proc = false, steal = p.absorbEffect == true, dispel = not p.absorbEffect }
        end
        return {
            proc = true,
            animation = p.animation or 0,
            message = p.message or 0,
            effectId = p.dispelledEffect,
            steal = p.absorbEffect == true,
            dispel = not p.absorbEffect,
        }
    end

    it('enhancement enspell no-proc', function()
        local r = executeEnhancement({ hasEnspell = true, effectId = EFFECT_POISON, addStatusOK = true })
        assert(r.proc == false)
    end)

    it('enhancement success triple', function()
        local r = executeEnhancement({
            effectId = EFFECT_POISON, addStatusOK = true,
            animation = 10, message = 20, power = 5, duration = 120,
        })
        assert(r.proc == true and r.effectId == EFFECT_POISON and r.applyDuration == 120)
    end)

    it('enfeeblement duration floors resistance', function()
        local r = executeEnfeeblement({
            effectId = EFFECT_SLOW, addStatusOK = true,
            baseDuration = 120, resistanceRate = 0.5,
        })
        assert(r.proc == true and r.applyDuration == 60)
    end)

    it('enfeeblement immune no-proc', function()
        local r = executeEnfeeblement({
            effectId = EFFECT_SLOW, addStatusOK = true, immune = true,
            baseDuration = 100, resistanceRate = 1,
        })
        assert(r.proc == false)
    end)

    it('dispel requires NONE effectId', function()
        local r = executeDispel({
            effectId = EFFECT_POISON, hasDispelable = true, dispelledEffect = 5,
        })
        assert(r.proc == false)
    end)

    it('dispel zero result no-proc', function()
        local r = executeDispel({
            effectId = EFFECT_NONE, hasDispelable = true, dispelledEffect = 0,
        })
        assert(r.proc == false and r.dispel == true)
    end)

    it('dispel steal success', function()
        local r = executeDispel({
            effectId = EFFECT_NONE, hasDispelable = true,
            absorbEffect = true, dispelledEffect = EFFECT_HASTE,
            animation = 8, message = 99,
        })
        assert(r.proc == true and r.effectId == EFFECT_HASTE and r.steal == true)
    end)
end)
