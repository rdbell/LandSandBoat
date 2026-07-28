-----------------------------------
--- Zdei Family Mixin
--  https://ffxiclopedia.fandom.com/wiki/Category:Zdei
--  https://www.bg-wiki.com/ffxi/Category:Zdei
--
--  Zdei swap forms every 60 seconds, the swap appears random between Bars, Rings, and Pot.
--
--  Animation Sub 0 Pot Form
--  Animation Sub 1 Pot Form (reverse eye position)
--  Animation Sub 2 Bar Form
--  Animation Sub 3 Ring Form
-----------------------------------
require('scripts/globals/mixins')
-----------------------------------

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.zdei = xi.mix.zdei or {}

local rotationPools =
{
    [xi.mobPool.EOZDEI_LEFT]   = -16,
    [xi.mobPool.EOZDEI_RIGHT]  =  16,
    [xi.mobPool.AWZDEI_LEFT]   = -16,
    [xi.mobPool.AWZDEI_RIGHT]  =  16,
    [xi.mobPool.AWZDEI_FAST_L] = -32,
    [xi.mobPool.AWZDEI_FAST_R] =  32,
}

xi.mix.zdei.rotationOffset = function(poolId)
    return rotationPools[poolId]
end

xi.mix.zdei.engagePlan = function(now, sampledDelay)
    return { animationSub = 1, changeTime = now + sampledDelay }
end

xi.mix.zdei.chargePlan = function(chargeCount, chargeTotal, sampledChargeTotal)
    if chargeTotal > 0 and chargeCount == chargeTotal then
        return { useFinal = true }
    end

    local plan = { useCharge = true, chargeCount = chargeCount + 1 }
    if chargeCount == 0 then
        plan.autoAttack = false
        plan.magicCasting = false
        plan.chargeTotal = sampledChargeTotal
    end
    return plan
end

xi.mix.zdei.finishPlan = function()
    return { autoAttack = true, magicCasting = true, chargeCount = 0, chargeTotal = 0 }
end

xi.mix.zdei.shouldChangeForm = function(now, changeTime, basicAttack, chargeCount)
    return now >= changeTime and basicAttack and chargeCount == 0
end

xi.mix.zdei.formChangePlan = function(now, newSub)
    return { animationSub = newSub, changeTime = now + 60 }
end

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

g_mixins.families.zdei = function(zdeiMob)
    zdeiMob:addListener('SPAWN', 'ZDEI_SPAWN', function(mob)
        mob:setAnimationSub(0)
        mob:addMod(xi.mod.MDEF, 20) -- Zdei have innate +20 MDEF

        -- Store the rotation offset for use in onPath
        local poolId = mob:getPool()
        local rotOffset = xi.mix.zdei.rotationOffset(poolId)
        if rotOffset then
            mob:setLocalVar('zdeiRotationOffset', rotOffset)
        end
    end)

    zdeiMob:addListener('ENGAGE', 'ZDEI_ENGAGE', function(mob, target)
        local plan = xi.mix.zdei.engagePlan(GetSystemTime(), math.random(15, 30))
        mob:setAnimationSub(plan.animationSub)
        mob:setLocalVar('changeTime', plan.changeTime)
    end)

    zdeiMob:addListener('WEAPONSKILL_STATE_EXIT', 'ZDEI_WS_EXIT', function(mob, skillId, wasExecuted)
        if skillId == xi.mobSkill.OPTIC_INDURATION_CHARGE then
            local chargeCount = mob:getLocalVar('chargeCount')
            local chargeTotal = mob:getLocalVar('chargeTotal')

            local plan = xi.mix.zdei.chargePlan(chargeCount, chargeTotal, math.random(3, 5))
            if plan.useFinal then
                mob:useMobAbility(xi.mobSkill.OPTIC_INDURATION, mob:getTarget())
            else
                if plan.chargeTotal then
                    mob:setAutoAttackEnabled(plan.autoAttack)
                    mob:setMagicCastingEnabled(plan.magicCasting)
                    mob:setLocalVar('chargeTotal', plan.chargeTotal)
                end

                mob:setLocalVar('chargeCount', plan.chargeCount)
                mob:useMobAbility(xi.mobSkill.OPTIC_INDURATION_CHARGE)
            end

        elseif skillId == xi.mobSkill.OPTIC_INDURATION then
            local plan = xi.mix.zdei.finishPlan()
            mob:setAutoAttackEnabled(plan.autoAttack)
            mob:setMagicCastingEnabled(plan.magicCasting)
            mob:setLocalVar('chargeCount', plan.chargeCount)
            mob:setLocalVar('chargeTotal', plan.chargeTotal)
        end
    end)

    zdeiMob:addListener('DISENGAGE', 'ZDEI_DISENGAGE', function(mob)
        mob:setAnimationSub(0)
        mob:setLocalVar('changeTime', 0)
    end)

    zdeiMob:addListener('COMBAT_TICK', 'ZDEI_CTICK', function(mob)
        local changeTime = mob:getLocalVar('changeTime')
        local now = GetSystemTime()

        -- Change to a new mode if time has expired and not currently charging optic induration
        if xi.mix.zdei.shouldChangeForm(now, changeTime, mob:getCurrentAction() == xi.action.category.BASIC_ATTACK, mob:getLocalVar('chargeCount')) then
            local newSub = math.random(1, 3)
            while newSub == mob:getAnimationSub() do
                newSub = math.random(1, 3)
            end

            local plan = xi.mix.zdei.formChangePlan(now, newSub)
            mob:setAnimationSub(plan.animationSub)
            mob:setLocalVar('changeTime', plan.changeTime)
        end
    end)
end

return g_mixins.families.zdei
