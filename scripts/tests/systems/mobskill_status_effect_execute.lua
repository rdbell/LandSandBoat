-----------------------------------
-- Pure system tests for executeMobskillStatusEffect full plan (slice 6106).
-- Source: scripts/globals/combat/action_mobskill_status_effect.lua ~139–169
-----------------------------------

describe('executeMobskillStatusEffect pure plan', function()
    local STEP_CANT_GAIN = 1
    local STEP_IMMUNE = 2
    local STEP_SUCCESS = 7

    local MSG_SKILL_MISS = 188
    local MSG_SKILL_NO_EFFECT = 189
    local MSG_SKILL_ENFEEB_IS = 242

    local function selectStep(c)
        if not c.canGain then
            return STEP_CANT_GAIN
        end
        if c.isImmune then
            return STEP_IMMUNE
        end
        if c.isTraitResisted then
            return 3
        end
        if c.isNullified then
            return 4
        end
        if not c.resistSuccess then
            return 5
        end
        if c.addSucceeded then
            return STEP_SUCCESS
        end
        return 6
    end

    local function durationAfterResist(duration, rate)
        return math.floor(duration * rate)
    end

    local function selectBest(results)
        if #results == 0 then
            return nil, -1
        end
        local best = results[1]
        local idx = 1
        for i = 2, #results do
            if results[i].result > best.result then
                best = results[i]
                idx = i
            end
        end
        return best, idx
    end

    local function messageForStep(step, bypass)
        if bypass then
            return 0, false
        end
        if step == STEP_CANT_GAIN then
            return MSG_SKILL_NO_EFFECT, true
        end
        if step == STEP_IMMUNE then
            return MSG_SKILL_MISS, true
        end
        if step == STEP_SUCCESS then
            return MSG_SKILL_ENFEEB_IS, true
        end
        return MSG_SKILL_MISS, true
    end

    local function execute(p)
        local results = {}
        local applies = {}
        for i = 1, #p.effects do
            local e = p.effects[i]
            local step = selectStep(e.checks)
            results[i] = { effectId = e.effectId, result = step }
            if step == STEP_SUCCESS then
                table.insert(applies, {
                    effectId = e.effectId,
                    duration = durationAfterResist(e.duration or 0, e.resistanceRate or 1),
                    power = e.power or 0,
                })
            end
        end
        local best, idx = selectBest(results)
        if not best then
            return { bestIndex = -1, effectId = 0 }
        end
        local msg, set = messageForStep(best.result, p.messageBypass == true)
        return {
            effectId = best.effectId,
            bestStep = best.result,
            bestIndex = idx,
            message = msg,
            setMessage = set,
            applies = applies,
        }
    end

    it('empty effects returns no best', function()
        local r = execute({ effects = {} })
        assert(r.bestIndex == -1)
    end)

    it('single success applies resisted duration', function()
        local r = execute({
            effects = {
                {
                    effectId = 42, power = 10, duration = 120, resistanceRate = 0.5,
                    checks = { canGain = true, resistSuccess = true, addSucceeded = true },
                },
            },
        })
        assert(r.effectId == 42 and r.bestStep == STEP_SUCCESS)
        assert(r.message == MSG_SKILL_ENFEEB_IS and r.setMessage == true)
        assert(#r.applies == 1 and r.applies[1].duration == 60)
    end)

    it('higher step wins over earlier immune', function()
        local r = execute({
            effects = {
                { effectId = 1, checks = { canGain = true, isImmune = true } },
                {
                    effectId = 2, duration = 30, resistanceRate = 1,
                    checks = { canGain = true, resistSuccess = true, addSucceeded = true },
                },
            },
        })
        assert(r.effectId == 2 and r.bestIndex == 2)
    end)

    it('tie keeps earliest index', function()
        local r = execute({
            effects = {
                { effectId = 10, checks = { canGain = true, isImmune = true } },
                { effectId = 20, checks = { canGain = true, isImmune = true } },
            },
        })
        assert(r.effectId == 10 and r.bestIndex == 1)
        assert(r.message == MSG_SKILL_MISS)
    end)

    it('message bypass leaves set false', function()
        local r = execute({
            messageBypass = true,
            effects = {
                {
                    effectId = 1, duration = 10, resistanceRate = 1,
                    checks = { canGain = true, resistSuccess = true, addSucceeded = true },
                },
            },
        })
        assert(r.setMessage == false and r.effectId == 1)
    end)

    it('cant gain uses NO_EFFECT message', function()
        local r = execute({
            effects = {
                { effectId = 7, checks = { canGain = false } },
            },
        })
        assert(r.bestStep == STEP_CANT_GAIN and r.message == MSG_SKILL_NO_EFFECT)
        assert(#r.applies == 0)
    end)
end)
