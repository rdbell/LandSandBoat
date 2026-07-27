require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.ladybug = xi.mix.ladybug or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.ladybug.phasePlan = function(hour, phase, daySkillList, nightSkillList)
    if hour >= 18 or hour < 6 then
        if phase == 0 then
            return {
                phase = 1,
                noAggro = 1,
                roamCool = 10,
                evasionDelta = -15,
                accuracyDelta = -15,
                delay = -400,
                skillList = nightSkillList,
            }
        end
    elseif phase == 1 then
        return {
            phase = 0,
            noAggro = 0,
            roamCool = 0,
            evasionDelta = 15,
            accuracyDelta = 15,
            delay = 0,
            skillList = daySkillList,
        }
    end

    return nil
end

local function applyPlan(mob, plan)
    mob:setMobMod(xi.mobMod.NO_AGGRO, plan.noAggro)
    mob:setMobMod(xi.mobMod.ROAM_COOL, plan.roamCool)
    if plan.evasionDelta < 0 then
        mob:delMod(xi.mod.EVA, -plan.evasionDelta)
        mob:delMod(xi.mod.ACC, -plan.accuracyDelta)
    else
        mob:addMod(xi.mod.EVA, plan.evasionDelta)
        mob:addMod(xi.mod.ACC, plan.accuracyDelta)
    end
    mob:setMod(xi.mod.DELAY, plan.delay)
    mob:setMobMod(xi.mobMod.SKILL_LIST, plan.skillList)
    mob:setLocalVar('Phase', plan.phase)
end

local function updatePhase(mob)
    local plan = xi.mix.ladybug.phasePlan(
        VanadielHour(),
        mob:getLocalVar('Phase'),
        mob:getLocalVar('[ladybug]daySkillList'),
        mob:getLocalVar('[ladybug]nightSkillList'))
    if plan then
        applyPlan(mob, plan)
    end
end

xi.mix.ladybug.config = function(mob, params)
    if params.nightTime and type(params.nightTime) == 'number' then
        mob:setLocalVar('[ladybug]nightTime', params.nightTime)
    end

    if params.morning and type(params.morning) == 'number' then
        mob:setLocalVar('[ladybug]morning', params.morning)
    end
end

g_mixins.families.ladybug = function(mob)
    mob:addListener('SPAWN', 'LADYBUG_SPAWN', function(ladybug)
        ladybug:setLocalVar('[ladybug]daySkillList', 170)
        ladybug:setLocalVar('[ladybug]nightSkillList', 1173)
    end)

    mob:addListener('ROAM_TICK', 'LADYBUG_ROAM_TICK', function(ladybug)
        updatePhase(ladybug)
    end)

    mob:addListener('COMBAT_TICK', 'LADYBUG_COMBAT_TICK', function(ladybug)
        updatePhase(ladybug)
    end)
end

return g_mixins.families.ladybug
