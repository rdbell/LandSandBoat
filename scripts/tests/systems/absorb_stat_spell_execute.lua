-----------------------------------
-- Pure system tests for doAbsorbStatSpell full plan (slice 6117).
-- Source: scripts/globals/spells/absorb_spell.lua ~26–72
-----------------------------------

describe('doAbsorbStatSpell pure plan', function()
    local EFFECT_STR_BOOST = 80
    local EFFECT_STR_DOWN = 136
    local EFFECT_ACCURACY_BOOST = 90
    local EFFECT_ACCURACY_DOWN = 146

    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_RESIST = 85
    local MSG_MAGIC_ABSORB_STR = 329
    local MSG_MAGIC_ABSORB_ACC = 533

    local function use(p)
        local downEffect = p.downEffect or 0
        local boostEffect = p.boostEffect or 0
        local absorbMsg = p.absorbMsg or 0
        local resist = p.resist or 1
        local potency = p.potency or 0
        local duration = p.duration or 0

        if resist < 0.5 then
            return {
                returnEffect = 0,
                setMsg = true,
                msg = MSG_MAGIC_RESIST,
                applyEnfeeble = false,
                applyBoost = false,
            }
        end

        if not p.addStatusOK then
            return {
                returnEffect = downEffect,
                setMsg = true,
                msg = MSG_MAGIC_NO_EFFECT,
                applyEnfeeble = false,
                applyBoost = false,
                power = potency,
                duration = duration,
            }
        end

        return {
            returnEffect = downEffect,
            setMsg = true,
            msg = absorbMsg,
            applyEnfeeble = true,
            applyBoost = true,
            delBoost = true, -- force-overwrite associated buff
            boostEffect = boostEffect,
            power = potency,
            duration = duration,
        }
    end

    it('resist below half sets MAGIC_RESIST and returns 0', function()
        local r = use({
            downEffect = EFFECT_STR_DOWN, boostEffect = EFFECT_STR_BOOST,
            absorbMsg = MSG_MAGIC_ABSORB_STR, resist = 0.25, potency = 10, duration = 180,
        })
        assert(r.returnEffect == 0 and r.msg == MSG_MAGIC_RESIST)
        assert(r.applyEnfeeble ~= true and r.applyBoost ~= true)
    end)

    it('resist exactly 0.5 continues (not below half)', function()
        local r = use({
            downEffect = EFFECT_STR_DOWN, boostEffect = EFFECT_STR_BOOST,
            absorbMsg = MSG_MAGIC_ABSORB_STR, resist = 0.5, potency = 12, duration = 200,
            addStatusOK = true,
        })
        assert(r.returnEffect == EFFECT_STR_DOWN and r.msg == MSG_MAGIC_ABSORB_STR)
        assert(r.applyEnfeeble == true and r.applyBoost == true and r.delBoost == true)
        assert(r.power == 12 and r.duration == 200)
    end)

    it('addStatus fail sets MAGIC_NO_EFFECT', function()
        local r = use({
            downEffect = EFFECT_STR_DOWN, boostEffect = EFFECT_STR_BOOST,
            absorbMsg = MSG_MAGIC_ABSORB_STR, resist = 1, potency = 15, duration = 180,
            addStatusOK = false,
        })
        assert(r.msg == MSG_MAGIC_NO_EFFECT and r.applyEnfeeble ~= true)
        assert(r.returnEffect == EFFECT_STR_DOWN)
        assert(r.power == 15)
    end)

    it('success applies enfeeble and boost with absorb msg', function()
        local r = use({
            downEffect = EFFECT_ACCURACY_DOWN, boostEffect = EFFECT_ACCURACY_BOOST,
            absorbMsg = MSG_MAGIC_ABSORB_ACC, resist = 1, potency = 20, duration = 240,
            addStatusOK = true,
        })
        assert(r.returnEffect == EFFECT_ACCURACY_DOWN)
        assert(r.msg == MSG_MAGIC_ABSORB_ACC)
        assert(r.applyEnfeeble == true and r.applyBoost == true and r.delBoost == true)
        assert(r.boostEffect == EFFECT_ACCURACY_BOOST)
        assert(r.power == 20 and r.duration == 240)
    end)

    it('full resist 0 returns resist msg', function()
        local r = use({
            downEffect = EFFECT_STR_DOWN, resist = 0, addStatusOK = true, potency = 99,
        })
        assert(r.returnEffect == 0 and r.msg == MSG_MAGIC_RESIST)
    end)
end)
