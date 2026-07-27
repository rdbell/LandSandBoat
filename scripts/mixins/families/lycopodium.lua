require('scripts/globals/mixins')
-----------------------------------
-- TODO: Lycopodiums should use a regen move on players with the title "Babban's Traveling Companion"

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.lycopodium = xi.mix.lycopodium or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.lycopodium.spawnPlan = function()
    return { setCombat = true, combatEnabled = false, alwaysAggro = true }
end

xi.mix.lycopodium.roamPlan = function(hpp, isAlly)
    if hpp ~= 100 then
        return nil
    end

    return { setCombat = true, combatEnabled = false, resetDamaged = not isAlly }
end

xi.mix.lycopodium.disengagePlan = function(isAlly)
    if isAlly then
        return nil
    end

    return { setCombat = true, combatEnabled = false }
end

xi.mix.lycopodium.engagePlan = function(battleTime, isAlly)
    return {
        disengageTime = battleTime + 45,
        setCombat = isAlly,
        combatEnabled = true,
    }
end

xi.mix.lycopodium.combatPlan = function(isAlly, hp, maxHP, battleTime, disengageTime, damaged)
    if isAlly or hp < maxHP then
        return { setCombat = true, combatEnabled = true }
    end

    if disengageTime > 0 and battleTime > disengageTime and damaged == 0 then
        return { disengageTime = 0, disengage = true }
    end

    return nil
end

xi.mix.lycopodium.damagePlan = function(isAlly)
    return { setCombat = true, combatEnabled = true, markDamaged = not isAlly }
end

local function applyPlan(mob, plan)
    if plan.setCombat then
        mob:setAutoAttackEnabled(plan.combatEnabled)
        mob:setMobAbilityEnabled(plan.combatEnabled)
    end

    if plan.alwaysAggro then
        mob:setMobMod(xi.mobMod.ALWAYS_AGGRO, 1)
    end

    if plan.resetDamaged then
        mob:setLocalVar('[lycopodium]damaged', 0)
    elseif plan.markDamaged then
        mob:setLocalVar('[lycopodium]damaged', 1)
    end

    if plan.disengageTime ~= nil then
        mob:setLocalVar('[lycopodium]disengageTime', plan.disengageTime)
    end

    if plan.disengage then
        mob:disengage()
    end
end

g_mixins.families.lycopodium = function(mob)
    mob:addListener('SPAWN', 'LYCOPODIUM_SPAWN', function(lycopodium)
        applyPlan(lycopodium, xi.mix.lycopodium.spawnPlan())
    end)

    mob:addListener('ROAM_TICK', 'LYCOPODIUM_RTICK', function(lycopodium)
        local plan = xi.mix.lycopodium.roamPlan(lycopodium:getHPP(), lycopodium:isAlly())
        if plan then
            applyPlan(lycopodium, plan)
        end
    end)

    mob:addListener('DISENGAGE', 'LYCOPODIUM_DISENGAGE', function(lycopodium)
        local plan = xi.mix.lycopodium.disengagePlan(lycopodium:isAlly())
        if plan then
            applyPlan(lycopodium, plan)
        end
    end)

    mob:addListener('ENGAGE', 'LYCOPODIUM_ENGAGE', function(lycopodium)
        applyPlan(lycopodium, xi.mix.lycopodium.engagePlan(lycopodium:getBattleTime(), lycopodium:isAlly()))
    end)

    mob:addListener('COMBAT_TICK', 'LYCOPODIUM_CTICK', function(lycopodium)
        local plan = xi.mix.lycopodium.combatPlan(
            lycopodium:isAlly(),
            lycopodium:getHP(),
            lycopodium:getMaxHP(),
            lycopodium:getBattleTime(),
            lycopodium:getLocalVar('[lycopodium]disengageTime'),
            lycopodium:getLocalVar('[lycopodium]damaged'))
        if plan then
            applyPlan(lycopodium, plan)
        end
    end)

    mob:addListener('TAKE_DAMAGE', 'LYCOPODIUM_DAMAGE', function(lycopodium)
        applyPlan(lycopodium, xi.mix.lycopodium.damagePlan(lycopodium:isAlly()))
    end)
end

return g_mixins.families.lycopodium
