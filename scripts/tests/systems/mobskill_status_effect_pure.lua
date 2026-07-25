-----------------------------------
-- Pure system tests for mobskill status-effect dual-wire helpers (slice 6703).
-- Calls production xi.combat.action pure exports.
-- Goldens match internal/mobskilleffect (0939 / 6106).
-----------------------------------

require('scripts/globals/combat/action_mobskill_status_effect')

local act = xi.combat.action
local step = act.mobskillStatusStep

describe('mobskill status step and defaults', function()
    it('pins step ordinals and default rank/stat/messages', function()
        assert(step.CANT_GAIN == 1)
        assert(step.IMMUNE_CHECK == 2)
        assert(step.RESIST_TRAIT_CHECK == 3)
        assert(step.NULLIFY_CHECK == 4)
        assert(step.RESIST_RATE_CHECK == 5)
        assert(step.APPLICATION_FAIL == 6)
        assert(step.APPLICATION_SUCCESS == 7)
        assert(act.mobskillStatusDefaultRank == xi.skillRank.A_PLUS)
        assert(act.mobskillStatusDefaultStat == xi.mod.INT)
        assert(xi.msg.basic.SKILL_MISS == 188)
        assert(xi.msg.basic.SKILL_NO_EFFECT == 189)
        assert(xi.msg.basic.SKILL_ENFEEB_IS == 242)
    end)
end)

describe('validateMobskillEffectParams and MessageParams', function()
    it('applies empty and explicit defaults', function()
        local p = act.validateMobskillEffectParams({}, 5)
        assert(p.effectId == 0 and p.power == 0 and p.duration == 0)
        assert(p.element == 5) -- associated element inject
        assert(p.rank == xi.skillRank.A_PLUS)
        assert(p.stat == xi.mod.INT)

        p = act.validateMobskillEffectParams({
            effectId = 3, power = 10, duration = 60, element = 2, rank = 0, stat = 0, macc = 5,
        }, 99)
        assert(p.effectId == 3 and p.power == 10 and p.duration == 60)
        assert(p.element == 2) -- explicit element wins
        assert(p.rank == 0 and p.stat == 0 and p.macc == 5)

        local m = act.validateMobskillMessageParams({})
        assert(m.messageBypass == false)
        assert(m.messageCantGain == 189)
        assert(m.messageIsImmune == 188)
        assert(m.messageIsSuccessful == 242)

        m = act.validateMobskillMessageParams({ messageBypass = true, messageIsSuccessful = 1 })
        assert(m.messageBypass == true and m.messageIsSuccessful == 1)
    end)
end)

describe('selectMobskillStatusStep and duration', function()
    it('walks gate ladder in LSB order', function()
        assert(act.selectMobskillStatusStep({ canGain = false }) == step.CANT_GAIN)
        assert(act.selectMobskillStatusStep({
            canGain = true, isImmune = true,
        }) == step.IMMUNE_CHECK)
        assert(act.selectMobskillStatusStep({
            canGain = true, isTraitResisted = true,
        }) == step.RESIST_TRAIT_CHECK)
        assert(act.selectMobskillStatusStep({
            canGain = true, isNullified = true,
        }) == step.NULLIFY_CHECK)
        assert(act.selectMobskillStatusStep({
            canGain = true, resistSuccess = false,
        }) == step.RESIST_RATE_CHECK)
        assert(act.selectMobskillStatusStep({
            canGain = true, resistSuccess = true, addSucceeded = true,
        }) == step.APPLICATION_SUCCESS)
        assert(act.selectMobskillStatusStep({
            canGain = true, resistSuccess = true, addSucceeded = false,
        }) == step.APPLICATION_FAIL)

        assert(act.durationAfterResist(120, 0.5) == 60)
        assert(act.durationAfterResist(100, 0.333) == 33)
    end)
end)

describe('messageForMobskillStatusStep and selectBest', function()
    it('maps steps to messages and picks highest step', function()
        local m = act.validateMobskillMessageParams({})
        local msg, set = act.messageForMobskillStatusStep(step.CANT_GAIN, m)
        assert(set and msg == 189)
        msg, set = act.messageForMobskillStatusStep(step.APPLICATION_SUCCESS, m)
        assert(set and msg == 242)
        msg, set = act.messageForMobskillStatusStep(step.APPLICATION_SUCCESS, {
            messageBypass = true, messageIsSuccessful = 242,
        })
        assert(not set and msg == 0)

        local effectId, best, idx = act.selectBestMobskillStatusResult({})
        assert(effectId == 0 and best == 0 and idx == 0)

        effectId, best, idx = act.selectBestMobskillStatusResult({
            { 10, step.CANT_GAIN },
            { 20, step.APPLICATION_SUCCESS },
            { 30, step.IMMUNE_CHECK },
        })
        assert(effectId == 20 and best == step.APPLICATION_SUCCESS and idx == 2)

        -- Tie keeps earliest
        effectId, best, idx = act.selectBestMobskillStatusResult({
            { 1, step.IMMUNE_CHECK },
            { 2, step.IMMUNE_CHECK },
        })
        assert(effectId == 1 and idx == 1)
    end)
end)
